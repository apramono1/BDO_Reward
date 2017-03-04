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
#include <bdor/batch_total_workflow.h>
#include "workflow_activities.h"
#include <bdor/batch.h>
#include <bdor/batch_data_types.h>
#include <utils/logger.h>
#include <stdx/ctime>

namespace bdor {

BatchTotalWorkflow::BatchTotalWorkflow(
                                       ApplicationSettings& app_setting,
                                       Batch& batch,
                                       Ui& ui,
                                       Receipts& receipts)
    : app_setting_(app_setting),
      batch_(batch),
      ui_(ui),
      receipts_(receipts) {
}

bool BatchTotalWorkflow::Start() {

  logger::debug("Starting Batch total");

  auto host_list =
      app_setting_.managed_settings_->GetAllHostDefinitions();

  if (host_list.empty())
    return true;

  AllHostsSettlementData all_hosts_data;

  for (auto& host_config : host_list) {
    auto single_host_data = GetSingleHostSummaryReportData(host_config);
    if (single_host_data != nullptr) {
      all_hosts_data.host_report_list.push_back(*single_host_data);
    }
  }

  if (!all_hosts_data.host_report_list.empty()) {
    ComputeAllHostsSummaryReport(all_hosts_data);
    DispatchMenu(all_hosts_data);
  } else {
    ui_.MessageDisplayBatchEmpty();
  }
  return true;
}

std::unique_ptr<SingleHostSettlementData> BatchTotalWorkflow::GetSingleHostSummaryReportData(
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

void BatchTotalWorkflow::ComputeAllHostsSummaryReport(
    AllHostsSettlementData& all_host_data) {

  // compute grand grand totals
  all_host_data.global_totals.currency = app_setting_
      .GetTerminalConfiguration().TerminalBaseCurrency();
  for (const auto& single_host_data : all_host_data.host_report_list) {
    all_host_data.global_totals.sales_total += single_host_data
        .report.global_batch_totals.sales_total;
    all_host_data.global_totals.tips_total += single_host_data
        .report.global_batch_totals.tips_total;
    all_host_data.global_totals.refunds_total += single_host_data
        .report.global_batch_totals.refunds_total;
    all_host_data.global_totals.offline_sales_total +=
        single_host_data.report.global_batch_totals.offline_sales_total;
    all_host_data.global_totals.void_sales_total +=
        single_host_data.report.global_batch_totals.void_sales_total;
    all_host_data.global_totals.void_refunds_total +=
        single_host_data.report.global_batch_totals.void_refunds_total;
    all_host_data.global_totals.total += single_host_data.report
        .global_batch_totals.total;
  }
}

bool BatchTotalWorkflow::IsBatchEmpty(
    const HostDefinition& host_config,
    const SingleHostSettlementData& report_data) const {

  if (batch_.IsBatchClosed(host_config.index))
    return true;

  if (report_data.report.global_batch_totals.total.count == 0)
    return true;

  return false;
}

void BatchTotalWorkflow::PrintAllHostSummaryReport(
    const AllHostsSettlementData& all_host_data) const {
  auto receipt = receipts_.GetAllHostsSettlementReceipt(
                                                        app_setting_,
                                                        all_host_data);
  PrintLoop(ui_, *receipt);
}

void BatchTotalWorkflow::PrintHostSummaryReport(
    const SingleHostSettlementData& report_data) const {
  auto receipt = receipts_.GetSingleHostSettlementReceipt(app_setting_,
                                                            report_data);
    PrintLoop(ui_, *receipt);
}

void BatchTotalWorkflow::DispatchMenu(
    const AllHostsSettlementData& all_host_data) const {

  std::vector<std::string> items;
  for (auto const& menu_item : all_host_data.host_report_list) {
    auto host_config = *(app_setting_.managed_settings_->GetHostDefinition(
        menu_item.report.host_index));
    items.push_back(host_config.host_name);
  }

  if (items.empty())
    return;

  items.push_back(grand_total_label_);

  unsigned long selection = 0;
  while (true) {
    selection = ui_.MenuSelectionIndex("BATCH TOTAL", items);
    if (selection >= items.size())
      return;
    std::unique_ptr<BatchTotalToDisplay> total_display_uptr = nullptr;
    if (selection == items.size() - 1) {
      total_display_uptr = GetDisplayTotalFromGrandTotal(
          all_host_data.global_totals);
    } else {
      auto host_data = all_host_data.host_report_list[selection].report;
      total_display_uptr = GetDisplayTotalFromBatchTotalsReport(host_data);
    }
    auto result = ui_.DisplayBatchTotal(*total_display_uptr);
    if (result == BatchTotalReturn::PRINT_REPORT)
    {
      if(selection == items.size() - 1)
      PrintAllHostSummaryReport(all_host_data);
      else{
        auto host_data = all_host_data.host_report_list[selection];
        PrintHostSummaryReport(host_data);
      }
    }
  }

  return;
}

std::unique_ptr<BatchTotalToDisplay> BatchTotalWorkflow::GetDisplayTotalFromBatchTotalsReport(
    const BatchTotalsReport batch_total) const {
  BatchTotalToDisplay output;

  HostDefinition host_config = *app_setting_.managed_settings_
      ->GetHostDefinition(batch_total.host_index);

  output.host_name = host_config.host_name;
  output.currency = batch_total.global_batch_totals.currency.GetName();
  output.sales_total = batch_total.global_batch_totals.sales_total;
  output.tips_total = batch_total.global_batch_totals.tips_total;
  output.refunds_total = batch_total.global_batch_totals.refunds_total;
  output.offline_sales_total = batch_total.global_batch_totals
      .offline_sales_total;
  output.void_sales_total = batch_total.global_batch_totals.void_sales_total;
  output.void_refunds_total =
      batch_total.global_batch_totals.void_refunds_total;
  output.total = batch_total.global_batch_totals.total;

  return stdx::make_unique<BatchTotalToDisplay>(output);
}

std::unique_ptr<BatchTotalToDisplay> BatchTotalWorkflow::GetDisplayTotalFromGrandTotal(
    const GrandBatchTotalsForReport grand_total) const {
  BatchTotalToDisplay output;

  output.host_name = grand_total_label_;
  output.currency = grand_total.currency.GetName();
  output.sales_total = grand_total.sales_total;
  output.tips_total = grand_total.tips_total;
  output.refunds_total = grand_total.refunds_total;
  output.offline_sales_total = grand_total.offline_sales_total;
  output.void_sales_total = grand_total.void_sales_total;
  output.void_refunds_total = grand_total.void_refunds_total;
  output.total = grand_total.total;

  return stdx::make_unique<BatchTotalToDisplay>(output);
}
}

