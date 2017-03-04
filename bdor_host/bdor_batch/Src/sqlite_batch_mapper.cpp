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
#include <bdor/sqlite_batch.h>
#include <sqlite/aid_mapper.h>
#include <sqlite/amount_mapper.h>
#include <sqlite/pan_mapper.h>
#include <sqlite/track2_equivalent_data_mapper.h>

#include <stdx/ctime>

using std::vector;
using std::ostringstream;
using std::string;
using stdx::optional;
using stdx::make_unique;
using types::Currency;
using namespace sqlite;

namespace bdor {

namespace {

const string TX_TABLE = "TX";
const string BATCH_INFO_TABLE = "BATCH_INFO";

BaseMapper MapTxTable(Db& db, Transaction& tx) {
  BaseMapper result(db);
  result.MapAttr("rowid", tx.rowid);
  result.MapAttr("tx_datetime", tx.tx_datetime);
  result.MapAttr("transaction_status", tx.transaction_status);
  result.MapAttr("previous_transaction_status", tx.previous_transaction_status);
  result.MapAttr("in_progress_status", tx.in_progress_status);
  result.MapAttr("tc_upload_status", tx.tc_upload_status);
  result.MapAttr("transaction_type", tx.transaction_type);

  result.MapAttr("is_voided", tx.is_voided);
  result.MapAttr("is_preauth_completed", tx.is_preauth_completed);
  result.MapAttr("is_fallback", tx.is_fallback);
  result.MapAttr("is_adjusted", tx.is_adjusted);

  result.MapAttr("pos_entry_mode", tx.pos_entry_mode);
  result.MapAttr("pos_condition_code", tx.pos_condition_code);
  result.MapAttr("batch_num", tx.batch_num);
  result.MapAttr("invoice", tx.invoice_num);
  result.MapAttr("original_invoice_number", tx.original_invoice_number);
  result.MapAttr("signature_required", tx.signature_required);
  result.MapAttr("online_authorization_required",
                 tx.online_authorization_required);
  result.MapAttr("pin_entered", tx.pin_entered);
  result.MapAttr("express_payment", tx.express_payment);

  result.MapAttr("authorization_stan", tx.stan);
  result.MapAttr("tc_upload_stan", tx.tc_upload_stan);
  result.MapAttr("sale_stan", tx.stan_original);

  result.MapAttr("processing_code", tx.processing_code);
  result.MapAttr("tpdu", tx.tpdu);
  result.MapAttr("nii", tx.nii);

  result.MapAttr("host_index", tx.host_index);
  result.MapAttr("card_index", tx.card_index);
  result.MapAttr("issuer_index", tx.issuer_index);

  result.MapAttr("pan", tx.pan);
  result.MapAttr("pan_sequence_number", tx.pan_sequence_number);
  result.MapAttr("currency_code", "amount", tx.amount);
  result.MapAttr("secondary_amount_currency_code", "secondary_amount",
                 tx.secondary_amount);
  result.MapAttr("original_secondary_amount_currency_code",
                 "original_secondary_amount",
                 tx.original_secondary_amount);

  result.MapAttr("preauth_currency_code", "preauth_amount", tx.preauth_amount);

  result.MapAttr("expiration_date", tx.expiration_date);
  // Track2 not stored

  result.MapAttr("rrn", tx.rrn);
  result.MapAttr("auth_id_response", tx.auth_id_response);
  result.MapAttr("sale_auth_id_response", tx.auth_id_response_original);
  result.MapAttr("response_code", tx.response_code);
  result.MapAttr("tid", tx.tid);
  result.MapAttr("mid", tx.mid);

  result.MapAttr("icc_data", tx.icc_data);
  // CVV not stored

  result.MapAttr("card_holder_name", tx.card_holder_name);
  result.MapAttr("application_prefered_name", tx.application_prefered_name);
  result.MapAttr("card_name", tx.card_name);

  // TODO: amex data

  result.MapAttr("transaction_certificate", tx.transaction_certificate);
  result.MapAttr("tvr", tx.tvr);
  result.MapAttr("tsi", tx.tsi);
  result.MapAttr("service_code", tx.service_code);
  // Track1 not stored

  // PIN data not stored

  result.MapAttr("aid", tx.aid);
  result.MapAttr("available_offline_spending_limit",
                 tx.available_offline_spending_limit);

  //installment data
  result.MapAttr("tenure", tx.tenure);
  result.MapAttr("interest_rate", tx.interest_rate);
  result.MapAttr("roi_tenure", tx.roi_tenure);
  result.MapAttr("interset_currency_code", "interest_amount",
                 tx.interest_amount);
  result.MapAttr("total_amount_currency_code", "total_prod_amount",
                 tx.total_prod_amount);
  result.MapAttr("emi_currency_code", "emi_per_month", tx.emi_per_month);
  result.MapAttr("processing_fee", tx.processing_fee);
  result.MapAttr("program_id", tx.program_id);
  result.MapAttr("product_id", tx.product_id);

  // DCC Data
  result.MapAttr("dcc_amount_currency_code", "dcc_amount",
                 tx.dcc_amount);
  result.MapAttr("dcc_amount_currency_code_original", "dcc_amount_original",
                 tx.dcc_amount_original);
  result.MapAttr("dcc_staff_id", tx.dcc_staff_id);
  result.MapAttr("dcc_conversion_rate", tx.dcc_conversion_rate);
  result.MapAttr("dcc_margin", tx.dcc_margin);
  result.MapAttr("dcc_status", tx.dcc_status);
  result.MapAttr("dcc_provider", tx.dcc_provider);

  return result;
}

BaseMapper MapBatchInfoTable(Db& db, BatchInfo& batch_info) {
  BaseMapper result(db);

  result.MapAttr("rowid", batch_info.rowid);
  result.MapAttr("current_batch_num", batch_info.current_batch_num);
  result.MapAttr("host_index", batch_info.host_index);
  result.MapAttr("batch_status", batch_info.batch_status);
  result.MapAttr("opening_datetime", batch_info.opening_datetime);

  return result;
}

BaseMapper MapByHostIndexParams(Db& db, ByHostIndex& params) {
  BaseMapper result(db);
  result.MapAttr("1", params.host_index);
  return result;
}

BaseMapper MapByInvoiceParams(Db& db, ByInvoice& params) {
  BaseMapper result(db);
  result.MapAttr("1", params.invoice);
  return result;
}

BaseMapper MapByStanParams(Db& db, ByStan& params) {
  BaseMapper result(db);
  result.MapAttr("1", params.stan);
  return result;
}

BaseMapper MapByPanParams(Db& db, ByPan& params) {
  BaseMapper result(db);
  result.MapAttr("1", params.pan);
  return result;
}

BaseMapper MapByAmountParams(Db& db, ByAmount& params) {
  BaseMapper result(db);
  result.MapAttr("1", params.amount);
  return result;
}

BaseMapper MapByHostIndexAndIssuerParams(Db& db,
                                         ByHostIndexAndIssuer & params) {
  BaseMapper result(db);
  result.MapAttr("1", params.host_index);
  result.MapAttr("2", params.issuer);
  return result;
}

BaseMapper MapCountAndTip(Db& db, CountAndAmount& out) {
  BaseMapper result(db);
  result.MapAttr("COUNT(*)", out.count);
  result.MapAttr("TOTAL(secondary_amount)", out.amount);
  return result;
}

BaseMapper MapCountAndAmountResult(Db& db, CountAndAmount& out) {
  BaseMapper result(db);
  result.MapAttr("COUNT(*)", out.count);
  result.MapAttr("TOTAL(amount)", out.amount);
  return result;
}

BaseMapper MapCountAndTipResult(Db& db, CountAndAmount& out) {
  BaseMapper result(db);
  result.MapAttr("COUNT(*)", out.count);
  result.MapAttr("TOTAL(secondary_amount)", out.amount);
  return result;
}

BaseMapper MapCountAndAmountWithTipResult(Db& db, CountAndAmount& out) {
  BaseMapper result(db);
  result.MapAttr("COUNT(*)", out.count);
  result.MapAttr("TOTAL(COALESCE(amount, 0) + COALESCE(secondary_amount, 0))",
                 out.amount);
  return result;
}

BaseMapper MapCountOnly(Db& db, CountOnly& out) {
  BaseMapper result(db);
  result.MapAttr("COUNT(*)", out.count);
  return result;
}

BaseMapper MapUniqueIssuerIndexResult(Db& db, IssuerIndex& out) {
  BaseMapper result(db);
  result.MapAttr("DISTINCT issuer_index", out.issuer_index);
  return result;
}

BaseMapper MapByHostIndexAndCutoffParams(Db& db, ByHostIndexAndCutoff& params) {
  BaseMapper result(db);
  result.MapAttr("1", params.host_index);
  result.MapAttr("2", params.cutoff);
  return result;
}

BaseMapper MapByBatchStatusParams(Db& db, ByBatchStatus& params) {
  BaseMapper result(db);
  result.MapAttr("1", params.batch_status);
  return result;
}

}

TxTable::TxTable(sqlite::Db& db)
    : sqlite::MappedTable<Transaction>(db, TX_TABLE, MapTxTable) {
}

BatchInfoTable::BatchInfoTable(sqlite::Db& db)
    : sqlite::MappedTable<BatchInfo>(db, BATCH_INFO_TABLE, MapBatchInfoTable) {
}

TxAllQuery::TxAllQuery(Db& db)
    : TxQuery<ByHostIndex>(db, MapTxTable, MapByHostIndexParams) {
}

string TxAllQuery::GetTables() {
  return TX_TABLE;
}

string TxAllQuery::GetSelection() {
  return "host_index = ?";
}

string TxAllQuery::GetSortOrder() {
  return "ROWID";
}

TxSuccessQuery::TxSuccessQuery(Db& db)
    : TxQuery<ByHostIndex>(db, MapTxTable, MapByHostIndexParams) {
}

string TxSuccessQuery::GetTables() {
  return TX_TABLE;
}

string TxSuccessQuery::GetSelection() {
  ostringstream oss;
  oss << "host_index = ? AND " << "(transaction_type != "
      << TransactionType::PREAUTH
      << " OR" << " is_preauth_completed = " << true
      << ") AND "
      << "transaction_status = " << TransactionStatus::APPROVED;
  return oss.str();
}

string TxSuccessQuery::GetSortOrder() {
  return "ROWID";
}

TxByInvoiceQuery::TxByInvoiceQuery(Db& db)
    : TxQuery<ByInvoice>(db, MapTxTable, MapByInvoiceParams) {
}

string TxByInvoiceQuery::GetTables() {
  return TX_TABLE;
}

string TxByInvoiceQuery::GetSelection() {
  return "invoice = ?";
}

TxByStanQuery::TxByStanQuery(Db& db)
    : TxQuery<ByStan>(db, MapTxTable, MapByStanParams) {
}

string TxByStanQuery::GetTables() {
  return TX_TABLE;
}

string TxByStanQuery::GetSelection() {
  return "authorization_stan = ?";
}

TxByPanQuery::TxByPanQuery(Db& db)
    : TxQuery<ByPan>(db, MapTxTable, MapByPanParams) {
}

string TxByPanQuery::GetTables() {
  return TX_TABLE;
}

string TxByPanQuery::GetSelection() {
  return "pan = ?";
}

TxByAmountQuery::TxByAmountQuery(Db& db)
    : TxQuery<ByAmount>(db, MapTxTable, MapByAmountParams) {
}

string TxByAmountQuery::GetTables() {
  return TX_TABLE;
}

string TxByAmountQuery::GetSelection() {
  return "amount = ?";
}

TxPendingAdviceQuery::TxPendingAdviceQuery(Db& db)
    : TxQuery<ByHostIndex>(db, MapTxTable, MapByHostIndexParams) {
}

string TxPendingAdviceQuery::GetTables() {
  return TX_TABLE;
}

string TxPendingAdviceQuery::GetSelection() {
  ostringstream oss;
  oss << "host_index = ? AND " << "transaction_status = "
      << TransactionStatus::TO_ADVISE;
  return oss.str();
}

string TxPendingAdviceQuery::GetSortOrder() {
  return "ROWID";
}

TxTcUploadQuery::TxTcUploadQuery(Db& db)
    : TxQuery<ByHostIndex>(db, MapTxTable, MapByHostIndexParams) {
}

string TxTcUploadQuery::GetTables() {
  return TX_TABLE;
}

string TxTcUploadQuery::GetSelection() {
  ostringstream oss;
  oss << "host_index = ? AND " << "tc_upload_status = "
      << TcUploadStatus::TC_UPLOAD_NEEDED;
  return oss.str();
}

string TxTcUploadQuery::GetSortOrder() {
  return "ROWID";
}

TxReversalQuery::TxReversalQuery(Db& db)
    : TxQuery<ByHostIndex>(db, MapTxTable, MapByHostIndexParams) {
}

string TxReversalQuery::GetTables() {
  return TX_TABLE;
}

string TxReversalQuery::GetSelection() {
  ostringstream oss;
  oss << "host_index = ? AND " << "transaction_status IN ("
      << TransactionStatus::TO_REVERSE
      << "," << TransactionStatus::IN_PROGRESS
      << ")";
  return oss.str();
}

string TxReversalQuery::GetSortOrder() {
  return "ROWID";
}

TxPendingPreAuthQuery::TxPendingPreAuthQuery(Db& db)
    : TxQuery<ByHostIndex>(db, MapTxTable, MapByHostIndexParams) {
}

string TxPendingPreAuthQuery::GetTables() {
  return TX_TABLE;
}

string TxPendingPreAuthQuery::GetSelection() {
  ostringstream oss;
  oss << "host_index = ? AND " << "transaction_type ="
      << TransactionType::PREAUTH
      << " AND "
      << "is_preauth_completed = "
      << false << " AND "
      << "is_voided = "
      << false << " AND " << "transaction_status = "
      << TransactionStatus::APPROVED;
  return oss.str();
}

string TxPendingPreAuthQuery::GetSortOrder() {
  return "ROWID";
}

SaleTotalsForHostQuery::SaleTotalsForHostQuery(Db& db)
    : TotalsQuery(db, MapCountAndAmountWithTipResult, MapByHostIndexParams) {
}

string SaleTotalsForHostQuery::GetTables() {
  return TX_TABLE;
}

//To get Total approved + to_advise of SALE, CASH_ADVANCE, OFFLINE_SALE, COMPLETION excluded voided transaction
string SaleTotalsForHostQuery::GetSelection() {
  ostringstream oss;
  oss << "host_index = ?" << " AND (transaction_status = "
      << TransactionStatus::APPROVED
      << " OR transaction_status = "
      << TransactionStatus::TO_ADVISE
      << " )"
      << " AND (transaction_type IN ("
      << TransactionType::SALE << ","
      << TransactionType::CASH_ADVANCE
      << "," << TransactionType::OFFLINE_SALE
      << ","
      << TransactionType::PREAUTH_COMPLETION_OFFLINE
      << ","
      << TransactionType::PREAUTH_COMPLETION_ONLINE
      << ","
      << TransactionType::INSTALMENT_SALE
      << "))"
      << " AND is_voided = " << false << "";

  return oss.str();
}

RefundTotalsForHostQuery::RefundTotalsForHostQuery(Db& db)
    : TotalsQuery(db, MapCountAndAmountWithTipResult, MapByHostIndexParams) {
}

string RefundTotalsForHostQuery::GetTables() {
  return TX_TABLE;
}

// To get total REFUND excluded VOID REFUND
string RefundTotalsForHostQuery::GetSelection() {
  std::stringstream oss;
  oss << "host_index = ?" << " AND transaction_status = "
      << TransactionStatus::APPROVED
      << " AND (transaction_type IN ("
      << TransactionType::REFUND
      << ")" << " AND (is_voided = " << false
      << "))";

  return oss.str();
}

UniqueIssuerIndexQuery::UniqueIssuerIndexQuery(Db& db)
    : sqlite::MappedQuery<IssuerIndex, ByHostIndex>(db,
                                                    MapUniqueIssuerIndexResult,
                                                    MapByHostIndexParams) {
}

string UniqueIssuerIndexQuery::GetTables() {
  return TX_TABLE;
}

string UniqueIssuerIndexQuery::GetSelection() {
  std::stringstream oss;
  oss << "host_index = ?"
      << " AND issuer_index IS NOT NULL";
  return oss.str();
}

SaleTotalsForReportQuery::SaleTotalsForReportQuery(Db& db)
    : IssuerTotalsQuery(db, MapCountAndAmountResult,
                        MapByHostIndexAndIssuerParams) {
}

string SaleTotalsForReportQuery::GetTables() {
  return TX_TABLE;
}

//To get total of SALE + TIP_ADJUST + CASH_ADVANCE + COMPLETION ONLINE excluded voided transactions.
// including EMV approved offline transaction
string SaleTotalsForReportQuery::GetSelection() {
  std::stringstream oss;
  oss << "host_index = ?"
      << "AND issuer_index = ?"
      << " AND ((transaction_status = "
      << TransactionStatus::APPROVED
      << " OR transaction_status = "
      << TransactionStatus::TO_ADVISE
      << ")"
      << " AND (transaction_type IN ("
      << TransactionType::SALE
      << ","
      << TransactionType::CASH_ADVANCE
      << ","
      << TransactionType::PREAUTH_COMPLETION_ONLINE
      << ","
      << TransactionType::INSTALMENT_SALE
      << ")) AND is_voided = "
      << false << ")";

  return oss.str();
}

TipTotalsForReportQuery::TipTotalsForReportQuery(Db& db)
    : IssuerTotalsQuery(db, MapCountAndTipResult, MapByHostIndexAndIssuerParams) {
}

string TipTotalsForReportQuery::GetTables() {
  return TX_TABLE;
}

string TipTotalsForReportQuery::GetSelection() {
  std::stringstream oss;
  oss << "host_index = ?"
      << "AND issuer_index = ?"
      << " AND (transaction_status = "
      << TransactionStatus::APPROVED
      << " OR transaction_status = "
      << TransactionStatus::TO_ADVISE
      << ")"
      << " AND secondary_amount IS NOT NULL"
      << " AND ( (transaction_type IN (" << TransactionType::SALE << ","
      << TransactionType::CASH_ADVANCE
      << "," << TransactionType::OFFLINE_SALE
      << ","
      << TransactionType::PREAUTH_COMPLETION_ONLINE
      << ","
      << TransactionType::PREAUTH_COMPLETION_OFFLINE
      << ","
      << TransactionType::INSTALMENT_SALE
      << "))"
      << " AND is_voided = " << false << ")";

  return oss.str();
}

RefundTotalsForReportQuery::RefundTotalsForReportQuery(Db& db)
    : IssuerTotalsQuery(db, MapCountAndAmountResult,
                        MapByHostIndexAndIssuerParams) {
}

string RefundTotalsForReportQuery::GetTables() {
  return TX_TABLE;
}

string RefundTotalsForReportQuery::GetSelection() {
  std::stringstream oss;
  oss << "host_index = ?"
      << "AND issuer_index = ?"
      << " AND transaction_status = "
      << TransactionStatus::APPROVED
      << " AND ( ( (transaction_type IN ("
      << TransactionType::REFUND
      << "))"
      << " AND (is_voided = "
      << false << ")))";
  return oss.str();
}

OfflineSaleTotalsForReportQuery::OfflineSaleTotalsForReportQuery(Db& db)
    : IssuerTotalsQuery(db, MapCountAndAmountResult,
                        MapByHostIndexAndIssuerParams) {
}

string OfflineSaleTotalsForReportQuery::GetTables() {
  return TX_TABLE;
}

//To get total of OFFLINE and PREAUTH COMPLETION OFFLINE transactions
// excluding:
// - voided transactions
// - Sale EMV approved offline
// - TIP_ADJUST offline
string OfflineSaleTotalsForReportQuery::GetSelection() {
  std::stringstream oss;
  oss << "host_index = ?"
      << "AND issuer_index = ?"
      << " AND (transaction_status = "
      << TransactionStatus::APPROVED
      << " OR transaction_status = "
      << TransactionStatus::TO_ADVISE
      << ")"
      << " AND ((transaction_type IN ("
      << TransactionType::OFFLINE_SALE
      << ","
      << TransactionType::PREAUTH_COMPLETION_OFFLINE << "))"
      << " AND is_voided = "
      << false << ")";

  return oss.str();
}

VoidSaleTotalsForReportQuery::VoidSaleTotalsForReportQuery(Db& db)
    : IssuerTotalsQuery(db, MapCountAndAmountWithTipResult,
                        MapByHostIndexAndIssuerParams) {
}

string VoidSaleTotalsForReportQuery::GetTables() {
  return TX_TABLE;
}

string VoidSaleTotalsForReportQuery::GetSelection() {
  std::stringstream oss;
  oss << "host_index = ?"
      << "AND issuer_index = ?"
      << " AND transaction_status = "
      << TransactionStatus::APPROVED
      << " AND ((transaction_type IN ("
      << TransactionType::SALE
      << "," << TransactionType::OFFLINE_SALE
      << ","
      << TransactionType::PREAUTH_COMPLETION_OFFLINE
      << ","
      << TransactionType::PREAUTH_COMPLETION_ONLINE
      << ","
      << TransactionType::CASH_ADVANCE
      << ","
      << TransactionType::INSTALMENT_SALE
      << "))"
      << " AND is_voided = "
      << true
      << ")";
  return oss.str();
}

VoidRefundTotalsForReportQuery::VoidRefundTotalsForReportQuery(Db& db)
    : IssuerTotalsQuery(db, MapCountAndAmountResult,
                        MapByHostIndexAndIssuerParams) {
}

string VoidRefundTotalsForReportQuery::GetTables() {
  return TX_TABLE;
}

string VoidRefundTotalsForReportQuery::GetSelection() {
  std::stringstream oss;
  oss << "host_index = ?"
      << "AND issuer_index = ?"
      << " AND transaction_status = "
      << TransactionStatus::APPROVED
      << " AND ( ( (transaction_type IN ("
      << TransactionType::REFUND
      << "))"
      << " AND (is_voided = "
      << true << ")))";
  return oss.str();
}

VoidTotalsQuery::VoidTotalsQuery(Db& db)
    : TotalsQuery(db, MapCountAndAmountResult, MapByHostIndexParams) {
}

string VoidTotalsQuery::GetTables() {
  return TX_TABLE;
}

string VoidTotalsQuery::GetSelection() {
  std::stringstream oss;
  oss << "transaction_type IN ("
      << TransactionType::SALE
      << "," << TransactionType::CASH_ADVANCE
      << ","
      << TransactionType::PREAUTH_COMPLETION_OFFLINE
      << ","
      << TransactionType::PREAUTH_COMPLETION_ONLINE
      << ","
      << TransactionType::OFFLINE_SALE
      << ","
      << TransactionType::INSTALMENT_SALE
      << ") AND "
      << "transaction_status = "
      << TransactionStatus::APPROVED
      << " AND " << "is_voided = " << true
      << " AND "
      << "host_index = ?";
  return oss.str();
}

RefundVoidTotalsQuery::RefundVoidTotalsQuery(Db& db)
    : TotalsQuery(db, MapCountAndAmountResult, MapByHostIndexParams) {
}

string RefundVoidTotalsQuery::GetTables() {
  return TX_TABLE;
}

string RefundVoidTotalsQuery::GetSelection() {
  std::stringstream oss;
  oss << "transaction_type = " << TransactionType::REFUND << " AND "
      << "transaction_status = "
      << TransactionStatus::APPROVED << " AND "
      << "is_voided = "
      << true << " AND " << "host_index = ?";
  return oss.str();
}

BatchCountQuery::BatchCountQuery(Db& db)
    : MappedQuery(db, MapCountOnly, MapByHostIndexParams) {
}

string BatchCountQuery::GetTables() {
  return TX_TABLE;
}

string BatchCountQuery::GetSelection() {
  std::stringstream oss;
  oss << "host_index = ? AND " << "(transaction_type != "
      << TransactionType::PREAUTH
      << ")OR " << "(transaction_type = "
      << TransactionType::PREAUTH
      << " AND " << "(is_preauth_completed = "
      << true
      << " OR " << "is_voided = " << true << "))";
  return oss.str();
}

ClearBatchStatement::ClearBatchStatement(Db& db)
    : MappedDelete<ByHostIndex>(db, TX_TABLE, MapByHostIndexParams) {
}

string ClearBatchStatement::GetSelection() {
  std::stringstream oss;
  oss << "host_index = ? AND " << "(transaction_type != "
      << TransactionType::PREAUTH
      << ") OR " << "(transaction_type = "
      << TransactionType::PREAUTH
      << " AND " << "(is_preauth_completed = "
      << true
      << " OR " << "is_voided = " << true << "))";
  return oss.str();
}

PurgeOldPreAuthStatement::PurgeOldPreAuthStatement(Db& db)
    : MappedDelete<ByHostIndexAndCutoff>(db, TX_TABLE,
                                         MapByHostIndexAndCutoffParams) {
}

string PurgeOldPreAuthStatement::GetSelection() {
  std::stringstream oss;
  oss << "host_index = ? AND " << "transaction_type = "
      << TransactionType::PREAUTH
      << " AND " << "is_preauth_completed = "
      << false
      << " AND " << "is_voided = " << false << " AND "
      << "transaction_status = "
      << TransactionStatus::APPROVED << " AND "
      << "tx_datetime <= ?";
  return oss.str();
}

PurgeAllPreAuthStatement::PurgeAllPreAuthStatement(Db& db)
    : MappedDelete<ByHostIndex>(db, TX_TABLE,
                                MapByHostIndexParams) {
}

string PurgeAllPreAuthStatement::GetSelection() {
  std::stringstream oss;
  oss << "host_index = ? AND " << "transaction_type = "
      << TransactionType::PREAUTH
      << " AND " << "is_preauth_completed = "
      << false
      << " AND " << "is_voided = " << false << " AND "
      << "transaction_status = "
      << TransactionStatus::APPROVED;
  return oss.str();
}

ClearReversalStatement::ClearReversalStatement(Db& db)
    : MappedDelete<ByHostIndex>(db, TX_TABLE, MapByHostIndexParams) {
}

string ClearReversalStatement::GetSelection() {
  std::stringstream oss;
  oss << "host_index = ? AND " << "transaction_status IN ("
      << TransactionStatus::TO_REVERSE
      << "," << TransactionStatus::IN_PROGRESS
      << ")";
  return oss.str();
}

IssuerSaleTotalsQuery::IssuerSaleTotalsQuery(Db& db)
    : IssuerTotalsQuery(db, MapCountAndAmountResult,
                        MapByHostIndexAndIssuerParams) {
}

string IssuerSaleTotalsQuery::GetTables() {
  return TX_TABLE;
}

string IssuerSaleTotalsQuery::GetSelection() {
  ostringstream oss;
  oss << "host_index = ? AND card_name = ? AND " << "(transaction_type IN ("
      << TransactionType::SALE
      << "," << TransactionType::CASH_ADVANCE
      << ","
      << TransactionType::OFFLINE_SALE
      << ","
      << TransactionType::PREAUTH_COMPLETION_OFFLINE
      << ","
      << TransactionType::PREAUTH_COMPLETION_ONLINE
      << ","
      << TransactionType::INSTALMENT_SALE
      << ") AND "
      << "  (transaction_status = " << TransactionStatus::APPROVED
      << "  OR transaction_status = "
      << TransactionStatus::TO_ADVISE
      << ")) AND "
      << "is_voided = "
      << false;
  return oss.str();
}

IssuerVoidTotalsQuery::IssuerVoidTotalsQuery(Db& db)
    : IssuerTotalsQuery(db, MapCountAndAmountResult,
                        MapByHostIndexAndIssuerParams) {
}

string IssuerVoidTotalsQuery::GetTables() {
  return TX_TABLE;
}

string IssuerVoidTotalsQuery::GetSelection() {
  ostringstream oss;
  oss << "transaction_type IN (" << TransactionType::SALE << ","
      << TransactionType::CASH_ADVANCE
      << ","
      << TransactionType::PREAUTH_COMPLETION_OFFLINE
      << ","
      << TransactionType::PREAUTH_COMPLETION_ONLINE
      << ","
      << TransactionType::OFFLINE_SALE
      << ","
      << TransactionType::INSTALMENT_SALE
      << ") AND " << "transaction_status = "
      << TransactionStatus::APPROVED
      << " AND " << "is_voided = " << true
      << " AND "
      << "host_index = ? AND card_name = ?";
  return oss.str();
}

IssuerRefundTotalsQuery::IssuerRefundTotalsQuery(Db& db)
    : IssuerTotalsQuery(db, MapCountAndAmountResult,
                        MapByHostIndexAndIssuerParams) {
}

string IssuerRefundTotalsQuery::GetTables() {
  return TX_TABLE;
}

string IssuerRefundTotalsQuery::GetSelection() {
  std::stringstream oss;
  oss << "transaction_type = " << TransactionType::REFUND << " AND "
      << "transaction_status = "
      << TransactionStatus::APPROVED << " AND "
      << "is_voided = "
      << false << " AND " << "host_index = ? AND card_name = ?";
  return oss.str();
}

IssuerRefundVoidTotalsQuery::IssuerRefundVoidTotalsQuery(Db& db)
    : IssuerTotalsQuery(db, MapCountAndAmountResult,
                        MapByHostIndexAndIssuerParams) {
}

string IssuerRefundVoidTotalsQuery::GetTables() {
  return TX_TABLE;
}

string IssuerRefundVoidTotalsQuery::GetSelection() {
  std::stringstream oss;
  oss << "transaction_type = " << TransactionType::REFUND << " AND "
      << "transaction_status = "
      << TransactionStatus::APPROVED << " AND "
      << "is_voided = "
      << true << " AND " << "host_index = ? AND card_name = ?";
  return oss.str();
}

TxBatchUpload::TxBatchUpload(Db& db)
    : TxQuery<ByHostIndex>(db, MapTxTable, MapByHostIndexParams) {
}

string TxBatchUpload::GetTables() {
  return TX_TABLE;
}

string TxBatchUpload::GetSelection() {
  ostringstream oss;
  oss << "(host_index = ?)"
      << " AND (transaction_status = "
      << TransactionStatus::APPROVED << ")"
      << " AND (is_voided = "
      << false << ")"
      << " AND (transaction_type IN ("
      << TransactionType::SALE << ","
      << TransactionType::CASH_ADVANCE
      << ","
      << TransactionType::REFUND
      << ","
      << TransactionType::OFFLINE_SALE
      << ","
      << TransactionType::PREAUTH_COMPLETION_OFFLINE
      << ","
      << TransactionType::PREAUTH_COMPLETION_ONLINE
      << ","
      << TransactionType::INSTALMENT_SALE
      << "))";

  return oss.str();
}

BatchStatusQuery::BatchStatusQuery(Db& db)
    : MappedQuery<CountOnly, ByBatchStatus>(db, MapCountOnly,
                                            MapByBatchStatusParams) {
}

string BatchStatusQuery::GetTables() {
  return BATCH_INFO_TABLE;
}

string BatchStatusQuery::GetSelection() {
  return "batch_status = ?";
}

BatchInfoByHostIndexQuery::BatchInfoByHostIndexQuery(Db& db)
    : BatchInfoQuery(db, MapBatchInfoTable, MapByHostIndexParams) {
}

string BatchInfoByHostIndexQuery::GetTables() {
  return BATCH_INFO_TABLE;
}

string BatchInfoByHostIndexQuery::GetSelection() {
  return "host_index = ?";
}

TxBatchDetail::TxBatchDetail(Db& db)
    : TxQuery<ByHostIndex>(db, MapTxTable, MapByHostIndexParams) {
}

string TxBatchDetail::GetTables() {
  return TX_TABLE;
}

string TxBatchDetail::GetSelection() {
  ostringstream oss;
  oss << "(host_index = ?)"
      << " AND (transaction_status = "
      << TransactionStatus::APPROVED << " OR transaction_status = "
      << TransactionStatus::TO_ADVISE
      << ")"
      << " AND (transaction_type IN ("
      << TransactionType::SALE << ","
      << TransactionType::CASH_ADVANCE
      << ","
      << TransactionType::REFUND
      << ","
      << TransactionType::OFFLINE_SALE
      << ","
      << TransactionType::PREAUTH_COMPLETION_OFFLINE
      << ","
      << TransactionType::PREAUTH_COMPLETION_ONLINE
      << ","
      << TransactionType::INSTALMENT_SALE
      << "))";

  return oss.str();
}

}

