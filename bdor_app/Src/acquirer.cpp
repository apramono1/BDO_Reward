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
#include "acquirer.h"

#include <algorithm>
#include <tpcore/goal_handle.h>
#include <types/pos_condition_code.h>
#include <bdor/ui.h>
#include <bdor/sale_workflow.h>
#include <bdor/clear_reversal_workflow.h>
#include <bdor/completion_online_workflow.h>
#include <bdor/completion_offline_workflow.h>
#include <bdor/preauth_workflow.h>
#include <bdor/key_exchange_workflow.h>
#include <bdor/offline_sale_workflow.h>
#include <bdor/receipts.h>
#include <bdor/refund_workflow.h>
#include <bdor/settlement_workflow.h>
#include <bdor/summary_report_workflow.h>
#include <bdor/detail_report_workflow.h>
#include <bdor/tip_adjust_workflow.h>
#include <bdor/transaction_type_name.h>
#include <bdor/void_workflow.h>
#include <bdor/preauth_cancellation_workflow.h>
#include <bdor/instalment_sale_workflow.h>
#include <bdor/host_test_workflow.h>
#include <bdor/reprint_last_settlement_workflow.h>
#include <bdor/reprint_transaction_receipt_workflow.h>
#include <bdor/reward_workflow.h>
#include <bdor/password.h>
#include <bdor/batch_review_workflow.h>
#include <bdor/batch_total_workflow.h>

