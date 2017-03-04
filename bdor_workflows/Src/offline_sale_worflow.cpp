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
#include <bdor/offline_sale_workflow.h>

#include <utils/logger.h>
#include "remove_chip_card.h"
#include "workflow_activities.h"
#include "app_counter.h"
#include <bdor/copy_type.h>
#include <bdor/reprint_type.h>

namespace bdor {

OfflineSaleWorkflow::OfflineSaleWorkflow(ApplicationSettings& app_setting,
                                         Batch& batch,
                                         emv::EmvConfig& emv_config,
                                         contactless::ClessConfig& cless_config,
                                         Ui& ui,
                                         Receipts& receipts)
    : app_settings_(app_setting),
      batch_(batch),
      emv_config_(emv_config),
      cless_config_(cless_config),
      ui_(ui),
      receipts_(receipts),
      manual_workflow_(ui_, app_settings_),
      mag_workflow_(ui_, app_settings_),
      card_presenter_(app_setting, cless_config, ui) {
}

void OfflineSaleWorkflow::Start() {
  logger::debug("Starting Offline Sale Transaction");

  if (IsMemoryFull()) {
    return ui_.MessageBatchFull();
  }

  Transaction tx;
  tx.transaction_type = TransactionType::OFFLINE_SALE;
  tx.response_code = "00";

  if (!IsTxEnabled(app_settings_.GetTerminalConfiguration(), tx)) {
    return ui_.MessageTransactionNotAllowed();
  }

  if (!RequestTxAmount(tx))
    return;

  bool flowStatusOk = card_presenter_.PromptCardToStart(this, tx);

  if (!flowStatusOk) {
    WaitForCardRemoval(ui_);
    return;
  }

  if (!IsTxnEnabledPerCard(tx))
    return;

  if (ProcessOfflineSale(tx) == TransactionStatus::APPROVED) {
    if (tx.signature_required)
      ui_.RequestSignature(*tx.GetTotalAmount(),
                           tx.signature_file_path);

    ui_.MessageApproved(tx.auth_id_response);
    RunPrintReceiptFlow(tx);
  }
  WaitForCardRemoval(ui_);
}

WorkflowStatus OfflineSaleWorkflow::RunManualFlow(Transaction& tx) {
  stdx::optional<CardDefinition> card_definition_config;
  stdx::optional<IssuerDefinition> issuer_definition_config;
  stdx::optional<HostDefinition> host_definition_config;

  while (true) {
    if (!manual_workflow_.RequestPan(tx.pan))
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

  if (!card_definition_config->tx_offline_sale_enabled) {
    ui_.MessageTransactionNotAllowed();
    return WorkflowStatus::MANUAL_ERROR;
  }

  if (!card_definition_config->manual_entry_enabled) {
    ui_.MessageManualNotAllowedForThisCard();
    return WorkflowStatus::MANUAL_ERROR;
  }

  if (!manual_workflow_.AskAndValidateExpiryDate(tx.expiration_date))
    return WorkflowStatus::MANUAL_TIMEOUT_OR_CANCEL;

  manual_workflow_.ReadTransactionData(&tx);

  SetTransactionDataFromConfigs(*card_definition_config,
                                *issuer_definition_config,
                                *host_definition_config, &tx);

  tx.online_authorization_required = false;
  tx.pin_entered = false;

  return WorkflowStatus::MANUAL_SUCCESS;
}

WorkflowStatus OfflineSaleWorkflow::RunMagFlow(types::MagStripe mag_stripe,
                                               Transaction& tx) {
  ui_.MessageProcessingCard();
  if (!mag_workflow_.PerformCardValidation(mag_stripe)) {
    ui_.MessageInvalidTrack2();
    return WorkflowStatus::MAG_ERROR;
  }

  if (mag_stripe.IsChipCard() && !tx.is_fallback) {
      ui_.MessagePleaseInsertCard();
      return WorkflowStatus::CHIP_CARD_SWIPED;
    }

  stdx::optional<CardDefinition> card_definition_config;
  stdx::optional<IssuerDefinition> issuer_definition_config;
  stdx::optional<HostDefinition> host_definition_config;

  mag_workflow_.ReadTransactionData(mag_stripe, &tx);

  if (!RetrieveConfigsFromPan(app_settings_,
                              *tx.pan,
                              card_definition_config,
                              issuer_definition_config,
                              host_definition_config,
                              tx)) {
    ui_.MessageUnsupportedCard();
    return WorkflowStatus::MAG_ERROR;
  }

  if (!card_definition_config->tx_offline_sale_enabled) {
    ui_.MessageTransactionNotAllowed();
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

  tx.online_authorization_required = false;
  tx.pin_entered = false;

  if (tx.is_fallback)
      tx.pos_entry_mode = types::PosEntryMode::FALLBACK_MAGSTRIPE;
    else
      tx.pos_entry_mode = types::PosEntryMode::MAGSTRIPE;

  return WorkflowStatus::MAG_SUCCESS;
}

WorkflowStatus OfflineSaleWorkflow::RunEmvFlow(Transaction& tx) {

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

  tx.online_authorization_required = false;
  tx.signature_required = true;

  return WorkflowStatus::EMV_SUCCESS;
}

WorkflowStatus OfflineSaleWorkflow::RunClessFlow(
    ContactlessWorkflow& cless_workflow, Transaction& tx) {

  auto cless_tx_status = cless_workflow.ActivateTransaction(tx);
  if (cless_tx_status != WorkflowStatus::CLESS_SUCCESS)
    return cless_tx_status;

  stdx::optional<CardDefinition> card_definition_config;
  stdx::optional<IssuerDefinition> issuer_definition_config;
  stdx::optional<HostDefinition> host_definition_config;

  if (!RetrieveConfigsFromPan(app_settings_, *tx.pan,
                              card_definition_config,
                              issuer_definition_config,
                              host_definition_config,
                              tx)) {
    ui_.MessageUnsupportedCard();
    return WorkflowStatus::CLESS_ERROR;
  }

  if (!IsValidPAN(*card_definition_config, *tx.pan)) {
    ui_.MessageInvalidPan();
    return WorkflowStatus::CLESS_ERROR;
  }

  SetTransactionDataFromConfigs(*card_definition_config,
                                *issuer_definition_config,
                                *host_definition_config, &tx);

  tx.online_authorization_required = false;
  tx.signature_required = true;
  tx.icc_data.clear();

  return WorkflowStatus::CLESS_SUCCESS;
}

TransactionStatus OfflineSaleWorkflow::ProcessOfflineSale(Transaction& tx) {

  stdx::optional<HostDefinition> host_definition_config;
  host_definition_config =
      app_settings_.managed_settings_->GetHostDefinition(
                                                         tx.host_index);

  if (!host_definition_config.has_value()) {  // Invalid Host Index due to wrong settings
    ui_.MessageHostError();
    return TransactionStatus::TERMINAL_ERROR;
  }

  if (!CheckAndOpenBatch(tx.host_index, ui_, batch_))
    return TransactionStatus::TERMINAL_ERROR;

  if (!DisplayPanAndExpiryDate(app_settings_, &ui_, tx.issuer_index, *tx.pan,
                               tx.expiration_date)) {
    return TransactionStatus::CANCELLED;
  }

  std::string auth_code = host_definition_config->pre_defined_auth_code;
  if(auth_code.empty()){
    if (!GetAuthCode(tx.auth_id_response))
      return TransactionStatus::CANCELLED;
  }

  tx.stan_original = tx.stan = GetNextStanNo();
  tx.invoice_num = GetNextUniqueInvoice(batch_);
  tx.batch_num = batch_.GetCurrentBatchNumber(host_definition_config->index);
  tx.SetTransactionStatus(TransactionStatus::TO_ADVISE);
  if (!batch_.Insert(tx)) {
    ui_.MessageProcessAborted();  // Batch Error
    return TransactionStatus::TERMINAL_ERROR;
  }

  return TransactionStatus::APPROVED;
}

bool OfflineSaleWorkflow::IsTxnEnabledPerCard(const Transaction& tx) const {

  stdx::optional<CardDefinition> card_config = app_settings_
      .managed_settings_
      ->GetCardDefinition(tx.card_index);

  if (!card_config.has_value()) {
    ui_.MessageCardError();
    return false;
  }

  if (!card_config->tx_offline_sale_enabled) {
    ui_.MessageTransactionNotAllowed();
    return false;
  }
  return true;
}

bool OfflineSaleWorkflow::IsValidOfflineSaleAmount(
    const types::Amount& amount) const {
  types::Amount max_amount(
      amount.GetCurrencyName().c_str(),
      app_settings_.managed_settings_->terminal_config.OfflineSaleMaxAmount());

  return amount <= max_amount;
}

void OfflineSaleWorkflow::RunPrintReceiptFlow(
                                              const Transaction& tx) const {

  PrintReceipts(
      app_settings_, ui_,
      tx.transaction_status,
      *receipts_.GetReversalReceipt(app_settings_, tx),
      *receipts_.GetOfflineSaleReceipt(app_settings_, tx, CopyType::MERCHANT,
                                       ReprintType::ORIGINAL),
      *receipts_.GetOfflineSaleReceipt(app_settings_, tx, CopyType::CUSTOMER,
                                       ReprintType::ORIGINAL),
      *receipts_.GetOfflineSaleReceipt(app_settings_, tx, CopyType::MERCHANT,
                                       ReprintType::ORIGINAL),
      *receipts_.GetOfflineSaleReceipt(app_settings_, tx, CopyType::CUSTOMER,
                                       ReprintType::ORIGINAL)
                                       );
}

bool OfflineSaleWorkflow::GetAuthCode(std::string& auth_code) {
  if (!ui_.RequestAuthorizationCode(auth_code))
    return false;

  return true;
}

bool OfflineSaleWorkflow::RequestTxAmount(Transaction& tx) {
  auto tip_mode =
      app_settings_.managed_settings_->terminal_config.TipProcessing();

  while (true) {
    tx.amount.reset();
    tx.secondary_amount.reset();
    if (!RequestAmount(app_settings_, ui_, tip_mode, tx))
      return false;

    if (!IsValidAmount(app_settings_, *tx.GetTotalAmount())) {
      ui_.MessageAmountOutOfRange();
      continue;
    }

    if (!IsValidOfflineSaleAmount(*tx.GetTotalAmount())) {
      ui_.MessageAmountOutOfRange();
      continue;
    }
    break;
  }
  return true;
}
}

