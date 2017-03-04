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
#ifndef BDOR__RECEIPTS_H_
#define BDOR__RECEIPTS_H_

#include <receipt/receipt.h>
#include <bdor/transaction.h>
#include <bdor/batch.h>
#include <bdor/cvm_type.h>
#include <bdor/copy_type.h>
#include <bdor/reprint_type.h>
#include <bdor/terminal_configuration.h>
#include <bdor/application_settings.h>

#include <bdor/receipt_settlement_data_types.h>

namespace bdor {

class Receipts {
 public:
  using Status = receipt::PrintStatus;
  using receipt_uptr = receipt::Receipt::uptr;

  virtual ~Receipts() {
  }

  receipt_uptr GetReversalReceipt(
                                  const ApplicationSettings& app_settings,
                                  const Transaction& tx);

  receipt_uptr GetSaleReceipt(
                              const ApplicationSettings& app_settings,
                              const Transaction& tx,
                              CopyType copy = CopyType::MERCHANT,
                              ReprintType reprint = ReprintType::ORIGINAL);

  receipt_uptr GetOfflineSaleReceipt(
      const ApplicationSettings& app_settings,
      const Transaction& tx,
      CopyType copy = CopyType::MERCHANT,
      ReprintType reprint = ReprintType::ORIGINAL);

  receipt_uptr GetRefundReceipt(
                                const ApplicationSettings& app_settings,
                                const Transaction& tx,
                                CopyType copy = CopyType::MERCHANT,
                                ReprintType reprint = ReprintType::ORIGINAL);

  receipt_uptr GetPreauthReceipt(
                                 const ApplicationSettings& app_settings,
                                 const Transaction& tx,
                                 CopyType copy = CopyType::MERCHANT,
                                 ReprintType reprint = ReprintType::ORIGINAL);

  receipt_uptr GetPreauthCancellationReceipt(
      const ApplicationSettings& app_settings,
      const Transaction& tx,
      CopyType copy = CopyType::MERCHANT,
      ReprintType reprint = ReprintType::ORIGINAL);

  receipt_uptr GetCompletionReceipt(
      const ApplicationSettings& app_settings,
      const Transaction& tx,
      CopyType copy = CopyType::MERCHANT,
      ReprintType reprint = ReprintType::ORIGINAL);

  receipt_uptr GetTipAdjustReceipt(
                                   const ApplicationSettings& app_settings,
                                   const Transaction& tx,
                                   CopyType copy = CopyType::MERCHANT,
                                   ReprintType reprint = ReprintType::ORIGINAL);

  receipt_uptr GetVoidReceipt(
                              const ApplicationSettings& app_settings,
                              const Transaction& tx,
                              CopyType copy = CopyType::MERCHANT,
                              ReprintType reprint = ReprintType::ORIGINAL);

  receipt_uptr GetSingleHostSettlementReceipt(
      ApplicationSettings& app_settings,
      const SingleHostSettlementData& settlement_data);

  receipt_uptr GetAllHostsSettlementReceipt(
      ApplicationSettings& app_settings,
      const AllHostsSettlementData& settlement_data);

  receipt_uptr GetDetailReportReceipt(
      ApplicationSettings& app_settings,
      const DetailReportSingleHost& detail_report);

  receipt_uptr GetInstalmentSaleReceipt(const ApplicationSettings& app_settings,
                                        const Transaction& tx,
                                        CopyType copy = CopyType::MERCHANT,
                                        ReprintType reprint =
                                            ReprintType::ORIGINAL);

  virtual receipt_uptr GetTerminalInitReceipt() = 0;
  virtual receipt_uptr GetDuplicateReceiptHeader() = 0;
  virtual receipt_uptr LoadImageFile(const char *path) = 0;

 protected:
  virtual receipt_uptr PrepareReversalReceipt() = 0;
  virtual receipt_uptr PrepareSaleReceipt(
      CopyType copy,
      ReprintType reprint,
      TerminalConfiguration::TipProcessingMode terminal_tip_mode,
      CvmType cvm_type) = 0;

  virtual receipt_uptr PrepareOfflineSaleReceipt(
      CopyType copy,
      ReprintType reprint,
      TerminalConfiguration::TipProcessingMode terminal_tip_mode,
      CvmType cvm_type) = 0;

  virtual receipt_uptr PrepareRefundReceipt(CopyType copy,
                                            ReprintType reprint,
                                            CvmType cvm_type) = 0;

  virtual receipt_uptr PreparePreauthReceipt(CopyType copy,
                                             ReprintType reprint,
                                             CvmType cvm_type) = 0;

  virtual receipt_uptr PreparePreauthCancellationReceipt(CopyType copy,
                                                         ReprintType reprint,
                                                         CvmType cvm_type) = 0;

  virtual receipt_uptr PrepareCompletionReceipt(CopyType copy,
                                                ReprintType reprint,
                                                CvmType cvm_type) = 0;

  virtual receipt_uptr PrepareTipAdjustReceipt(CopyType copy,
                                               ReprintType reprint,
                                               CvmType cvm_type) = 0;

  virtual receipt_uptr PrepareVoidReceipt(
      CopyType copy,
      ReprintType reprint,
      TerminalConfiguration::TipProcessingMode terminal_tip_mode,
      CvmType cvm_type) = 0;

  virtual receipt_uptr PrepareSingleHostSettlementReceipt(
      unsigned int host_index, int card_type_number) = 0;
  virtual receipt_uptr PrepareAllHostsSettlementReceipt(
      const std::vector<SingleHostSettlementData>& host_report_list) = 0;
  virtual receipt_uptr PrepareDetailReportReceipt(int transaction_number,
                                                  int card_type_number) = 0;

  virtual receipt_uptr PrepareInstalmentSaleReceipt(CopyType copy,
                                                    ReprintType reprint,
                                                    CvmType cvm_type) =0;

};
}

#endif
