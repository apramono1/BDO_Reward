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
#include <bdor/tip_adjust_workflow.h>

#include <utils/logger.h>
#include <idalloc/idalloc.h>
#include "show_transaction_info.h"
#include "workflow_activities.h"
#include <bdor/transaction_type_name.h>
#include <bdor/copy_type.h>
#include <bdor/reprint_type.h>

namespace bdor {

TipAdjustWorkflow::TipAdjustWorkflow(ApplicationSettings& app_setting,
                                     Batch& batch,
                                     Ui& ui,
                                     Receipts& receipts)
    : app_settings_(app_setting),
      batch_(batch),
      ui_(ui),
      receipts_(receipts) {
}

void TipAdjustWorkflow::Start() {
  logger::debug("Starting SaleTipAdjust Transaction");

  Ui::RecordSearchOption search_option;
  if (!ui_.SelectRecordSearchOption(search_option))
    return;

  auto tx = RetrieveTransactionInfo(app_settings_, batch_, ui_, search_option);

  if (tx == nullptr)
    return;

  if (batch_.IsBatchLocked(tx->host_index))
    return ui_.MessagePleaseSettleBatch();

  if (!CheckTransactionTypeAndStatus(*tx))
    return;

  auto tx_detail = GetTxDetailToDisplay(app_settings_, *tx);

  if (!ui_.DisplayTxReview(tx_detail))
    return;

  auto currency_info = app_settings_.managed_settings_
      ->terminal_config.TerminalBaseCurrency();

  auto original_secondary_amount = tx->secondary_amount;

  tx->secondary_amount = types::Amount(currency_info.GetName(), 0);

  bool tip_amount_ok = false;
  do {
    if (!ui_.RequestTipAmount(*tx->secondary_amount))
      return;

    if (!ui_.DisplayTransactionAndTipAmount(*tx->amount,
                                            *tx->secondary_amount))
      return;

    uint64_t tip_limit = app_settings_.managed_settings_->terminal_config
        .TipAdjustMaxVariance();

    if (!VerifyTipLimit(*tx->amount, *tx->secondary_amount, tip_limit))
      ui_.MessageTipPercentageExceeded();
    else
      tip_amount_ok = true;
  } while (!tip_amount_ok);

  ui_.MessageProcessing();
  if (tx->transaction_status == TransactionStatus::TO_ADVISE) {
    tx->is_adjusted = false;
  }
  else {
    tx->is_adjusted = true;
    tx->SetTransactionStatus(TransactionStatus::TO_ADVISE);
  }

  tx->original_secondary_amount = original_secondary_amount;
  tx->UpdateDccAmounts();

  batch_.InsertOrUpdate(*tx);
  ui_.MessageApproved("OFFLINE");

  PrintReceipts(
      app_settings_, ui_,
      tx->transaction_status,
      *receipts_.GetReversalReceipt(app_settings_, *tx),
      *receipts_.GetTipAdjustReceipt(app_settings_, *tx, CopyType::MERCHANT,
                                     ReprintType::ORIGINAL),
      *receipts_.GetTipAdjustReceipt(app_settings_, *tx, CopyType::CUSTOMER,
                                     ReprintType::ORIGINAL),
      *receipts_.GetTipAdjustReceipt(app_settings_, *tx, CopyType::MERCHANT,
                                     ReprintType::DUPLICATE),
      *receipts_.GetTipAdjustReceipt(app_settings_, *tx, CopyType::CUSTOMER,
                                     ReprintType::DUPLICATE)
                                     );
  return;
}

bool TipAdjustWorkflow::CheckTransactionTypeAndStatus(
    const Transaction& tx) const {

  if (tx.transaction_type != TransactionType::SALE
      && tx.transaction_type != TransactionType::OFFLINE_SALE) {
    ui_.MessageTransactionNotAllowed();
    return false;
  }

  if (tx.transaction_status != TransactionStatus::TO_ADVISE
      && tx.transaction_status != TransactionStatus::APPROVED) {
    ui_.MessageTransactionNotFound();
    return false;
  }

  if (tx.is_adjusted) {
    ui_.MessageTransactionAlreadyAdjusted();
    return false;
  }

  if (tx.is_voided) {
    ui_.MessageTransactionAlreadyVoided();
    return false;
  }

  return true;
}

}
