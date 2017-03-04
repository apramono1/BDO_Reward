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
#include <bdor/summary_report_workflow.h>
#include "workflow_activities.h"
#include <bdor/batch.h>
#include <bdor/batch_data_types.h>
#include <utils/logger.h>
#include <stdx/ctime>

namespace bdor {

SummaryReportWorkflow::SummaryReportWorkflow(
                                             ApplicationSettings& app_setting,
                                             Batch& batch,
                                             Ui& ui,
                                             Receipts& receipts)
    : app_setting_(app_setting),
      batch_(batch),
      ui_(ui),
      receipts_(receipts) {
}

bool SummaryReportWorkflow::Start() {

  logger::debug("Starting Summary Report");

  auto report_option = Ui::SettlementOption::SETTLE_BY_HOST;  // TODO: refactor: change SettlementOption to SelectHostOption, to use for multiple purposes.
  auto host_list =
      app_setting_.managed_settings_->GetAllHostDefinitions();

  if (host_list.size() > 1) {
    if (!ui_.SelectReportOption(report_option))
      return false;
  }

  if (report_option == Ui::SettlementOption::SETTLE_BY_HOST) {
    return SelectAndPrintSingleHostReport(host_list);

  } else if (report_option == Ui::SettlementOption::SETTLE_ALL) {
    return PrintAllHostReport(host_list);
  }
  return false;
}

bool SummaryReportWorkflow::SelectAndPrintSingleHostReport(
    std::vector<HostDefinition>& host_list) const {
  std::vector<std::string> host_name_list;
  for (const auto& i : host_list)
    host_name_list.push_back(i.host_name);

  unsigned int pos = 0;
  if (!ui_.SelectHost(host_name_list, pos)) {
    return false;
  }
  auto& host_config = host_list.at(pos);

  auto report_data = GetSingleHostSummaryReportData(host_config);
  if (report_data == nullptr) {
    ui_.MessageDisplayBatchEmpty(host_config.host_name);
    return false;
  }

  auto receipt = receipts_.GetSingleHostSettlementReceipt(app_setting_,
                                                          *report_data);
  PrintLoop(ui_, *receipt);

  return true;
}

bool SummaryReportWorkflow::PrintAllHostReport(
    std::vector<HostDefinition>& host_list) const {
  AllHostsSettlementData all_hosts_data;

  for (auto& host_config : host_list) {
    auto single_host_data = GetSingleHostSummaryReportData(host_config);
    if (single_host_data != nullptr) {
      all_hosts_data.host_report_list.push_back(*single_host_data);
    }
  }

  if (!all_hosts_data.host_report_list.empty())
    ComputeAndPrintAllHostsSummaryReport(all_hosts_data);
  else
    ui_.MessageDisplayBatchEmpty();

  return true;
}

std::unique_ptr<SingleHostSettlementData> SummaryReportWorkflow::GetSingleHostSummaryReportData(
    const HostDefinition& host_config) const {
  std::unique_ptr<SingleHostSettlementData> output = nullptr;

  SingleHostSettlementData report_data;
  report_data.report_type = HostReportType::SUMMARY_REPORT;
  report_data.settlement_datetime = stdx::time(NULL);
  report_data.report = batch_.GetBatchTotalsForReport(host_config.index);

  if (IsBatchEmpty(host_config, report_data))
    return output;

  output = stdx::make_unique<SingleHostSettlementData>(report_data);
  return output;
}

void SummaryReportWorkflow::ComputeAndPrintAllHostsSummaryReport(
    AllHostsSettlementData& all_host_settlement_data) const {

  // compute grand grand totals
  all_host_settlement_data.global_totals.currency = app_setting_
      .GetTerminalConfiguration().TerminalBaseCurrency();
  for (const auto& single_host_data : all_host_settlement_data.host_report_list) {
    all_host_settlement_data.global_totals.sales_total += single_host_data
        .report.global_batch_totals.sales_total;
    all_host_settlement_data.global_totals.tips_total += single_host_data
        .report.global_batch_totals.tips_total;
    all_host_settlement_data.global_totals.refunds_total += single_host_data
        .report.global_batch_totals.refunds_total;
    all_host_settlement_data.global_totals.offline_sales_total +=
        single_host_data.report.global_batch_totals.offline_sales_total;
    all_host_settlement_data.global_totals.void_sales_total +=
        single_host_data.report.global_batch_totals.void_sales_total;
    all_host_settlement_data.global_totals.void_refunds_total +=
        single_host_data.report.global_batch_totals.void_refunds_total;
    all_host_settlement_data.global_totals.total += single_host_data.report
        .global_batch_totals.total;
  }

  auto receipt = receipts_.GetAllHostsSettlementReceipt(
      app_setting_, all_host_settlement_data);
  PrintLoop(ui_, *receipt);
}

bool SummaryReportWorkflow::IsBatchEmpty(
    const HostDefinition& host_config,
    const SingleHostSettlementData& report_data) const {

  if (batch_.IsBatchClosed(host_config.index))
    return true;

  if (report_data.report.global_batch_totals.total.count == 0)
    return true;

  return false;
}

}

