/*
 ------------------------------------------------------------------------------
 INGENICO Technical Software Department
 ------------------------------------------------------------------------------
 Copyright (c) 2014 INGENICO S.A.
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
#include "receipt_other_transaction_test.h"

#include <tpcore/goal_handle.h>
#include <bdor/goal_receipts.h>

#include <bdor/receipts.h>

static receipt::PrintStatus PrintLoop(const receipt::Receipt& receipt) {
  receipt::PrintStatus value;
  for (;;) {
    auto print_status = receipt.Print();
    if (print_status != receipt::PRINT_NOPAPER) {
      value = print_status;
      break;
    }
  }
  return value;
}
receipt::PrintStatus TestPrintInstalmentSale(
    bdor::CopyType copy, bdor::Transaction& txn1, bdor::Receipts& receipts,
    bdor::ApplicationSettings& app_settings) {

  using stdx::make_unique;

  txn1.transaction_type = bdor::TransactionType::INSTALMENT_SALE;
  txn1.mid = "999999123455555";
  txn1.tid = "21537916";
  txn1.batch_num = 1;
  txn1.invoice_num = 1;
  txn1.auth_id_response = "12AB34";
  txn1.expiration_date = "0220";
  txn1.card_name = "VISA";
  txn1.card_holder_name = "ANUSHA";
  txn1.tx_datetime = stdx::time(NULL) - (22 * 24 * 60 * 60);
  txn1.host_index = 2;
  txn1.transaction_status = bdor::TransactionStatus::APPROVED;
  txn1.amount = types::Amount("HKD", 1220966);
  txn1.secondary_amount = types::Amount("HKD", 500);
  txn1.rrn="163057000050";
  txn1.description="06 MONTHS";
  txn1.emi_per_month = types::Amount("HKD", 203495);


  types::Pan pan( { '4', '7', '2', '6', '4', '2', '6', '8', '9', '7', '6', '9',
      '0', '0', '3', '0' });
  txn1.pan = pan;



  return PrintLoop(
      *receipts.GetInstalmentSaleReceipt(app_settings,txn1,copy,bdor::ReprintType::ORIGINAL));

}

