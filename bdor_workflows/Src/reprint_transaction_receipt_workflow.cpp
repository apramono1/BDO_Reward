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
#include <bdor/reprint_transaction_receipt_workflow.h>
#include <tpcore/goal_handle.h>
#include "show_transaction_info.h"
#include <bdor/transaction.h>
#include "workflow_activities.h"

namespace bdor {

ReprintTransactionReceiptWorkflow::ReprintTransactionReceiptWorkflow(
    ApplicationSettings& app_setting,
    Batch& batch,
    Ui& ui,
    Receipts& receipts)
    : app_setting_(app_setting),
      batch_(batch),
      ui_(ui),
      receipts_(receipts) {
}

bool ReprintTransactionReceiptWorkflow::Start(bool last_receipt) {

  const uint32_t merchant_receipt_index = 0;
  const uint32_t customer_receipt_index = 1;
  tpcore::GoalHandle gl;

  auto recovered_receipt = RetrieveTransactionReceipt(last_receipt);

  if (recovered_receipt.empty()) {
    return false;
  }

  if (recovered_receipt[merchant_receipt_index]) {
    ui_.MessagePrintingMerchantCopy();
    PrintLoop(ui_, *recovered_receipt[merchant_receipt_index]);
  }
  if (recovered_receipt.size() > 1
      && recovered_receipt[customer_receipt_index]) {
    if (ui_.AskToPrintCustomerCopy()) {
      ui_.MessagePrintingCustomerCopy();

      PrintLoop(ui_, *recovered_receipt[customer_receipt_index]);
    }
  }

  return true;
}

std::vector<receipt::Receipt::uptr> ReprintTransactionReceiptWorkflow::RetrieveTransactionReceipt(
    const bool last_receipt) const {

  std::vector<receipt::Receipt::uptr> output;

  if (last_receipt) {
    output = RetrieveLastTransactionReceiptFromBmp();
  } else {
    auto tx = RetrieveTransactionInfo(app_setting_, batch_, ui_,
                                      Ui::SEARCH_BY_INVOICE_NUMBER);
    if (tx) {
      output = GetReceiptFromTxn(*tx);
    }
  }

  return output;
}

std::vector<receipt::Receipt::uptr> ReprintTransactionReceiptWorkflow::RetrieveLastTransactionReceiptFromBmp() const {
  std::vector<receipt::Receipt::uptr> output;

  receipt::Receipt::uptr merchant_copy = nullptr;
  receipt::Receipt::uptr customer_copy = nullptr;

  if (CheckFileExist(app_setting_.GetLastMerchantTxReceiptBmpPath())) {
    merchant_copy = receipts_.LoadImageFile(
        app_setting_.GetLastMerchantTxReceiptBmpPath().c_str());
  }

  if (CheckFileExist(app_setting_.GetLastCustomerTxReceiptBmpPath())) {
    customer_copy = receipts_.LoadImageFile(
        app_setting_.GetLastCustomerTxReceiptBmpPath().c_str());
  }
  if (!merchant_copy && !customer_copy)
    ui_.MessageTransactionNotFound();
  else {
    output.push_back(std::move(merchant_copy));
    output.push_back(std::move(customer_copy));
  }

  return output;
}

std::vector<receipt::Receipt::uptr> ReprintTransactionReceiptWorkflow::GetReceiptFromTxn(
    const Transaction& tx) const {

  std::vector<receipt::Receipt::uptr> output;

  receipt::Receipt::uptr merchant_copy = nullptr;
  receipt::Receipt::uptr customer_copy = nullptr;

  if (tx.is_voided) {
    if (tx.is_preauth_completed == TransactionType::PREAUTH) {
      merchant_copy = receipts_.GetPreauthCancellationReceipt(
          app_setting_, tx, CopyType::MERCHANT,
          ReprintType::DUPLICATE);

      customer_copy = receipts_.GetPreauthCancellationReceipt(
          app_setting_, tx, CopyType::CUSTOMER,
          ReprintType::DUPLICATE);

    } else {
      merchant_copy = receipts_.GetVoidReceipt(app_setting_, tx,
                                               CopyType::MERCHANT,
                                               ReprintType::DUPLICATE);

      customer_copy = receipts_.GetVoidReceipt(app_setting_, tx,
                                               CopyType::CUSTOMER,
                                               ReprintType::DUPLICATE);
    }
  }

  switch (tx.transaction_type) {
    case TransactionType::SALE:
      if (tx.is_adjusted) {

        merchant_copy = receipts_.GetTipAdjustReceipt(app_setting_, tx,
                                                      CopyType::MERCHANT,
                                                      ReprintType::DUPLICATE);

        customer_copy = receipts_.GetTipAdjustReceipt(app_setting_, tx,
                                                      CopyType::CUSTOMER,
                                                      ReprintType::DUPLICATE);
      } else {
        merchant_copy = receipts_.GetSaleReceipt(app_setting_, tx,
                                                 CopyType::MERCHANT,
                                                 ReprintType::DUPLICATE);

        customer_copy = receipts_.GetSaleReceipt(app_setting_, tx,
                                                 CopyType::CUSTOMER,
                                                 ReprintType::DUPLICATE);
      }
      break;

    case TransactionType::OFFLINE_SALE:
      merchant_copy = receipts_.GetOfflineSaleReceipt(app_setting_, tx,
                                                      CopyType::MERCHANT,
                                                      ReprintType::DUPLICATE);

      customer_copy = receipts_.GetOfflineSaleReceipt(app_setting_, tx,
                                                      CopyType::CUSTOMER,
                                                      ReprintType::DUPLICATE);
      break;

    case TransactionType::REFUND:
      merchant_copy = receipts_.GetRefundReceipt(app_setting_, tx,
                                                 CopyType::MERCHANT,
                                                 ReprintType::DUPLICATE);

      customer_copy = receipts_.GetRefundReceipt(app_setting_, tx,
                                                 CopyType::CUSTOMER,
                                                 ReprintType::DUPLICATE);

    case TransactionType::PREAUTH:
      merchant_copy = receipts_.GetPreauthReceipt(app_setting_, tx,
                                                  CopyType::MERCHANT,
                                                  ReprintType::DUPLICATE);

      customer_copy = receipts_.GetPreauthReceipt(app_setting_, tx,
                                                  CopyType::CUSTOMER,
                                                  ReprintType::DUPLICATE);
      break;
    case TransactionType::PREAUTH_COMPLETION_ONLINE:
      case TransactionType::PREAUTH_COMPLETION_OFFLINE:
      merchant_copy = receipts_.GetCompletionReceipt(app_setting_, tx,
                                                     CopyType::MERCHANT,
                                                     ReprintType::DUPLICATE);

      customer_copy = receipts_.GetCompletionReceipt(app_setting_, tx,
                                                     CopyType::CUSTOMER,
                                                     ReprintType::DUPLICATE);
      break;
    default:
      break;
  }
  if (!merchant_copy && !customer_copy)
      ui_.MessageTransactionNotFound();

  if (merchant_copy)
    output.push_back(std::move(merchant_copy));
  if (customer_copy)
    output.push_back(std::move(customer_copy));

  return output;
}

}
