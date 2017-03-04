/*
 ------------------------------------------------------------------------------
 INGENICO Technical Software Department
 ------------------------------------------------------------------------------
 Copyright (c) 2015 INGENICO S.A.
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
#include <bdor/sqlite_batch.h>
#include <utils/strings.h>
#include <utils/logger.h>
#include <idalloc/idalloc.h>

namespace bdor {

using std::uint64_t;

namespace {

std::string GetBatchNumberIdAllocKey(unsigned int host_index) {
  static const std::string kBaseKey = "FDMS_batch_number_for_host_";
  return kBaseKey + utils::ToString(host_index);
}

uint64_t GetNextBatchNumber(unsigned int host_index) {
  const uint64_t kMinBatchNumber = 1;
  const uint64_t kMaxBatchNumber = 999999;  // TODO: to check
  std::string idalloc_key = GetBatchNumberIdAllocKey(host_index);

  if (!idalloc::IsCounter(idalloc_key)) {
    idalloc::InitCounter(idalloc_key, kMinBatchNumber, kMaxBatchNumber);
  }
  return idalloc::NextValue(idalloc_key);
}

void UpdateTotal(sqlite::MappedQuery<CountAndAmount, ByHostIndex> &query,
                 unsigned int host_index,
                 BatchTotal& batch_total) {
  auto rows = query.AllRows( { host_index });
  if (!rows.empty()) {
    batch_total.count = rows.front().count;
    batch_total.total = rows.front().amount;
  }
}

void UpdateIssuerTotal(
    sqlite::MappedQuery<CountAndAmount, ByHostIndexAndIssuer> &query,
    unsigned int host_index,
    unsigned int issuer_index,
    BatchTotal& batch_total) {
  auto rows = query.AllRows( { host_index, issuer_index });
  if (!rows.empty()) {
    batch_total.count = rows.front().count;
    batch_total.total = rows.front().amount;
  }
}

}

SqliteBatch::SqliteBatch(ApplicationSettings& app_settings, const char* path,
                         bool migrate_batch)
    : app_settings_(app_settings),
      db_(sqlite::Db(path)),
      tx_table_(db_),
      batch_info_table_(db_) {

  if (migrate_batch) {
    tx_table_.DropTable();
    batch_info_table_.DropTable();
  }
  tx_table_.CreateTable();
  batch_info_table_.CreateTable();
}

bool SqliteBatch::OpenBatch(unsigned int host_index) {
  BatchInfo batch_info = GetBatchInfo(host_index);

  if (batch_info.rowid == 0) {
    // new batch
    batch_info.batch_status = BATCH_OPEN;
    batch_info.current_batch_num = GetNextBatchNumber(host_index);
    batch_info.host_index = host_index;
    batch_info.opening_datetime = stdx::time(nullptr);
    return batch_info_table_.Insert(batch_info) == sqlite::kSqlite_DONE;
  }

  // existing batch info, we make sure it's in closed state first
  if (batch_info.batch_status != BATCH_CLOSED) {
    logger::error(
        "FDMS Batch - Attempting to open a non-closed batch: Host:%d, Batch:%d",
        host_index,
        batch_info.current_batch_num);
    return false;
  }

  batch_info.current_batch_num = GetNextBatchNumber(host_index);
  batch_info.opening_datetime = stdx::time(nullptr);
  batch_info.batch_status = BATCH_OPEN;
  return batch_info_table_.Update(batch_info) == sqlite::kSqlite_DONE;
}

bool SqliteBatch::LockBatch(unsigned int host_index) {
  return SetBatchStatus(host_index, BATCH_SETTLE_IN_PROGRESS);
}

bool SqliteBatch::CloseBatch(unsigned int host_index) {
  ClearBatch(host_index);
  return SetBatchStatus(host_index, BATCH_CLOSED);
}

BatchStatus SqliteBatch::GetBatchStatus(unsigned int host_index) {
  return GetBatchInfo(host_index).batch_status;
}

BatchInfo SqliteBatch::GetBatchInfo(unsigned int host_index) {
  auto info = GetBatchInfoByHostIndexQuery(db_).AllRows( { host_index });
  return info.empty() ? BatchInfo() : info.front();
}

int SqliteBatch::GetCurrentBatchNumber(unsigned int host_index) {
  return GetBatchInfo(host_index).current_batch_num;
}

bool SqliteBatch::IsBatchOpen(unsigned int host_index) {
  return GetBatchStatus(host_index) == BATCH_OPEN;
}

bool SqliteBatch::IsBatchLocked(unsigned int host_index) {
  return GetBatchStatus(host_index) == BATCH_SETTLE_IN_PROGRESS;
}

bool SqliteBatch::IsBatchClosed(unsigned int host_index) {
  return GetBatchStatus(host_index) == BATCH_CLOSED;
}

bool SqliteBatch::IsAnyBatchLocked() {
  auto records = GetBatchStatusQuery(db_).AllRows(
                                                  { BATCH_SETTLE_IN_PROGRESS });
  return !records.empty() && records.front().count > 0;
}

bool SqliteBatch::IsAnyBatchOpen() {
  auto records = GetBatchStatusQuery(db_).AllRows( { BATCH_OPEN });
  return !records.empty() && records.front().count > 0;
}

bool SqliteBatch::Insert(Transaction& txn) {
  return tx_table_.Insert(txn) == sqlite::kSqlite_DONE;
}

bool SqliteBatch::Update(Transaction& txn) {
  return tx_table_.Update(txn) == sqlite::kSqlite_DONE;
}

bool SqliteBatch::InsertOrUpdate(Transaction& txn) {
  return (txn.rowid == 0) ? Insert(txn) : Update(txn);
}

bool SqliteBatch::Delete(Transaction& txn) {
  return tx_table_.Delete(txn.rowid) == sqlite::kSqlite_DONE;
}

Transactions SqliteBatch::GetAllTransactions(unsigned int host_index) {
  auto& query = GetTxAllQuery(db_);
  return query.AllRows( { host_index });
}

Transactions SqliteBatch::FindTransactionByInvoice(
    const unsigned int invoice_num) {
  return GetTxByInvoiceQuery(db_).AllRows( { invoice_num });
}

Transactions SqliteBatch::FindTransactionByStan(unsigned int stan) {
  return GetTxByStanQuery(db_).AllRows( { stan });
}

Transactions SqliteBatch::FindTransactionByPan(const types::Pan& pan) {
  return GetTxByPanQuery(db_).AllRows(pan);
}

Transactions SqliteBatch::FindTransactionByAmount(const types::Amount& amount) {
  return GetTxByAmountQuery(db_).AllRows( { amount.GetValue() });
}

Transactions SqliteBatch::GetPendingAdvices(unsigned int host_index) {
  return GetTxPendingAdviceQuery(db_).AllRows( { host_index });
}

Transactions SqliteBatch::GetPendingTcUpload(unsigned int host_index) {
  return GetTxTcUploadQuery(db_).AllRows( { host_index });
}

Transactions SqliteBatch::GetPendingPreAuth(unsigned int host_index) {
  return GetTxPendingPreAuthQuery(db_).AllRows( { host_index });
}

bool SqliteBatch::IsPreAuthPending(unsigned int host_index) {
  auto &query = GetTxPendingPreAuthQuery(db_).Bind( { host_index });
  return query.begin() != query.end();
}

bool SqliteBatch::PurgeOldPreAuth(unsigned int host_index) {
  uint64_t preauth_max_days = app_settings_.GetTerminalConfiguration()
      .PreAuthMaxAge();
  std::time_t cutoff = stdx::time(NULL) - (preauth_max_days * 24 * 60 * 60);

  auto& stmt = GetPurgeOldPreAuthStatement(db_);
  return stmt.Delete( { host_index, cutoff }) == sqlite::kSqlite_DONE;
}

bool SqliteBatch::PurgeAllPreAuth(unsigned int host_index) {
  auto& stmt = GetPurgeAllPreAuthStatement(db_);
  return stmt.Delete( { host_index }) == sqlite::kSqlite_DONE;
}

Transactions SqliteBatch::GetReversal(unsigned int host_index) {
  auto &query = GetTxReversalQuery(db_);
  return query.AllRows( { host_index });
}

bool SqliteBatch::IsReversalPending(unsigned int host_index) {
  auto &query = GetTxReversalQuery(db_).Bind( { host_index });
  return query.begin() != query.end();
}

bool SqliteBatch::ClearReversal(unsigned int host_index) {
  auto& stmt = GetClearReversalStatement(db_);
  return stmt.Delete( { host_index }) == sqlite::kSqlite_DONE;
}

bool SqliteBatch::ClearBatch(unsigned int host_index) {
  auto& stmt = GetClearBatchStatement(db_);
  return stmt.Delete( { host_index }) == sqlite::kSqlite_DONE;
}

bool SqliteBatch::IsSettlementNeeded(unsigned int host_index) {
  auto stmt = GetBatchCountQuery(db_).AllRows( {host_index} );
  return stmt.empty() ? false : true;
}

BatchTotalsForHost SqliteBatch::GetBatchTotalsForHost(unsigned int host_index) {
  BatchTotalsForHost output;
  output.currency = app_settings_.GetTerminalConfiguration()
      .TerminalBaseCurrency();

  UpdateTotal(GetSaleTotalsForHostQuery(db_), host_index, output.sales_total);
  UpdateTotal(GetRefundTotalsForHostQuery(db_), host_index,
              output.refunds_total);

  return output;
}

Transactions SqliteBatch::GetBatchUploadTransactions(unsigned int host_index) {
  auto& query = GetTxBatchUpload(db_);
  return query.AllRows( { host_index });
}

DetailReportSingleHost SqliteBatch::GetBatchDetailTransactions(
    unsigned int host_index) {
  DetailReportSingleHost output;

  output.batch_number = GetCurrentBatchNumber(host_index);

  output.host_index = host_index;

  output.transaction_count = 0;

  auto& query_batch_detail = GetTxBatchDetail(db_);
  output.transaction_list = query_batch_detail.AllRows( { host_index });

  //Calc Card Total
  // get list of unique issuer_index for that host
  auto& query = GetUniqueIssuerIndexQuery(db_);
  auto issuer_list = query.AllRows( { host_index });

  for (const auto& issuer : issuer_list) {
    unsigned int issuer_index = issuer.issuer_index;
    BatchTotalsForReport batch_totals;

    auto issuer_definition =
        app_settings_.managed_settings_->GetIssuerDefinition(
                                                             issuer_index);
    if (issuer_definition) {
      batch_totals.card_type = issuer_definition->issuer_name;
    }

    batch_totals.currency =
        app_settings_.GetTerminalConfiguration().TerminalBaseCurrency();
    UpdateIssuerTotal(GetSaleTotalsForReportQuery(db_), host_index,
                      issuer_index,
                      batch_totals.sales_total);
    UpdateIssuerTotal(GetTipTotalsForReportQuery(db_), host_index,
                      issuer_index,
                      batch_totals.tips_total);
    UpdateIssuerTotal(GetRefundTotalsForReportQuery(db_), host_index,
                      issuer_index,
                      batch_totals.refunds_total);
    UpdateIssuerTotal(GetOfflineSaleTotalsForReportQuery(db_), host_index,
                      issuer_index,
                      batch_totals.offline_sales_total);
    UpdateIssuerTotal(GetVoidSaleTotalsForReportQuery(db_), host_index,
                      issuer_index,
                      batch_totals.void_sales_total);
    UpdateIssuerTotal(GetVoidRefundTotalsForReportQuery(db_), host_index,
                      issuer_index,
                      batch_totals.void_refunds_total);

    // host total
    batch_totals.card_total += batch_totals.sales_total;
    batch_totals.card_total += batch_totals.tips_total;
    batch_totals.card_total += batch_totals.offline_sales_total;

    // update grand total
    output.issuer_totals_list.push_back(batch_totals);

    //Update TXN count
    output.transaction_count += batch_totals.sales_total.count
        + batch_totals.refunds_total.count
        + batch_totals.offline_sales_total.count
        + batch_totals.void_sales_total.count
        + batch_totals.void_refunds_total.count;

  }

  return output;
}

BatchTotalsReport SqliteBatch::GetBatchTotalsForReport(
    unsigned int host_index) {
  BatchTotalsReport output;
  output.host_index = host_index;
  output.batch_number = GetCurrentBatchNumber(host_index);

  // get list of unique issuer_index for that host
  auto& query = GetUniqueIssuerIndexQuery(db_);
  auto issuer_list = query.AllRows( { host_index });

  for (const auto& issuer : issuer_list) {
    unsigned int issuer_index = issuer.issuer_index;
    BatchTotalsForReport batch_totals;

    auto issuer_definition = app_settings_.managed_settings_
        ->GetIssuerDefinition(issuer_index);
    if (issuer_definition) {
      batch_totals.card_type = issuer_definition->issuer_name;
    }

    batch_totals.currency = app_settings_.GetTerminalConfiguration()
        .TerminalBaseCurrency();
    UpdateIssuerTotal(GetSaleTotalsForReportQuery(db_), host_index,
                      issuer_index,
                      batch_totals.sales_total);
    UpdateIssuerTotal(GetTipTotalsForReportQuery(db_), host_index, issuer_index,
                      batch_totals.tips_total);
    UpdateIssuerTotal(GetRefundTotalsForReportQuery(db_), host_index,
                      issuer_index,
                      batch_totals.refunds_total);
    UpdateIssuerTotal(GetOfflineSaleTotalsForReportQuery(db_), host_index,
                      issuer_index,
                      batch_totals.offline_sales_total);
    UpdateIssuerTotal(GetVoidSaleTotalsForReportQuery(db_), host_index,
                      issuer_index,
                      batch_totals.void_sales_total);
    UpdateIssuerTotal(GetVoidRefundTotalsForReportQuery(db_), host_index,
                      issuer_index,
                      batch_totals.void_refunds_total);

    // card total
    batch_totals.card_total += batch_totals.sales_total;
    batch_totals.card_total += batch_totals.tips_total;
    batch_totals.card_total += batch_totals.offline_sales_total;

    // update grand total
    output.global_batch_totals.currency =
        app_settings_.GetTerminalConfiguration()
            .TerminalBaseCurrency();
    output.global_batch_totals.sales_total += batch_totals.sales_total;
    output.global_batch_totals.tips_total += batch_totals.tips_total;
    output.global_batch_totals.refunds_total += batch_totals.refunds_total;
    output.global_batch_totals.offline_sales_total += batch_totals
        .offline_sales_total;
    output.global_batch_totals.void_sales_total +=
        batch_totals.void_sales_total;
    output.global_batch_totals.void_refunds_total +=
        batch_totals.void_refunds_total;
    output.global_batch_totals.total += batch_totals.card_total;

    output.batch_totals_list.push_back(batch_totals);

  }

  return output;
}

AllHostBatchTotalsReport SqliteBatch::GetAllHostsBatchTotalsForReport() {
  AllHostBatchTotalsReport output;

  output.global_batch_totals.currency = app_settings_.GetTerminalConfiguration()
      .TerminalBaseCurrency();

  auto host_list = app_settings_.managed_settings_->GetAllHostDefinitions();  // TODO: check if we should get the list of host from the batch instead
  for (const auto& host : host_list) {
    BatchTotalsReport host_report = GetBatchTotalsForReport(host.index);

    output.global_batch_totals.sales_total += host_report.global_batch_totals
        .sales_total;
    output.global_batch_totals.tips_total += host_report.global_batch_totals
        .tips_total;
    output.global_batch_totals.refunds_total += host_report.global_batch_totals
        .refunds_total;
    output.global_batch_totals.offline_sales_total += host_report
        .global_batch_totals.offline_sales_total;
    output.global_batch_totals.void_sales_total += host_report
        .global_batch_totals.void_sales_total;
    output.global_batch_totals.void_refunds_total += host_report
        .global_batch_totals.void_refunds_total;
    output.global_batch_totals.total += host_report.global_batch_totals.total;

    output.batch_totals_report_list.push_back(host_report);
  }

  return output;
}

// PRIVATE METHODS FROM HERE ONWARDS

bool SqliteBatch::SetBatchStatus(unsigned int host_index,
                                 BatchStatus batch_status) {
  auto batch_info = GetBatchInfo(host_index);
  if (batch_info.current_batch_num != 0) {
    batch_info.batch_status = batch_status;
    return batch_info_table_.Update(batch_info) == sqlite::kSqlite_DONE;
  }

  return false;
}

}
