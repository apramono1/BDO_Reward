/*
 ------------------------------------------------------------------------------
 INGENICO Technical Software Department
 ------------------------------------------------------------------------------
 Copyright (c) 2014-2016 INGENICO S.A.
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
#include <bdor/goal_receipts.h>
#include <bdor/transaction.h>

#include <stdx/memory>
#include <receipt/goal_receipt_from_file.h>
#include <settlement_single_host_receipt.h>
#include <settlement_all_hosts_receipt.h>
#include "sale_receipt.h"
#include "offline_sale_receipt.h"
#include "refund_receipt.h"
#include "preauth_receipt.h"
#include "preauth_cancellation_receipt.h"
#include "preauth_completion_receipt.h"
#include "tip_adjust_receipt.h"
#include "reversal_receipt.h"
#include "void_receipt.h"
#include "terminal_initialization_receipt.h"
#include "duplicate_receipt_header.h"
#include "detail_report_receipt.h"
#include "instalment_sale_receipt.h"

namespace bdor {

GoalReceipts::GoalReceipts(tpcore::GoalHandle& gl, unsigned print_timeout)
    : gl_(gl),
      print_timeout_(print_timeout) {
}

using stdx::make_unique;
using uptr = GoalReceipts::receipt_uptr;

uptr GoalReceipts::PrepareReversalReceipt() {

  return make_unique<ReversalReceipt>(gl_,
                                      print_timeout_);
}

uptr GoalReceipts::PrepareSaleReceipt(
    CopyType copy,
    ReprintType reprint,
    TerminalConfiguration::TipProcessingMode terminal_tip_mode,
    CvmType cvm_type) {
  return make_unique<SaleReceipt>(gl_,
                                  print_timeout_,
                                  copy, reprint, terminal_tip_mode, cvm_type);
}

uptr GoalReceipts::PrepareOfflineSaleReceipt(
    CopyType copy,
    ReprintType reprint,
    TerminalConfiguration::TipProcessingMode terminal_tip_mode,
    CvmType cvm_type) {
  return make_unique<OfflineSaleReceipt>(gl_,
                                         print_timeout_,
                                         copy, reprint, terminal_tip_mode,
                                         cvm_type);
}

uptr GoalReceipts::PrepareRefundReceipt(CopyType copy,
                                        ReprintType reprint,
                                        CvmType cvm_type) {

  return make_unique<RefundReceipt>(gl_,
                                    print_timeout_,
                                    copy, reprint, cvm_type);
}

uptr GoalReceipts::PreparePreauthReceipt(CopyType copy,
                                         ReprintType reprint,
                                         CvmType cvm_type) {

  return make_unique<PreauthReceipt>(gl_,
                                     print_timeout_,
                                     copy, reprint, cvm_type);
}

uptr GoalReceipts::PreparePreauthCancellationReceipt(CopyType copy,
                                                     ReprintType reprint,
                                                     CvmType cvm_type) {
  return make_unique<PreauthCancellationReceipt>(gl_,
                                                 print_timeout_,
                                                 copy, reprint, cvm_type);
}

uptr GoalReceipts::PrepareCompletionReceipt(CopyType copy,
                                            ReprintType reprint,
                                            CvmType cvm_type) {

  return make_unique<PreauthCompletionReceipt>(gl_,
                                               print_timeout_,
                                               copy, reprint, cvm_type);
}

uptr GoalReceipts::PrepareTipAdjustReceipt(CopyType copy,
                                           ReprintType reprint,
                                           CvmType cvm_type) {

  return make_unique<TipAdjustReceipt>(gl_,
                                       print_timeout_,
                                       copy, reprint, cvm_type);

}

uptr GoalReceipts::PrepareVoidReceipt(
    CopyType copy,
    ReprintType reprint,
    TerminalConfiguration::TipProcessingMode terminal_tip_mode,
    CvmType cvm_type) {

  return make_unique<VoidReceipt>(gl_,
                                  print_timeout_,
                                  copy, reprint, terminal_tip_mode, cvm_type);
}

uptr GoalReceipts::PrepareSingleHostSettlementReceipt(unsigned int host_index,
                                                      int card_type_number) {
  return make_unique<SettlementSingleHostReceipt>(gl_,
                                                  print_timeout_,
                                                  host_index,
                                                  card_type_number);
}

uptr GoalReceipts::PrepareAllHostsSettlementReceipt(
    const std::vector<SingleHostSettlementData>& host_report_list) {
  return make_unique<SettlementAllHostsReceipt>(gl_,
                                                print_timeout_,
                                                host_report_list);
}

uptr GoalReceipts::PrepareDetailReportReceipt(int transaction_number, int card_type_number) {
  return make_unique<DetailReportReceipt>(gl_, print_timeout_, transaction_number, card_type_number);
}
uptr GoalReceipts::PrepareInstalmentSaleReceipt(CopyType copy,
                                                ReprintType reprint,
                                                CvmType cvm_type) {

  return make_unique<InstalmentSaleReceipt>(gl_,
                                            print_timeout_,
                                            copy, reprint, cvm_type);
}

uptr GoalReceipts::GetTerminalInitReceipt() {

  return make_unique<TerminalInitializationReceipt>(gl_,
                                                    print_timeout_);
}

uptr GoalReceipts::GetDuplicateReceiptHeader() {

  return make_unique<DuplicateReceiptHeader>(gl_, print_timeout_);
}

uptr GoalReceipts::LoadImageFile(const char *path) {

  return make_unique<receipt::GoalReceiptFromFile>(gl_, path, print_timeout_);
}
}
