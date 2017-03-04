/*
 ------------------------------------------------------------------------------
 INGENICO Technical Software Department
 ------------------------------------------------------------------------------
 Copyright (c) 2016 INGENICO S.A.
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
#include <bdor/preauth_cancellation_workflow.h>
#include <utils/logger.h>
#include <bdor/transaction.h>
#include <bdor/receipts.h>
#include <bdor/transaction_type_name.h>
#include "show_transaction_info.h"
#include "workflow_activities.h"
#include "post_online_activities.h"
#include "reversal_processing.h"
#include "advice_processing.h"
#include "app_counter.h"
#include <bdor/copy_type.h>
#include <bdor/reprint_type.h>
#include "remove_chip_card.h"
#include <bdor/emv_workflow.h>
#include <bdor/mag_workflow.h>
#include <bdor/manual_workflow.h>

namespace bdor {

PreAuthCancellationWorkflow::PreAuthCancellationWorkflow(
    ApplicationSettings& app_settings,
    Batch& batch,
    HostSwitch& host,
    emv::EmvConfig& emv_config,
    contactless::ClessConfig& cless_config,
    tpcore::PinEntry& pin_entry,
    Ui& ui,
    Receipts& receipts)
    : app_settings_(app_settings),
      batch_(batch),
      host_(host),
      emv_config_(emv_config),
      cless_config_(cless_config),
      pin_entry_(pin_entry),
      ui_(ui),
      receipts_(receipts),
      card_presenter_(app_settings, cless_config, ui) {
}

void PreAuthCancellationWorkflow::Start(
    types::PosConditionCode pos_condition_mode) {
  logger::debug("Starting Void Preauth Transaction");

  if (IsMemoryFull()) {
    return ui_.MessageBatchFull();
  }

  Transaction tx;

  tx.transaction_type = TransactionType::PREAUTH_CANCELLATION;
  tx.pos_condition_code = pos_condition_mode;

  TerminalConfiguration terminal_config =
      app_settings_.GetTerminalConfiguration();

  if (!IsTxEnabled(terminal_config, tx)) {
    return ui_.MessageTransactionNotAllowed();
  }

  if (!RequestPreauthAmount(app_settings_, ui_, tx))
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

  if (ProcessPreAuthCancellationOnline(tx)
      == TransactionStatus::APPROVED) {
    host_.Disconnect();
    RunPrintReceiptFlow(tx);
    batch_.Delete(tx);
  }
  host_.Disconnect();
}

WorkflowStatus PreAuthCancellationWorkflow::RunManualFlow(Transaction& tx) {

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

  tx.batch_num = batch_.GetCurrentBatchNumber(host_definition_config->index);

  return WorkflowStatus::MANUAL_SUCCESS;
}

WorkflowStatus PreAuthCancellationWorkflow::RunMagFlow(
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

  tx.batch_num = batch_.GetCurrentBatchNumber(host_definition_config->index);

  return WorkflowStatus::MAG_SUCCESS;
}

WorkflowStatus PreAuthCancellationWorkflow::RunEmvFlow(Transaction& tx) {

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

  tx.batch_num = batch_.GetCurrentBatchNumber(host_definition_config->index);

  if (emv_workflow.FullEMVProcessingBeforeOnline(pin_entry_, tx, nullptr,
                                                 EmvForceAction::ONLINE)
      != WorkflowStatus::EMV_SUCCESS) {
    ui_.DisplayEMVProcessingError(tx.transaction_status);
    return WorkflowStatus::EMV_ERROR;
  }
  tx.online_authorization_required = true;
  return WorkflowStatus::EMV_SUCCESS;
}

WorkflowStatus PreAuthCancellationWorkflow::RunClessFlow(
    ContactlessWorkflow& cless_workflow,
    Transaction& tx) {
  ui_.MessagePleaseUseOtherInterfaces();
  return WorkflowStatus::CLESS_FALLBACK_NEEDED;
}

TransactionStatus PreAuthCancellationWorkflow::ProcessPreAuthCancellationOnline(
    Transaction& tx) {

  stdx::optional<HostDefinition> host_definition_config;
  host_definition_config =
      app_settings_.managed_settings_->GetHostDefinition(
                                                         tx.host_index);

  if (!host_definition_config.has_value()) {  // Invalid Host Index due to wrong settings
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

  if (!DisplayPanAndExpiryDate(app_settings_, &ui_,
                               tx.issuer_index,
                               *tx.pan,
                               tx.expiration_date)) {
    tx.transaction_status = TransactionStatus::CANCELLED;
    return tx.transaction_status;
  }

  ui_.MessageCommunicatingHost();

  if (!ProcessPendingReversals(batch_, host_, tx.host_index)) {
    ui_.MessagePleaseTryAgain();
    return TransactionStatus::UNKNOWN_ERROR;
  }

  tx.SetTransactionStatus(TransactionStatus::IN_PROGRESS);

  tx.invoice_num = GetNextUniqueInvoice(batch_);
  tx.batch_num = batch_.GetCurrentBatchNumber(
                                              host_definition_config->index);
  tx.stan = GetNextStanNo();  // get STAN after processing reversal, in case if acq host requires increase STAN for every reversal attempt.

  if (!batch_.Insert(tx)) {
    SetInvoiceNo(tx.invoice_num);
    ui_.MessageProcessAborted();  // Batch Error
    return TransactionStatus::UNKNOWN_ERROR;
  }

  HostSwitch::Status host_status = host_.PerformFdmsPreAuthCancellation(tx);

  if (host_status == HostSwitch::Status::TRANSIENT_FAILURE
      || host_status == HostSwitch::Status::PERM_FAILURE) {
    ui_.MessagePleaseTryAgain();
    return TransactionStatus::UNKNOWN_ERROR;
  }
  else if (host_status == HostSwitch::Status::COMPLETED
      && IsResponseCodeApproved(tx.response_code)) {
    tx.SetTransactionStatus(TransactionStatus::APPROVED);
    tx.is_voided = true;
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

void PreAuthCancellationWorkflow::RunPrintReceiptFlow(const Transaction& tx) {

  PrintReceipts(
                app_settings_,
                ui_,
                tx.transaction_status,
                *receipts_.GetReversalReceipt(app_settings_, tx),
                *receipts_.GetVoidReceipt(app_settings_, tx, CopyType::MERCHANT,
                                          ReprintType::ORIGINAL),
                *receipts_.GetVoidReceipt(app_settings_, tx, CopyType::CUSTOMER,
                                          ReprintType::ORIGINAL),
                *receipts_.GetVoidReceipt(app_settings_, tx, CopyType::MERCHANT,
                                          ReprintType::DUPLICATE),
                *receipts_.GetVoidReceipt(app_settings_, tx, CopyType::CUSTOMER,
                                          ReprintType::DUPLICATE)
                                          );
}

bool PreAuthCancellationWorkflow::IsTxnEnabledPerCard(
    const Transaction& tx) const {

  stdx::optional<CardDefinition> card_config = app_settings_
      .managed_settings_
      ->GetCardDefinition(tx.card_index);

  if (!card_config.has_value()) {
    ui_.MessageCardError();
    return false;
  }

  if (!card_config->tx_void_preauth_enabled) {
    ui_.MessageTransactionNotAllowed();
    return false;
  }
  return true;
}

}