namespace bdor {

Acquirer::Acquirer(ApplicationSettings& app_setting, Batch& batch,
                   HostSwitch& host,
				   Acquirer& acquirer,
                   emv::EmvConfig& emv_config,
                   contactless::ClessConfig& cless_config,
                   tpcore::PinEntry& pin_entry,
                   Ui& ui,
                   Receipts& receipts,
                   Password& password)
    : app_setting_(app_setting),
      batch_(batch),
      host_(host),
	  acquirer_(acquirer),
      emv_config_(emv_config),
      cless_config_(cless_config),
      pin_entry_(pin_entry),
      ui_(ui),
      receipts_(receipts),
      password_(password){
}

bool Acquirer::StartReward() {
  if (!password_.PerformPasswordAccess(TransactionType::REWARD))
    return true;

  RewardWorkflow(app_setting_, batch_, host_, acquirer_, emv_config_, cless_config_,
               pin_entry_,
               ui_, receipts_, password_).Start();
  return true;
}

/*bool Acquirer::StartRedemption() {
  if (!password_.PerformPasswordAccess(TransactionType::REWARD))
    return true;

  RedemptionWorkflow(app_setting_, batch_, host_, emv_config_, cless_config_,
               pin_entry_,
               ui_, receipts_, password_).Start();
  return true;
} */

bool Acquirer::StartCash() {
  if (!password_.PerformPasswordAccess(TransactionType::REWARD))
    return true;

  /* RewardWorkflow(app_setting_, batch_, host_, emv_config_, cless_config_,
                 pin_entry_,
                 ui_, receipts_, password_).Start(); */ // To implement
  return true;
}

bool Acquirer::StartSale() {
  if (!password_.PerformPasswordAccess(TransactionType::SALE))
    return true;

  SaleWorkflow(app_setting_, batch_, host_, emv_config_, cless_config_,
               pin_entry_,
               ui_, receipts_).Start();
  return true;
}

bool Acquirer::StartVoid() {
  if (!password_.PerformPasswordAccess(true))
    return true;

  VoidWorkflow(app_setting_, batch_, host_, ui_, receipts_).Start();
  return true;
}

bool Acquirer::StartVoidPreAuth() {
  if (!password_.PerformPasswordAccess(TransactionType::PREAUTH_CANCELLATION))
    return true;

  PreAuthCancellationWorkflow(app_setting_, batch_, host_, emv_config_,
                              cless_config_,
                              pin_entry_, ui_, receipts_).Start(
      types::PosConditionCode::PREAUTH);
  return true;
}

bool Acquirer::StartPreauth() {
  if (!password_.PerformPasswordAccess(TransactionType::PREAUTH))
    return true;

  PreauthWorkflow(app_setting_, batch_, host_, emv_config_, cless_config_,
                  pin_entry_,
                  ui_, receipts_).Start();
  return true;
}

bool Acquirer::StartCompletionOnline() {
  if (!password_.PerformPasswordAccess(
      TransactionType::PREAUTH_COMPLETION_ONLINE))
    return true;

  CompletionOnlineWorkflow(app_setting_, batch_, host_, emv_config_,
                           cless_config_,
                           ui_,
                           receipts_,
                           pin_entry_).Start(
                                             types::PosConditionCode::PREAUTH);
  return true;
}

bool Acquirer::StartCompletionOffline() {
  if (!password_.PerformPasswordAccess(
      TransactionType::PREAUTH_COMPLETION_OFFLINE))
    return true;

  CompletionOfflineWorkflow(app_setting_, batch_, host_, emv_config_,
                            cless_config_,
                            ui_,
                            receipts_,
                            pin_entry_).Start(
                                              types::PosConditionCode::PREAUTH);
  return true;
}

bool Acquirer::StartMotoPreauth() {
  if (!password_.PerformPasswordAccess(TransactionType::PREAUTH))
    return true;

  PreauthWorkflow(app_setting_, batch_, host_, emv_config_, cless_config_,
                  pin_entry_,
                  ui_, receipts_).Start(
                                        types::PosConditionCode::MOTO_PREAUTH);
  return true;
}

bool Acquirer::StartTipAdjust() {
  if (!password_.PerformPasswordAccess(TransactionType::OFFLINE_SALE))
    return false;

  TipAdjustWorkflow(app_setting_, batch_, ui_, receipts_).Start();
  return true;
}

bool Acquirer::StartSettlement(bool pos_trigger) {
  if (!pos_trigger) {
    if (!password_.PerformSettlementPasswordAccess())
      return true;
  }

  if (!pos_trigger) {
    SettlementWorkflow(app_setting_, batch_, host_, ui_, receipts_).Start();
  }
  else {
    return SettlementWorkflow(app_setting_, batch_, host_, ui_, receipts_)
        .PosStart();
  }
  return true;
}

bool Acquirer::StartRefund() {

  RefundWorkflow(app_setting_, batch_, host_, emv_config_, cless_config_,
                 pin_entry_,
                 ui_, receipts_).Start();
  return false;
}

bool Acquirer::StartMOTOSale() {
  if (!password_.PerformPasswordAccess(TransactionType::SALE))
    return true;

  SaleWorkflow(app_setting_, batch_, host_, emv_config_, cless_config_,
               pin_entry_,
               ui_, receipts_).Start(types::PosConditionCode::MOTO);
  return true;
}
bool Acquirer::StartInstalmentSale() {

  if (!password_.PerformPasswordAccess(TransactionType::INSTALMENT_SALE))
    return true;

  InstalmentSaleWorkflow(app_setting_, batch_, host_, emv_config_,
                         cless_config_,
                         pin_entry_,
                         ui_,
                         receipts_).Start();

  return true;
}
bool Acquirer::StartKeyExchange() {
  if (batch_.IsAnyBatchLocked()) {
    ui_.MessagePleaseSettleBatch();
    return true;
  }

  unsigned int host_index;
  do {
    if (!ui_.SelectAcquirer(host_index, true))
      return true;

    KeyExchangeWorkflow(app_setting_, host_, ui_).Start();
  } while (true);

  return true;
}

bool Acquirer::StartClearReversal() {
  if (!password_.PerformManagerPasswordAccess())
    return true;

  ClearReversalWorkFlow(app_setting_, batch_, ui_).Start();
  return true;
}

bool Acquirer::StartHostTest() {
  HostTestWorkfow(app_setting_, host_, ui_).Start();
  return true;
}

bool Acquirer::StartOfflineSale() {
  if (!password_.PerformPasswordAccess(TransactionType::OFFLINE_SALE))
    return true;

  OfflineSaleWorkflow(app_setting_, batch_, emv_config_, cless_config_, ui_,
                      receipts_).Start();
  return true;
}

bool Acquirer::StartClearBatch() {

  unsigned int host_index;

  if (!password_.PerformManagerPasswordAccess())
    return true;

  if (!(ui_.SelectAcquirer(host_index, true)))
    return false;

  if (!ui_.ConfirmDeleteBatch())
    return false;

  if (batch_.ClearBatch(host_index))
    ui_.MessageDeleteBatch(host_index);

  return true;
}

bool Acquirer::StartMOTORefund() {

  RefundWorkflow(app_setting_, batch_, host_, emv_config_, cless_config_,
                 pin_entry_,
                 ui_, receipts_).Start(
                                       types::PosConditionCode::MOTO);
  return true;
}

bool Acquirer::StartReprintLastTransaction(bool pos_trigger) {
  bool ret = ReprintTransactionReceiptWorkflow(app_setting_, batch_, ui_,
                                               receipts_)
      .Start(true);

  return !pos_trigger || ret;
}

bool Acquirer::StartReprintAnyTransaction() {
  ReprintTransactionReceiptWorkflow(app_setting_, batch_, ui_, receipts_)
      .Start(false);
  return true;
}

bool Acquirer::StartReprintLastSettlement() {
  ReprintLastSettlementWorkflow(app_setting_, ui_, receipts_).Start();
  return true;
}

bool Acquirer::StartPrintSummaryReport() {

  SummaryReportWorkflow(app_setting_, batch_, ui_, receipts_).Start();
  return true;
}

bool Acquirer::PrintDetailReport() {

  DetailReportWorkflow(app_setting_, batch_, ui_, receipts_).Start();
  return true;
}

bool Acquirer::StartBatchReview() {
  BatchReviewWorkflow(app_setting_, batch_, ui_).Start();
  return true;
}

bool Acquirer::StartBatchTotal() {
  BatchTotalWorkflow(app_setting_, batch_, ui_, receipts_).Start();
  return true;
}

std::unique_ptr<Transaction> Acquirer::StartSale(
    const types::Amount& amount) {
  return stdx::make_unique<Transaction>(
      SaleWorkflow(app_setting_, batch_, host_, emv_config_, cless_config_,
                   pin_entry_,
                   ui_, receipts_).Start(amount));
}

std::unique_ptr<Transaction> Acquirer::StartRefund(
    const types::Amount& amount) {
  return stdx::make_unique<Transaction>(
      RefundWorkflow(app_setting_, batch_, host_, emv_config_, cless_config_,
                     pin_entry_,
                     ui_, receipts_).Start(amount));
}

std::unique_ptr<Transaction> Acquirer::StartPreauth(
    const types::Amount& amount) {
  return stdx::make_unique<Transaction>(
      PreauthWorkflow(app_setting_, batch_, host_, emv_config_, cless_config_,
                      pin_entry_,
                      ui_, receipts_).Start(amount));
}

bool Acquirer::TODOFunction() {
  ui_.MessageTransactionNotAllowed();
  return true;
}

}

