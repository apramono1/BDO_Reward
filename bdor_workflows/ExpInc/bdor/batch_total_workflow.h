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
#ifndef BDOR__BATCH_TOTAL_WORKFLOW_H_
#define BDOR__BATCH_TOTAL_WORKFLOW_H_

#include <bdor/batch.h>
#include <bdor/ui.h>
#include <bdor/receipts.h>
#include <bdor/receipt_settlement_data_types.h>

namespace bdor {

class BatchTotalWorkflow {

 public:

  BatchTotalWorkflow(ApplicationSettings& app_setting,
                     Batch& batch,
                     Ui& ui,
                     Receipts& receipts);

  bool Start();

 private:
  ApplicationSettings& app_setting_;
  Batch& batch_;
  Ui& ui_;
  Receipts& receipts_;
  const std::string grand_total_label_ = "GRAND TOTAL";

  bool SelectAndPrintSingleHostReport(
      std::vector<HostDefinition>& host_list) const;
  std::unique_ptr<SingleHostSettlementData> GetSingleHostSummaryReportData(
      const HostDefinition& host_config) const;
  void ComputeAllHostsSummaryReport(
      AllHostsSettlementData& all_host_settlement_data);

  bool IsBatchEmpty(const HostDefinition& host_config,
                    const SingleHostSettlementData& report_data) const;
  void PrintAllHostSummaryReport(
      const AllHostsSettlementData& all_host_settlement_data) const;
  void PrintHostSummaryReport(const SingleHostSettlementData& report_data) const;
  void DispatchMenu(
                    const AllHostsSettlementData& all_host_data) const;

  std::unique_ptr<BatchTotalToDisplay> GetDisplayTotalFromBatchTotalsReport(
      const BatchTotalsReport batch_total) const;

  std::unique_ptr<BatchTotalToDisplay> GetDisplayTotalFromGrandTotal(
      const GrandBatchTotalsForReport grand_total) const;
};

}

#endif

