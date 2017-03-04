/*
 ------------------------------------------------------------------------------
 INGENICO Technical Software Department
 ------------------------------------------------------------------------------
 Copyright (c) 2015-2016 INGENICO S.A.
 28-32 boulevard de Grenelle 75015 Paris, France.
 All rights reserved.
 This source program is the property of the INGENICO Company mentioned above
 and may not be copied in any form or by any means, whether in part or in whole,
 except under license expressly granted by such INGENICO company.
 All copies of this source program, whether in part or in whole, and
 whether modified or not, must display this and all other
 embedded copyright and ownership notices in full.
 ------------------------------------------------------------------------------
 */
#include <bdor/completion_online_workflow.h>
#include <utils/logger.h>
#include "online_pin_entry.h"
#include "remove_chip_card.h"
#include "floor_limit.h"
#include "reversal_processing.h"
#include "workflow_activities.h"
#include "post_online_activities.h"
#include "show_transaction_info.h"
#include "app_counter.h"
#include <bdor/transaction_type_name.h>
#include <bdor/copy_type.h>
#include <bdor/reprint_type.h>

namespace bdor {

CompletionOnlineWorkflow::CompletionOnlineWorkflow(
    ApplicationSettings& app_settings,
    Batch& batch,
    HostSwitch& host,
    emv::EmvConfig& emv_config,
    contactless::ClessConfig& cless_config,
    Ui& ui,
    Receipts& receipts,
    tpcore::PinEntry& pin_entry)
    :
      app_settings_(app_settings),
      batch_(batch),
      host_(host),
      emv_config_(emv_config),
      cless_config_(cless_config),
      ui_(ui),
      receipts_(receipts),
      pin_entry_(pin_entry),
      card_presenter_(app_settings, cless_config, ui) {
}

void CompletionOnlineWorkflow::Start(
    types::PosConditionCode pos_condition_mode) {
  logger::debug("Starting Pre-auth completion Transaction");

  if (IsMemoryFull()) {
    return ui_.MessageBatchFull();
  }

  Transaction tx;

  tx.transaction_type = TransactionType::PREAUTH_COMPLETION_ONLINE;
  tx.pos_condition_code = pos_condition_mode;
  tx.response_code = "00";  //TODO: to confirm

  TerminalConfiguration terminal_config =
      app_settings_.GetTerminalConfiguration();

  if (!IsTxEnabled(terminal_config, tx)) {
    return ui_.MessageTransactionNotAllowed();
  }

  if (!RequestPreauthAmount(app_settings_, ui_, tx))
    return;

  if (!RequestCompletionAmount(tx))
    return;

  if (!ui_.RequestAuthorizationCode(tx.auth_id_response_original))
    return;

  tx.auth_id_response = tx.auth_id_response_original;

  if (!ui_.RequestInvoice(tx.original_invoice_number))
     return;

  bool flowStatusOk;
  if (tx.pos_condition_code == types::PosConditionCode::MOTO_PREAUTH) {
    flowStatusOk = card_presenter_.PromptManualEntryForMoto(this, tx);
  }
  else {
    flowStatusOk = card_presenter_.PromptCardToStart(this, tx);
  }

  if (!flowStatusOk) {
    WaitForCardRemoval(ui_);
    return;
  }

  if (!IsTxnEnabledPerCard(tx))
    return;

  if (ProcessCompletionSaleOnline(tx) == TransactionStatus::APPROVED) {
    host_.Disconnect();
    ui_.MessageApproved(tx.auth_id_response);
    RunPrintReceiptFlow(tx);
  } else {
    host_.Disconnect();
  }
  WaitForCardRemoval(ui_);
}

WorkflowStatus CompletionOnlineWorkflow::RunManualFlow(Transaction& tx) {

  ManualWorkflow manual_workflow(ui_, app_settings_);
  stdx::optional<CardDefinition> card_definition_config;
  stdx::optional<IssuerDefinition> issuer_definition_config;
  stdx::optional<HostDefinition> host_definition_config;

  while (true) {
    if (!manual_workflow.RequestPan(tx.pan))
      return WorkflowStatus::MANUAL_TIMEOUT_OR_CANCEL;

    if (!RetrieveConfigsFromPan(app_settings_,
                                *tx.pan,
                                card_definition_config,
                                issuer_definition_config,
                                host_definition_config,
                                tx)) {
      ui_.MessageUnsupportedCard();
      continue;
    }

    if (!IsValidPAN(*card_definition_config, *tx.pan)) {
      ui_.MessageInvalidPan();
      continue;
    }

    break;
  }

  if (!manual_workflow.AskAndValidateExpiryDate(tx.expiration_date))
    return WorkflowStatus::MANUAL_TIMEOUT_OR_CANCEL;

  manual_workflow.ReadTransactionData(&tx);

  if (!card_definition_config->manual_entry_enabled) {
    ui_.MessageManualNotAllowedForThisCard();
    return WorkflowStatus::MANUAL_ERROR;
  }

  SetTransactionDataFromConfigs(*card_definition_config,
                                *issuer_definition_config,
                                *host_definition_config, &tx);

  tx.online_authorization_required = true;

  return WorkflowStatus::MANUAL_SUCCESS;
}

WorkflowStatus CompletionOnlineWorkflow::RunMagFlow(
                                                    types::MagStripe mag_stripe,
                                                    Transaction& tx) {

  MagWorkflow mag_workflow(ui_, app_settings_);

  ui_.MessageProcessingCard();
  if (!mag_workflow.PerformCardValidation(mag_stripe)) {
    ui_.MessageInvalidTrack2();
    return WorkflowStatus::MAG_ERROR;
  }

  if (mag_stripe.IsChipCard() && !tx.is_fallback) {
    ui_.MessagePleaseInsertCard();
    return WorkflowStatus::CHIP_CARD_SWIPED;
  }

  mag_workflow.ReadTransactionData(mag_stripe, &tx);

  stdx::optional<CardDefinition> card_definition_config;
  stdx::optional<IssuerDefinition> issuer_definition_config;
  stdx::optional<HostDefinition> host_definition_config;

  if (!RetrieveConfigsFromPan(app_settings_,
                              *tx.pan,
                              card_definition_config,
                              issuer_definition_config,
                              host_definition_config,
                              tx)) {
    ui_.MessageUnsupportedCard();
    return WorkflowStatus::MAG_ERROR;
  }

  if (!IsValidPAN(*card_definition_config, *tx.pan)) {
    ui_.MessageInvalidPan();
    return WorkflowStatus::MAG_ERROR;
  }

  if (!CheckCardExpired(ui_, tx.expiration_date))
    return WorkflowStatus::MAG_ERROR;

  SetTransactionDataFromConfigs(*card_definition_config,
                                *issuer_definition_config,
                                *host_definition_config, &tx);

  tx.online_authorization_required = true;

  if (tx.is_fallback)
    tx.pos_entry_mode = types::PosEntryMode::FALLBACK_MAGSTRIPE;
  else
    tx.pos_entry_mode = types::PosEntryMode::MAGSTRIPE;

  return WorkflowStatus::MAG_SUCCESS;
}

WorkflowStatus CompletionOnlineWorkflow::RunEmvFlow(Transaction& tx) {

  EmvWorkflow emv_workflow(ui_, app_settings_, emv_config_);

  auto emv_status = emv_workflow.ReadICCData(tx);
  if (emv_status != WorkflowStatus::EMV_SUCCESS)
    return emv_status;

  stdx::optional<CardDefinition> card_definition_config;
  stdx::optional<IssuerDefinition> issuer_definition_config;
  stdx::optional<HostDefinition> host_definition_config;

  if (!RetrieveConfigsFromPan(app_settings_,
                              *tx.pan,
                              card_definition_config,
                              issuer_definition_config,
                              host_definition_config,
                              tx)) {
    ui_.MessageUnsupportedCard();
    return WorkflowStatus::EMV_ERROR;
  }

  if (!IsValidPAN(*card_definition_config, *tx.pan)) {
    ui_.MessageInvalidPan();
    return WorkflowStatus::EMV_ERROR;
  }

  SetTransactionDataFromConfigs(*card_definition_config,
                                *issuer_definition_config,
                                *host_definition_config, &tx);

  tx.pos_entry_mode = types::PosEntryMode::CHIP;

  if (emv_workflow.FullEMVProcessingBeforeOnline(pin_entry_, tx, nullptr,
                                                 EmvForceAction::ONLINE)
      != WorkflowStatus::EMV_SUCCESS) {
    ui_.DisplayEMVProcessingError(tx.transaction_status);
    return WorkflowStatus::EMV_ERROR;
  }
  tx.online_authorization_required = true;
  return WorkflowStatus::EMV_SUCCESS;
}

WorkflowStatus CompletionOnlineWorkflow::RunClessFlow(
    ContactlessWorkflow& cless_workflow,
    Transaction& tx) {
  ui_.MessagePleaseUseOtherInterfaces();
  return WorkflowStatus::CLESS_FALLBACK_NEEDED;
}

TransactionStatus CompletionOnlineWorkflow::ProcessPostOnlineNonFullEmv(
    const HostSwitch::Status& host_status,
    Transaction& tx) {
  if (host_status == HostSwitch::Status::TRANSIENT_FAILURE
      || host_status == HostSwitch::Status::PERM_FAILURE) {
    ui_.MessagePleaseTryAgain();
    return TransactionStatus::UNKNOWN_ERROR;
  }
  else if (host_status == HostSwitch::Status::COMPLETED
      && IsResponseCodeApproved(tx.response_code)) {
    tx.SetTransactionStatus(TransactionStatus::APPROVED);
    tx.is_preauth_completed = true;
    batch_.InsertOrUpdate(tx);

    UploadPendingItemsAfterTxnApproved(app_settings_, batch_, host_,
                                       tx.host_index);

    return TransactionStatus::APPROVED;
  }
  else {
    batch_.Delete(tx);
    ui_.DisplayResponseMessage(tx.response_code);
    return TransactionStatus::DECLINED;
  }

  return TransactionStatus::APPROVED;
}

TransactionStatus CompletionOnlineWorkflow::ProcessCompletionSaleOnline(
    Transaction& tx) {

  stdx::optional<HostDefinition> host_definition_config;
  host_definition_config =
      app_settings_.managed_settings_->GetHostDefinition(
                                                         tx.host_index);

  if (!host_definition_config.has_value()) {  // Invalid Host Index due to wrong settings
    ui_.MessageProcessAborted();
    return TransactionStatus::UNKNOWN_ERROR;
  }

  stdx::optional<CardDefinition> card_definition_config;
  card_definition_config = app_settings_.managed_settings_
      ->GetCardDefinition(tx.card_index);

  if (!card_definition_config.has_value()) {
    ui_.MessageProcessAborted();
    return TransactionStatus::UNKNOWN_ERROR;
  }

  if (!CheckAndOpenBatch(tx.host_index, ui_, batch_))
    return TransactionStatus::UNKNOWN_ERROR;

  host_.Disconnect();
  if (!host_.PreConnect(host_definition_config->index)) {  // Wrong comm config
    ui_.MessageCommunicationError();
    return TransactionStatus::UNKNOWN_ERROR;
  }

  if (!DisplayPanAndExpiryDate(app_settings_, &ui_, tx.issuer_index, *tx.pan,
                               tx.expiration_date)) {
    return TransactionStatus::UNKNOWN_ERROR;
  }

  ui_.MessageCommunicatingHost();

  if (!ProcessPendingReversals(batch_, host_, tx.host_index)) {
    ui_.MessagePleaseTryAgain();
    return TransactionStatus::UNKNOWN_ERROR;
  }

  tx.SetTransactionStatus(TransactionStatus::IN_PROGRESS);

  tx.stan_original = tx.stan = GetNextStanNo();
  tx.invoice_num = GetNextUniqueInvoice(batch_);
  tx.batch_num = batch_.GetCurrentBatchNumber(
                                              host_definition_config->index);

  if (!batch_.Insert(tx)) {
    ui_.MessageProcessAborted();  // Batch Error
    return TransactionStatus::UNKNOWN_ERROR;
  }

  HostSwitch::Status host_status;
  if (!app_settings_.GetTerminalConfiguration().DccEnabled()
      || !card_definition_config->dcc_enabled) {

    host_status = host_.AuthorizePreAuthCompletion(tx);
    return ProcessPostOnlineNonFullEmv(host_status, tx);
  }

  if (!RunDccFlow(app_settings_, ui_, batch_, host_,
                  tx,
                  &HostSwitch::AuthorizePreAuthCompletionWithDccEnquiry,
                  &HostSwitch::AuthorizePreAuthCompletionWithDccAllowed,
                  host_status)) {
    return TransactionStatus::CANCELLED;
  }

  return ProcessPostOnlineNonFullEmv(host_status, tx);

}

bool CompletionOnlineWorkflow::IsTxnEnabledPerCard(
    const Transaction& tx) const {

  stdx::optional<CardDefinition> card_config = app_settings_
      .managed_settings_
      ->GetCardDefinition(tx.card_index);

  if (!card_config.has_value()) {
    ui_.MessageCardError();
    return false;
  }

  if (!card_config->tx_preauth_completion_online_enabled) {
    ui_.MessageTransactionNotAllowed();
    return false;
  }
  return true;
}

bool CompletionOnlineWorkflow::VerifyPreAuthCompletionAmount(
    const types::Amount& preauth_amount,
    const types::Amount& preauth_completion_amount,
    const uint64_t allowed_variance) const {
  auto variance = (preauth_amount.GetValue() * allowed_variance) / 100;

  types::Amount variance_amount(preauth_amount.GetCurrencyName().c_str(),
                                variance);

  if (preauth_completion_amount > preauth_amount + variance_amount)
    return false;

  return true;
}

bool CompletionOnlineWorkflow::RequestCompletionAmount(Transaction& tx) {

  std::string currency_name = app_settings_.managed_settings_->terminal_config
      .TerminalBaseCurrency().GetName();

  while (true) {
    types::Amount completion_amount(currency_name.c_str(), 0);
    if (!ui_.RequestPreAuthCompletionAmount(completion_amount))
      return false;

    if (!IsValidAmount(app_settings_, completion_amount)) {
      ui_.MessageAmountOutOfRange();
      continue;
    }

    if (!VerifyPreAuthCompletionAmount(
        *tx.GetTotalPreauthAmount(),
        completion_amount,
        app_settings_.managed_settings_->terminal_config
            .PreAuthCompletionAllowedVariance())) {
      ui_.MessageAmountOutOfRange();
      continue;
    }
    tx.amount = completion_amount;
    break;
  }

  return true;
}

void CompletionOnlineWorkflow::RunPrintReceiptFlow(
    const Transaction& tx) const {

  PrintReceipts(
      app_settings_,
      ui_,
      tx.transaction_status,
      *receipts_.GetReversalReceipt(app_settings_, tx),
      *receipts_.GetCompletionReceipt(app_settings_, tx, CopyType::MERCHANT,
                                      ReprintType::ORIGINAL),
      *receipts_.GetCompletionReceipt(app_settings_, tx, CopyType::CUSTOMER,
                                      ReprintType::ORIGINAL),
      *receipts_.GetCompletionReceipt(app_settings_, tx, CopyType::MERCHANT,
                                      ReprintType::DUPLICATE),
      *receipts_.GetCompletionReceipt(app_settings_, tx, CopyType::CUSTOMER,
                                      ReprintType::DUPLICATE)
                                      );
}

}
