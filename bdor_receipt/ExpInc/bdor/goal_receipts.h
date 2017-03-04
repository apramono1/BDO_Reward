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
#ifndef BDOR__GOAL_RECEIPTS_H_
#define BDOR__GOAL_RECEIPTS_H_

#include <bdor/receipts.h>
#include <tpcore/goal_handle.h>
#include <bdor/copy_type.h>
#include <bdor/reprint_type.h>

namespace bdor {

class GoalReceipts : public Receipts {
 public:
  GoalReceipts(tpcore::GoalHandle& gl, unsigned print_timeout);
  receipt_uptr GetTerminalInitReceipt() override;
  receipt_uptr GetDuplicateReceiptHeader() override;
  receipt_uptr LoadImageFile(const char *path) override;

 protected:
  receipt_uptr PrepareReversalReceipt() override;
  receipt_uptr PrepareSaleReceipt(CopyType copy,
                                  ReprintType reprint,
                                  TerminalConfiguration::TipProcessingMode terminal_tip_mode,
                                  CvmType cvm_type) override;

  receipt_uptr PrepareOfflineSaleReceipt(CopyType copy,
                                        ReprintType reprint,
                                        TerminalConfiguration::TipProcessingMode terminal_tip_mode,
                                         CvmType cvm_type) override;

  receipt_uptr PrepareRefundReceipt(CopyType copy,
                                    ReprintType reprint,
                                    CvmType cvm_type) override;

  receipt_uptr PreparePreauthReceipt(CopyType copy,
                                     ReprintType reprint,
                                     CvmType cvm_type) override;

  receipt_uptr PreparePreauthCancellationReceipt(CopyType copy,
                                                 ReprintType reprint,
                                       CvmType cvm_type) override;

  receipt_uptr PrepareCompletionReceipt(CopyType copy,
                                        ReprintType reprint,
                                        CvmType cvm_type) override;

  receipt_uptr PrepareTipAdjustReceipt(CopyType copy,
                                       ReprintType reprint,
                                       CvmType cvm_type) override;

  receipt_uptr PrepareVoidReceipt(CopyType copy,
                                  ReprintType reprint,
                                  TerminalConfiguration::TipProcessingMode terminal_tip_mode,
                                    CvmType cvm_type) override;

  receipt_uptr PrepareSingleHostSettlementReceipt(unsigned int host_index, int card_type_number) override;

  virtual receipt_uptr PrepareAllHostsSettlementReceipt(const std::vector<SingleHostSettlementData>& host_report_list) override;

  receipt_uptr PrepareDetailReportReceipt(int transaction_number, int card_type_number) override;

  receipt_uptr PrepareInstalmentSaleReceipt(CopyType copy,
                                            ReprintType reprint,
                                            CvmType cvm_type) override;

 private:
  tpcore::GoalHandle& gl_;
  unsigned print_timeout_;
};
}

#endif
