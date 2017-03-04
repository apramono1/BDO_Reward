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
#include <bdor/refund_workflow.h>

#include <utils/logger.h>
#include "online_pin_entry.h"
#include "remove_chip_card.h"
#include "floor_limit.h"
#include "reversal_processing.h"
#include "workflow_activities.h"
#include "post_online_activities.h"
#include "app_counter.h"
#include <types/pos_condition_code.h>
#include <bdor/copy_type.h>
#include <bdor/reprint_type.h>
#include <bdor/password.h>

namespace bdor {

RefundWorkflow::RefundWorkflow(ApplicationSettings& app_settings,
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

void RefundWorkflow::Start(types::PosConditionCode pos_condition_code) {
  logger::debug("Starting Refund Transaction");

  if (IsMemoryFull()) {
    return ui_.MessageBatchFull();
  }

  TerminalConfiguration terminal_config =
      app_settings_.GetTerminalConfiguration();

  if (!terminal_config.TxRefundEnabled()) {
    return ui_.MessageTransactionNotAllowed();
  }

  std::string currency_name = terminal_config
      .TerminalBaseCurrency().GetName();

  Transaction tx;
  tx.transaction_type = TransactionType::REFUND;
  tx.pos_condition_code = pos_condition_code;

  while (true) {
    types::Amount base_amount(currency_name.c_str(), 0);
    if (!ui_.RequestAmount(base_amount))
      return;

    tx.amount = base_amount;

    if (!IsValidAmount(app_settings_, *tx.GetTotalAmount())) {
      ui_.MessageAmountOutOfRange();
      continue;
    }

    if (!IsValidRefundAmount(app_settings_, *tx.GetTotalAmount())) {
      ui_.MessageAmountOutOfRange();
      continue;
    }

    break;
  }

  if (IsRefundPasswordRequired(*tx.GetTotalAmount())) {
    if (!VerifyRefundPassword(tx.transaction_type))
      return;
  }

  bool flowStatusOk;
  if (tx.pos_condition_code == types::PosConditionCode::MOTO) {
    flowStatusOk = card_presenter_.PromptManualEntryForMoto(this, tx);
  }
  else {
    flowStatusOk = card_presenter_.PromptCardToStart(this, tx);
  }

  if (!flowStatusOk) {
    host_.Disconnect();
    WaitForCardRemoval(ui_);
    return;
  }
  host_.Disconnect();

  if (tx.transaction_status == TransactionStatus::APPROVED)
    ui_.MessageApproved(tx.auth_id_response);

  PrintReceipts(
      app_settings_, ui_,
      tx.transaction_status,
      *receipts_.GetReversalReceipt(app_settings_, tx),
      *receipts_.GetRefundReceipt(app_settings_, tx, CopyType::MERCHANT,
                                  ReprintType::ORIGINAL),
      *receipts_.GetRefundReceipt(app_settings_, tx, CopyType::CUSTOMER,
                                  ReprintType::ORIGINAL),
      *receipts_.GetRefundReceipt(app_settings_, tx, CopyType::MERCHANT,
                                  ReprintType::DUPLICATE),
      *receipts_.GetRefundReceipt(app_settings_, tx, CopyType::CUSTOMER,
                                  ReprintType::DUPLICATE)
                                  );
  WaitForCardRemoval(ui_);
}

Transaction RefundWorkflow::Start(const types::Amount& amount,
                                  types::PosConditionCode pos_condition_code) {
  logger::debug("Starting Refund Transaction");

  Transaction tx;

  tx.transaction_type = TransactionType::REFUND;
  tx.pos_condition_code = pos_condition_code;

  if (IsMemoryFull()) {
    ui_.MessageBatchFull();
    return tx;
  }

  tx.amount = amount;

  if (!IsValidAmount(app_settings_, *tx.GetTotalAmount())) {
    ui_.MessageAmountOutOfRange();
    return tx;
  }

  if (!IsValidRefundAmount(app_settings_, *tx.GetTotalAmount())) {
    ui_.MessageAmountOutOfRange();
    return tx;
  }

  if (IsRefundPasswordRequired(*tx.GetTotalAmount())) {
    if (!VerifyRefundPassword(tx.transaction_type))
      return tx;
  }

  bool flowStatusOk;
  if (tx.pos_condition_code == types::PosConditionCode::MOTO) {
    flowStatusOk = card_presenter_.PromptManualEntryForMoto(this, tx);
  }
  else {
    flowStatusOk = card_presenter_.PromptCardToStart(this, tx);
  }

  if (!flowStatusOk) {
    host_.Disconnect();
    WaitForCardRemoval(ui_);
    return tx;
  }
  host_.Disconnect();

  if (tx.transaction_status == TransactionStatus::APPROVED)
    ui_.MessageApproved(tx.auth_id_response);

  PrintReceipts(
      app_settings_, ui_,
      tx.transaction_status,
      *receipts_.GetReversalReceipt(app_settings_, tx),
      *receipts_.GetRefundReceipt(app_settings_, tx, CopyType::MERCHANT,
                                  ReprintType::ORIGINAL),
      *receipts_.GetRefundReceipt(app_settings_, tx, CopyType::CUSTOMER,
                                  ReprintType::ORIGINAL),
      *receipts_.GetRefundReceipt(app_settings_, tx, CopyType::MERCHANT,
                                  ReprintType::DUPLICATE),
      *receipts_.GetRefundReceipt(app_settings_, tx, CopyType::CUSTOMER,
                                  ReprintType::DUPLICATE)
                                  );
  WaitForCardRemoval(ui_);
  return tx;
}

WorkflowStatus RefundWorkflow::RunManualFlow(Transaction& tx) {
  ManualWorkflow manual_workflow(ui_, app_settings_);

  stdx::optional<CardDefinition> card_definition_config;
  stdx::optional<IssuerDefinition> issuer_definition_config;
  stdx::optional<HostDefinition> host_definition_config;

  while (true) {
    if (!manual_workflow.RequestPan(tx.pan))
      return WorkflowStatus::MANUAL_TIMEOUT_OR_CANCEL;

    if (!RetrieveConfigsFromPan(app_settings_, *tx.pan, card_definition_config,
                                issuer_definition_config,
                                host_definition_config, tx)) {
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

  if (!IsTxEnabledPerCard(*card_definition_config, tx)) {
    ui_.MessageUnsupportedCard();
    return WorkflowStatus::MANUAL_ERROR;
  }

  if (!card_definition_config->manual_entry_enabled) {
    ui_.MessageManualNotAllowedForThisCard();
    return WorkflowStatus::MANUAL_ERROR;
  }

  SetTransactionDataFromConfigs(*card_definition_config,
                                *issuer_definition_config,
                                *host_definition_config, &tx);

  if (!CheckAndOpenBatch(tx.host_index, ui_, batch_))
    return WorkflowStatus::BATCH_ERROR;

  tx.online_authorization_required = true;  // no floor limit checking for refund

  host_.Disconnect();
  if (!host_.PreConnect(host_definition_config->index)) {  // Wrong comm config
    ui_.MessageCommunicationError();
    return WorkflowStatus::COMM_ERROR;
  }

  if (!DisplayPanAndExpiryDate(app_settings_, &ui_, tx.issuer_index, *tx.pan,
                               tx.expiration_date)) {
    return WorkflowStatus::MANUAL_TIMEOUT_OR_CANCEL;
  }

  tx.batch_num = batch_.GetCurrentBatchNumber(host_definition_config->index);
  tx.invoice_num = GetNextUniqueInvoice(batch_);

  ui_.MessageCommunicatingHost();

  if (!ProcessPendingReversals(batch_, host_, host_definition_config->index)) {
    SetInvoiceNo(tx.invoice_num);
    ui_.MessagePleaseTryAgain();
    return WorkflowStatus::MANUAL_ERROR;
  }

  tx.SetTransactionStatus(TransactionStatus::IN_PROGRESS);

  tx.stan_original = tx.stan = GetNextStanNo();  // get STAN after processing reversal, in case if acq host requires increase STAN for every reversal attempt.

  if (!batch_.Insert(tx)) {
    SetInvoiceNo(tx.invoice_num);
    ui_.MessageProcessAborted();  // Batch Error
    return WorkflowStatus::BATCH_ERROR;
  }

  host_.AuthorizeRefund(tx);

  tx.SetTransactionStatus(TransactionStatus::TO_REVERSE);
  batch_.InsertOrUpdate(tx);

  if (!IsHostResponseReceived(tx)) {
    ui_.MessagePleaseTryAgain();
    return WorkflowStatus::MANUAL_ERROR;
  }
  else if (IsResponseCodeApproved(tx.response_code)) {
    tx.SetTransactionStatus(TransactionStatus::APPROVED);
    batch_.InsertOrUpdate(tx);

    UploadPendingItemsAfterTxnApproved(app_settings_, batch_, host_,
                                       host_definition_config->index);

    return WorkflowStatus::MANUAL_SUCCESS;
  }
  else {
    batch_.Delete(tx);
    SetInvoiceNo(tx.invoice_num);
    tx.SetTransactionStatus(TransactionStatus::DECLINED);
    ui_.DisplayResponseMessage(tx.response_code);
    return WorkflowStatus::MANUAL_SUCCESS;
  }

  return WorkflowStatus::MANUAL_SUCCESS;
}

WorkflowStatus RefundWorkflow::RunMagFlow(types::MagStripe mag_stripe,
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

  if (!RetrieveConfigsFromPan(app_settings_, *tx.pan, card_definition_config,
                              issuer_definition_config,
                              host_definition_config, tx)) {
    ui_.MessageUnsupportedCard();
    return WorkflowStatus::MAG_ERROR;
  }

  if (!IsValidPAN(*card_definition_config, *tx.pan)) {
    ui_.MessageInvalidPan();
    return WorkflowStatus::MAG_ERROR;
  }

  if (!CheckCardExpired(ui_, tx.expiration_date))
    return WorkflowStatus::MAG_ERROR;

  if (!IsTxEnabledPerCard(*card_definition_config, tx)) {
    ui_.MessageUnsupportedCard();
    return WorkflowStatus::MAG_ERROR;
  }

  SetTransactionDataFromConfigs(*card_definition_config,
                                *issuer_definition_config,
                                *host_definition_config, &tx);

  if (!CheckAndOpenBatch(tx.host_index, ui_, batch_))
    return WorkflowStatus::BATCH_ERROR;

  tx.online_authorization_required = true;

  host_.Disconnect();
  if (!host_.PreConnect(host_definition_config->index)) {  // Wrong comm config
    ui_.MessageCommunicationError();
    return WorkflowStatus::COMM_ERROR;
  }

  if (!DisplayPanAndExpiryDate(app_settings_, &ui_, tx.issuer_index, *tx.pan,
                               tx.expiration_date)) {
    return WorkflowStatus::MAG_TIMEOUT_OR_CANCEL;
  }

  if (tx.is_fallback)
    tx.pos_entry_mode = types::PosEntryMode::FALLBACK_MAGSTRIPE;
  else
    tx.pos_entry_mode = types::PosEntryMode::MAGSTRIPE;

  // assuming the Visa, MC, JCB, CUP doing the same
  if (IsQPSTransaction(tx, card_definition_config->qps_floor_limit))
    tx.signature_required = false;

  tx.batch_num = batch_.GetCurrentBatchNumber(host_definition_config->index);
  tx.invoice_num = GetNextUniqueInvoice(batch_);

  ui_.MessageCommunicatingHost();

  if (!ProcessPendingReversals(batch_, host_, host_definition_config->index)) {
    SetInvoiceNo(tx.invoice_num);
    ui_.MessagePleaseTryAgain();
    return WorkflowStatus::MAG_ERROR;
  }

  tx.SetTransactionStatus(TransactionStatus::IN_PROGRESS);

  tx.stan_original = tx.stan = GetNextStanNo();  // get STAN after processing reversal, in case if acq host requires increase STAN for every reversal attempt.

  if (!batch_.Insert(tx)) {
    SetInvoiceNo(tx.invoice_num);
    ui_.MessageProcessAborted();  // Batch Error
    return WorkflowStatus::BATCH_ERROR;
  }

  host_.AuthorizeRefund(tx);

  tx.SetTransactionStatus(TransactionStatus::TO_REVERSE);
  batch_.InsertOrUpdate(tx);

  if (!IsHostResponseReceived(tx)) {
    ui_.MessagePleaseTryAgain();
    return WorkflowStatus::MAG_ERROR;
  }
  else if (IsResponseCodeApproved(tx.response_code)) {
    tx.SetTransactionStatus(TransactionStatus::APPROVED);
    batch_.InsertOrUpdate(tx);

    UploadPendingItemsAfterTxnApproved(app_settings_, batch_, host_,
                                       host_definition_config->index);

    return WorkflowStatus::MAG_SUCCESS;
  }
  else {
    batch_.Delete(tx);
    SetInvoiceNo(tx.invoice_num);
    tx.SetTransactionStatus(TransactionStatus::DECLINED);
    ui_.DisplayResponseMessage(tx.response_code);
    return WorkflowStatus::MAG_SUCCESS;
  }

  return WorkflowStatus::MAG_SUCCESS;
}

WorkflowStatus RefundWorkflow::RunEmvFlow(Transaction& tx) {
  EmvWorkflow emv_workflow(ui_, app_settings_, emv_config_);
  auto emv_status = emv_workflow.ReadICCData(tx);
  if (emv_status != WorkflowStatus::EMV_SUCCESS)
    return emv_status;

  stdx::optional<CardDefinition> card_definition_config;
  stdx::optional<IssuerDefinition> issuer_definition_config;
  stdx::optional<HostDefinition> host_definition_config;

  if (!RetrieveConfigsFromPan(app_settings_, *tx.pan, card_definition_config,
                              issuer_definition_config,
                              host_definition_config, tx)) {
    ui_.MessageUnsupportedCard();
    return WorkflowStatus::EMV_ERROR;
  }

  if (!IsValidPAN(*card_definition_config, *tx.pan)) {
    ui_.MessageInvalidPan();
    return WorkflowStatus::EMV_ERROR;
  }

  if (!IsTxEnabledPerCard(*card_definition_config, tx)) {
    ui_.MessageUnsupportedCard();
    return WorkflowStatus::EMV_ERROR;
  }

  SetTransactionDataFromConfigs(*card_definition_config,
                                *issuer_definition_config,
                                *host_definition_config, &tx);

  if (!CheckAndOpenBatch(tx.host_index, ui_, batch_))
    return WorkflowStatus::BATCH_ERROR;

  host_.Disconnect();
  if (!host_.PreConnect(host_definition_config->index)) {  // Wrong comm config
    ui_.MessageCommunicationError();
    return WorkflowStatus::COMM_ERROR;
  }

  if (!DisplayPanAndExpiryDate(app_settings_, &ui_, tx.issuer_index, *tx.pan,
                               tx.expiration_date)) {
    tx.transaction_status = TransactionStatus::CANCELLED;
    return WorkflowStatus::EMV_CANCELLED;
  }

  tx.pos_entry_mode = types::PosEntryMode::CHIP;

  if (emv_workflow.FullEMVProcessingBeforeOnline(pin_entry_, tx, nullptr,
                                                 EmvForceAction::DECLINE)
      != WorkflowStatus::EMV_SUCCESS) {
    ui_.DisplayEMVProcessingError(tx.transaction_status);
    return WorkflowStatus::EMV_ERROR;
  }

  // assuming the Visa, MC, JCB, CUP doing the same
  if (IsQPSTransaction(tx, card_definition_config->qps_floor_limit))
    tx.signature_required = false;

  tx.batch_num = batch_.GetCurrentBatchNumber(host_definition_config->index);
  tx.invoice_num = GetNextUniqueInvoice(batch_);

  ui_.MessageCommunicatingHost();

  if (!host_.WaitForConnection()) {
    ui_.MessageCommunicationError();
    return WorkflowStatus::COMM_ERROR;
  }

  if (!ProcessPendingReversals(batch_, host_, host_definition_config->index)) {
    SetInvoiceNo(tx.invoice_num);
    ui_.MessagePleaseTryAgain();
    return WorkflowStatus::EMV_ERROR;
  }

  tx.SetTransactionStatus(TransactionStatus::IN_PROGRESS);

  tx.stan_original = tx.stan = GetNextStanNo();  // get STAN after processing reversal, in case if acq host requires increase STAN for every reversal attempt.
  tx.tc_upload_stan = GetNextStanNo();

  if (!batch_.Insert(tx)) {
    SetInvoiceNo(tx.invoice_num);
    ui_.MessageProcessAborted();  // Batch Error
    return WorkflowStatus::BATCH_ERROR;
  }

  host_.AuthorizeRefund(tx);

  tx.SetTransactionStatus(TransactionStatus::TO_REVERSE);
  batch_.InsertOrUpdate(tx);

  if (!IsHostResponseReceived(tx)) {
    ui_.MessagePleaseTryAgain();
    return WorkflowStatus::EMV_ERROR;
  }
  else if (IsResponseCodeApproved(tx.response_code)) {
    tx.SetTransactionStatus(TransactionStatus::APPROVED);
    batch_.InsertOrUpdate(tx);

    UploadPendingItemsAfterTxnApproved(app_settings_, batch_, host_,
                                       host_definition_config->index);

    return WorkflowStatus::EMV_SUCCESS;
  }
  else {
    batch_.Delete(tx);
    SetInvoiceNo(tx.invoice_num);
    tx.SetTransactionStatus(TransactionStatus::DECLINED);
    ui_.DisplayResponseMessage(tx.response_code);
    return WorkflowStatus::EMV_SUCCESS;
  }
}

WorkflowStatus RefundWorkflow::RunClessFlow(ContactlessWorkflow& cless_workflow,
                                            Transaction& tx) {

  auto cless_tx_status = cless_workflow.ActivateRefundTransaction(tx);
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

  if (!IsTxEnabledPerCard(*card_definition_config, tx)) {
    ui_.MessageUnsupportedCard();
    return WorkflowStatus::CLESS_ERROR;
  }

  SetTransactionDataFromConfigs(*card_definition_config,
                                *issuer_definition_config,
                                *host_definition_config, &tx);

  if (!CheckAndOpenBatch(tx.host_index, ui_, batch_))
    return WorkflowStatus::BATCH_ERROR;

  tx.batch_num = batch_.GetCurrentBatchNumber(
                                              host_definition_config->index);
  tx.invoice_num = GetNextUniqueInvoice(batch_);

  host_.Disconnect();
  if (!host_.PreConnect(host_definition_config->index)) {  // Wrong comm config
    ui_.MessageCommunicationError();
    return WorkflowStatus::COMM_ERROR;
  }

  if (!DisplayPanAndExpiryDate(app_settings_, &ui_,
                               tx.issuer_index,
                               *tx.pan,
                               tx.expiration_date)) {
    tx.transaction_status = TransactionStatus::CANCELLED;
    return WorkflowStatus::CLESS_CANCELLED;
  }

  ui_.MessageCommunicatingHost();

  if (!ProcessPendingReversals(batch_, host_,
                               host_definition_config->index)) {
    SetInvoiceNo(tx.invoice_num);
    ui_.MessagePleaseTryAgain();
    return WorkflowStatus::CLESS_ERROR;
  }

  tx.SetTransactionStatus(TransactionStatus::IN_PROGRESS);

  tx.stan_original = tx.stan = GetNextStanNo();  // get STAN after processing reversal, in case if acq host requires increase STAN for every reversal attempt.
  tx.tc_upload_stan = GetNextStanNo();

  if (!batch_.Insert(tx)) {
    SetInvoiceNo(tx.invoice_num);
    ui_.MessageProcessAborted();  // Batch Error
    return WorkflowStatus::BATCH_ERROR;
  }

  host_.AuthorizeRefund(tx);

  tx.SetTransactionStatus(TransactionStatus::TO_REVERSE);
  batch_.InsertOrUpdate(tx);

  if (!IsHostResponseReceived(tx)) {
    ui_.MessagePleaseTryAgain();
    return WorkflowStatus::CLESS_ERROR;
  }
  else if (IsResponseCodeApproved(tx.response_code)) {
    tx.SetTransactionStatus(TransactionStatus::APPROVED);
    batch_.InsertOrUpdate(tx);

    UploadPendingItemsAfterTxnApproved(app_settings_, batch_,
                                       host_,
                                       host_definition_config->index);

    return WorkflowStatus::CLESS_SUCCESS;
  }
  else {
    batch_.Delete(tx);
    SetInvoiceNo(tx.invoice_num);
    ui_.DisplayResponseMessage(tx.response_code);
    return WorkflowStatus::CLESS_SUCCESS;
  }

}

bool RefundWorkflow::IsRefundPasswordRequired(
    const types::Amount& amount) const {
  types::Amount password_floor_limit_amount(
      amount.GetCurrencyName().c_str(),
      app_settings_.managed_settings_->terminal_config.RefundPasswordFloorLimit());

  return amount > password_floor_limit_amount;
}

bool RefundWorkflow::VerifyRefundPassword(const TransactionType tx_type) const {
  Password password(app_settings_, ui_);

  return password.PerformPasswordAccess(tx_type);
}

}

