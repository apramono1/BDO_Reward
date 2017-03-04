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
#include <bdor/settlement_workflow.h>

#include <tpcore/telium_event.h>
#include <tpcore/goal_handle.h>
#include <stdx/string>
#include <utils/logger.h>
#include <idalloc/idalloc.h>
#include <bdor/application_settings.h>

#include "reversal_processing.h"
#include "advice_processing.h"
#include "workflow_activities.h"
#include "app_counter.h"

namespace bdor {

SettlementWorkflow::SettlementWorkflow(ApplicationSettings& app_setting,
                                       Batch& batch,
                                       HostSwitch& host,
                                       Ui& ui,
                                       Receipts& receipts)
    : app_setting_(app_setting),
      batch_(batch),
      host_(host),
      ui_(ui),
      receipts_(receipts) {
}

void SettlementWorkflow::Start() {
  logger::debug("Starting Settlement");

  bool settlement_successful = false;
  auto settlement_option = Ui::SettlementOption::SETTLE_BY_HOST;
  auto host_list =
      app_setting_.managed_settings_->GetAllHostDefinitions();

  if (host_list.size() > 1
      &&
      app_setting_.managed_settings_->terminal_config.TxSettleAllHostEnabled()) {
    if (!ui_.SelectSettlementOption(settlement_option))
      return;
  }

  if (settlement_option == Ui::SettlementOption::SETTLE_BY_HOST) {
    std::vector<std::string> host_name_list;
    for (const auto& i : host_list)
      host_name_list.push_back(i.host_name);

    unsigned int pos = 0;
    if (!ui_.SelectHost(host_name_list, pos)) {
      return;
    }
    auto& host_config = host_list.at(pos);

    SingleHostSettlementData settlement_data = ProcessSettlement(
        host_config, settlement_option);
    if (settlement_data.settlement_status != SettlementStatus::SUCCESS) {
      return;
    }

    PrintSingleHostSettlementReport(settlement_data);
    batch_.CloseBatch(host_config.index);

    PerformPreAuthPurge(host_config.index);

  } else if (settlement_option == Ui::SettlementOption::SETTLE_ALL) {
    settlement_successful = true;
    AllHostsSettlementData all_hosts_settlement_data;

    for (auto& host_config : host_list) {
      auto single_host_settlement_data = ProcessSettlement(host_config,
                                                           settlement_option);
      if (single_host_settlement_data.settlement_status
          != SettlementStatus::NONE)
        all_hosts_settlement_data.host_report_list.push_back(
            single_host_settlement_data);
      settlement_successful |=
          (single_host_settlement_data.settlement_status
              == SettlementStatus::SUCCESS);
    }

    if (!settlement_successful) {
      ui_.MessageSettlementFailed();
    }

    ComputeAndPrintAllHostsSettlementReport(all_hosts_settlement_data);

    // Close all successful batch after grand total
    for (const auto& host_report : all_hosts_settlement_data.host_report_list) {
      if (host_report.settlement_status == SettlementStatus::SUCCESS)
        batch_.CloseBatch(host_report.report.host_index);
    }

    PerformPreAuthPurge();
  }

}

bool SettlementWorkflow::PosStart() {
  logger::debug("Starting Settlement");

  bool settlement_successful = false;
  auto settlement_option = Ui::SettlementOption::SETTLE_ALL;
  auto host_list =
      app_setting_.managed_settings_->GetAllHostDefinitions();

  if (settlement_option == Ui::SettlementOption::SETTLE_ALL) {
    settlement_successful = true;
    AllHostsSettlementData all_hosts_settlement_data;

    for (auto& host_config : host_list) {
      auto single_host_settlement_data = ProcessSettlement(host_config,
                                                           settlement_option);
      if (single_host_settlement_data.settlement_status
          != SettlementStatus::NONE)
        all_hosts_settlement_data.host_report_list.push_back(
            single_host_settlement_data);
      settlement_successful &=
          (single_host_settlement_data.settlement_status
              == SettlementStatus::SUCCESS);
    }

    if (!settlement_successful) {
      ui_.MessageSettlementFailed();
    }

    ComputeAndPrintAllHostsSettlementReport(all_hosts_settlement_data);

    // Close all successful batch after grand total
    for (const auto& host_report : all_hosts_settlement_data.host_report_list) {
      if (host_report.settlement_status == SettlementStatus::SUCCESS)
        batch_.CloseBatch(host_report.report.host_index);
    }

    PerformPreAuthPurge();
  }
  return settlement_successful;
}

SingleHostSettlementData SettlementWorkflow::ProcessSettlement(
    const HostDefinition& host_config,
    Ui::SettlementOption settlement_option) {

  SingleHostSettlementData output;
  output.settlement_datetime = 0;
  output.batch_upload_performed = false;
  output.settlement_status = SettlementStatus::NONE;

  ui_.MessageSettlementInProgress(host_config.host_name);

  if (batch_.IsBatchClosed(host_config.index)) {
    ui_.MessageDisplayBatchEmpty(host_config.host_name);
    output.settlement_status = SettlementStatus::NONE;
    return output;
  }

  if (settlement_option == Ui::SettlementOption::SETTLE_BY_HOST) {
    bool settlement_needed = batch_.IsSettlementNeeded(host_config.index);

    if (!settlement_needed) {
      ui_.MessageDisplayBatchEmpty(host_config.host_name);
      output.settlement_status = SettlementStatus::NONE;
      return output;
    }
  }

  SettlementData settlement_data;
  settlement_data.batch_number = batch_.GetCurrentBatchNumber(
      host_config.index);
  settlement_data.nii = utils::FromString<uint32_t>(host_config.settlement_nii);
  settlement_data.tpdu = host_config.settlement_tpdu;
  settlement_data.tid = host_config.terminal_id;
  settlement_data.mid = host_config.merchant_id;
  settlement_data.stan = GetNextStanNo();

  settlement_data.batch_summary = batch_.GetBatchTotalsForHost(
      host_config.index);

  if (settlement_option == Ui::SettlementOption::SETTLE_BY_HOST) {
    if (!ui_.DisplayTransactionCountAndTotal(
        settlement_data.batch_summary.currency,
        settlement_data.batch_summary.sales_total.count,
        settlement_data.batch_summary.sales_total.total,
        settlement_data.batch_summary.refunds_total.count,
        settlement_data.batch_summary.refunds_total.total)) {

      output.settlement_status = SettlementStatus::NONE;
      return output;
    }
  }

  batch_.LockBatch(host_config.index);

  ui_.MessageSettlementInProgress(host_config.host_name);

  ui_.MessageCommunicatingHost();

  if (!host_.PreConnect(host_config.index)) {
    host_.Disconnect();
    ui_.MessagePleaseTryAgain();
    output.settlement_status = SettlementStatus::FAILED;
    output.report = batch_.GetBatchTotalsForReport(host_config.index);
    output.settlement_datetime = settlement_data.tx_datetime;
    return output;
  }
  if (!ProcessPendingReversals(batch_, host_, host_config.index)) {
    host_.Disconnect();
    ui_.MessagePleaseTryAgain();
    output.settlement_status = SettlementStatus::FAILED;
    output.report = batch_.GetBatchTotalsForReport(host_config.index);
    output.settlement_datetime = settlement_data.tx_datetime;
    return output;
  }

  if (!ProcessAllPendingAdvices(batch_, host_, host_config.index)) {
    host_.Disconnect();
    ui_.MessagePleaseTryAgain();
    output.settlement_status = SettlementStatus::FAILED;
    output.report = batch_.GetBatchTotalsForReport(host_config.index);
    output.settlement_datetime = settlement_data.tx_datetime;
    return output;
  }

  output.report = batch_.GetBatchTotalsForReport(host_config.index);

  HostSwitch::Status host_status = host_.PerformSettlement(settlement_data,
                                                           false);

  if (host_status != HostSwitch::Status::COMPLETED) {
    host_.Disconnect();
    ui_.MessagePleaseTryAgain();
    output.settlement_status = SettlementStatus::FAILED;
    output.settlement_datetime = settlement_data.tx_datetime;
    return output;
  }

  if (settlement_data.response_code != "00"
      && settlement_data.response_code != "95") {
    host_.Disconnect();
    ui_.DisplayResponseMessage(settlement_data.response_code);
    output.settlement_status = SettlementStatus::FAILED;
    output.settlement_datetime = settlement_data.tx_datetime;
    return output;
  }

  if (host_config.batch_upload_enabled
      && settlement_data.response_code == "95") {

    // BATCH UPLOAD NEEDED
    logger::error("FDMS - Settlement - Batch Upload needed for Host: %s",
                  host_config.host_name.c_str());
    ui_.MessageBatchUploadInProgress(host_config.host_name);

    auto batch_upload_tx_list = batch_.GetBatchUploadTransactions(
        host_config.index);
    host_status = host_.PerformBatchUpload(batch_upload_tx_list);
    if (host_status != HostSwitch::Status::COMPLETED) {
      host_.Disconnect();
      ui_.MessagePleaseTryAgain();
      output.settlement_status = SettlementStatus::FAILED;
      output.settlement_datetime = settlement_data.tx_datetime;
      return output;
    }

    output.batch_upload_performed = true;

    // BATCH UPLOAD SUCCESSFUL HERE
    settlement_data.stan = GetNextStanNo();
    host_status = host_.PerformSettlement(settlement_data, true);  // TODO: check if need to rewrite settlement_data
    if (host_status != HostSwitch::Status::COMPLETED
        || settlement_data.response_code != "00") {
      host_.Disconnect();
      ui_.DisplayResponseMessage(settlement_data.response_code);
      output.settlement_status = SettlementStatus::FAILED;
      output.settlement_datetime = settlement_data.tx_datetime;
      return output;
    }
  }

  // SETTLEMENT SUCCESSFUL HERE
  host_.Disconnect();
  ui_.MessageSettlementSuccessful();

  output.report = batch_.GetBatchTotalsForReport(host_config.index);
  output.settlement_datetime = settlement_data.tx_datetime;
  output.settlement_status = SettlementStatus::SUCCESS;

  return output;
}

void SettlementWorkflow::PrintSingleHostSettlementReport(
    const SingleHostSettlementData& settlement_data) {
  auto receipt = receipts_.GetSingleHostSettlementReceipt(app_setting_,
                                                          settlement_data);

  PrintLoop(ui_, *receipt);

  SaveLastSettlement(receipt);
}

void SettlementWorkflow::ComputeAndPrintAllHostsSettlementReport(
    AllHostsSettlementData& all_host_settlement_data) {

  // compute grand grand totals
  all_host_settlement_data.global_totals.currency = app_setting_
      .GetTerminalConfiguration().TerminalBaseCurrency();
  for (const auto& single_host_data : all_host_settlement_data.host_report_list) {
    if (single_host_data.settlement_status == SettlementStatus::SUCCESS) {
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
  }

  auto receipt = receipts_.GetAllHostsSettlementReceipt(
      app_setting_,
      all_host_settlement_data);
  PrintLoop(ui_, *receipt);

  SaveLastSettlement(receipt);
}

void SettlementWorkflow::PerformPreAuthPurge(unsigned int host_index) {
  TerminalConfiguration::PreAuthClearMode clear_mode =
      app_setting_.managed_settings_->terminal_config.PreAuthClearPolicy();
  bool ret;

  switch (clear_mode) {
    case TerminalConfiguration::PreAuthClearMode::SETTLE_NO_PROMPT_AND_CLEAR:
      batch_.PurgeOldPreAuth(host_index);
      break;
    case TerminalConfiguration::PreAuthClearMode::SETTLE_PROMPT_AND_CLEAR_IF_REQUESTED:
      ret = ui_.AskToDeleteOldPreAuth();
      if (ret) {
        batch_.PurgeOldPreAuth(host_index);
      }
      break;
    default:
      break;
  }
}

void SettlementWorkflow::PerformPreAuthPurge() {
  TerminalConfiguration::PreAuthClearMode clear_mode =
      app_setting_.managed_settings_->terminal_config.PreAuthClearPolicy();

  if (clear_mode
      == TerminalConfiguration::PreAuthClearMode::SETTLE_PROMPT_AND_CLEAR_IF_REQUESTED) {
    bool ret = ui_.AskToDeleteOldPreAuth();
    if (!ret) {
      return;
    }
  }

  if ((clear_mode
      == TerminalConfiguration::PreAuthClearMode::SETTLE_PROMPT_AND_CLEAR_IF_REQUESTED)
      ||
      (clear_mode
          == TerminalConfiguration::PreAuthClearMode::SETTLE_NO_PROMPT_AND_CLEAR)) {

    auto host_list =
        app_setting_.managed_settings_->GetAllHostDefinitions();

    for (const auto& host : host_list) {
      batch_.PurgeOldPreAuth(host.index);
    }
  }

}

void SettlementWorkflow::SaveLastSettlement(
    const Receipts::receipt_uptr& receipt) const {

  tpcore::GoalHandle gl;

  receipt->SaveToFile(app_setting_.GetSettlementReceiptBmpPath().c_str(),
                      receipt::MimeType::MIME_BMP);
}

}

