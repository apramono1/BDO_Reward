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

#ifndef BDOR__SQLITE_BATCH_MAPPER_H_
#define BDOR__SQLITE_BATCH_MAPPER_H_

#include <sqlite/mapper.h>
#include <bdor/batch.h>

namespace bdor {

struct TxTable : public sqlite::MappedTable<Transaction> {
  TxTable(sqlite::Db& db);
};

struct BatchInfoTable : public sqlite::MappedTable<BatchInfo> {
  BatchInfoTable(sqlite::Db& db);
};

struct ByHostIndex {
  unsigned int host_index;
};

struct ByInvoice {
  uint32_t invoice;
};

struct ByStan {
  uint32_t stan;
};

struct ByPan {
  ByPan(const types::Pan& pan_)
  : pan(pan_) {

  }
  ByPan()
  : pan({'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '1', '2'}) {  // dummy construction
  }
  types::Pan pan;
};

struct ByAmount {
  uint64_t amount;
};

struct ByHostIndexAndIssuer {
  unsigned int host_index;
  unsigned int issuer;
};

struct ByHostIndexAndCutoff {
  unsigned int host_index;
  time_t cutoff;
};

struct CountAndAmount {
  int count;
  uint64_t amount;
};

struct CountOnly {
  int count;
};

struct IssuerIndex {
  unsigned int issuer_index;
};

struct ByBatchStatus {
  BatchStatus batch_status;
};

template<typename PARAMS>
using TxQuery = sqlite::MappedQuery<Transaction, PARAMS>;
using TotalsQuery = sqlite::MappedQuery<CountAndAmount, ByHostIndex>;
using IssuerTotalsQuery = sqlite::MappedQuery<CountAndAmount, ByHostIndexAndIssuer>;
using BatchInfoQuery = sqlite::MappedQuery<BatchInfo, ByHostIndex>;

struct TxAllQuery : TxQuery<ByHostIndex> {
  TxAllQuery(sqlite::Db& db);
  std::string GetTables();
  std::string GetSelection();
  std::string GetSortOrder();
};

struct TxSuccessQuery : TxQuery<ByHostIndex> {
  TxSuccessQuery(sqlite::Db& db);
  std::string GetTables();
  std::string GetSelection();
  std::string GetSortOrder();
};

struct TxByInvoiceQuery : TxQuery<ByInvoice> {
  TxByInvoiceQuery(sqlite::Db& db);
  std::string GetTables();
  std::string GetSelection();
};

struct TxByStanQuery : TxQuery<ByStan> {
  TxByStanQuery(sqlite::Db& db);
  std::string GetTables();
  std::string GetSelection();
};

struct TxByPanQuery : TxQuery<ByPan> {
  TxByPanQuery(sqlite::Db& db);
  std::string GetTables();
  std::string GetSelection();
};

struct TxByAmountQuery : TxQuery<ByAmount> {
  TxByAmountQuery(sqlite::Db& db);
  std::string GetTables();
  std::string GetSelection();
};

struct TxPendingAdviceQuery : TxQuery<ByHostIndex> {
  TxPendingAdviceQuery(sqlite::Db& db);
  std::string GetTables();
  std::string GetSelection();
  std::string GetSortOrder();
};

struct TxTcUploadQuery : TxQuery<ByHostIndex> {
  TxTcUploadQuery(sqlite::Db& db);
  std::string GetTables();
  std::string GetSelection();
  std::string GetSortOrder();
};

struct TxReversalQuery : TxQuery<ByHostIndex> {
  TxReversalQuery(sqlite::Db& db);
  std::string GetTables();
  std::string GetSelection();
  std::string GetSortOrder();
};

struct TxPendingPreAuthQuery : TxQuery<ByHostIndex> {
  TxPendingPreAuthQuery(sqlite::Db& db);
  std::string GetTables();
  std::string GetSelection();
  std::string GetSortOrder();
};

// QUERIES FOR HOST SETTLEMENT
struct SaleTotalsForHostQuery : TotalsQuery {
  SaleTotalsForHostQuery(sqlite::Db& db);
  std::string GetTables();
  std::string GetSelection();
};

struct RefundTotalsForHostQuery : TotalsQuery {
  RefundTotalsForHostQuery(sqlite::Db& db);
  std::string GetTables();
  std::string GetSelection();
};

struct UniqueIssuerIndexQuery : sqlite::MappedQuery<IssuerIndex, ByHostIndex> {
  UniqueIssuerIndexQuery(sqlite::Db& db);
  std::string GetTables();
  std::string GetSelection();
};

// QUERiES FOR SETTELMENT REPORT
struct SaleTotalsForReportQuery : IssuerTotalsQuery {
  SaleTotalsForReportQuery(sqlite::Db& db);
  std::string GetTables();
  std::string GetSelection();
};

struct TipTotalsForReportQuery : IssuerTotalsQuery {
  TipTotalsForReportQuery(sqlite::Db& db);
  std::string GetTables();
  std::string GetSelection();
};

struct RefundTotalsForReportQuery : IssuerTotalsQuery {
  RefundTotalsForReportQuery(sqlite::Db& db);
  std::string GetTables();
  std::string GetSelection();
};

struct OfflineSaleTotalsForReportQuery : IssuerTotalsQuery {
  OfflineSaleTotalsForReportQuery(sqlite::Db& db);
  std::string GetTables();
  std::string GetSelection();
};

struct VoidSaleTotalsForReportQuery : IssuerTotalsQuery {
  VoidSaleTotalsForReportQuery(sqlite::Db& db);
  std::string GetTables();
  std::string GetSelection();
};

struct VoidRefundTotalsForReportQuery : IssuerTotalsQuery {
  VoidRefundTotalsForReportQuery(sqlite::Db& db);
  std::string GetTables();
  std::string GetSelection();
};

struct VoidTotalsQuery : TotalsQuery {
  VoidTotalsQuery(sqlite::Db& db);
  std::string GetTables();
  std::string GetSelection();
};

struct RefundVoidTotalsQuery : TotalsQuery {
  RefundVoidTotalsQuery(sqlite::Db& db);
  std::string GetTables();
  std::string GetSelection();
};

struct BatchCountQuery : sqlite::MappedQuery<CountOnly, ByHostIndex> {
  BatchCountQuery(sqlite::Db& db);
  std::string GetTables();
  std::string GetSelection();
};

struct ClearBatchStatement : sqlite::MappedDelete<ByHostIndex> {
  ClearBatchStatement(sqlite::Db& db);
  std::string GetSelection();
};

struct PurgeOldPreAuthStatement : sqlite::MappedDelete<ByHostIndexAndCutoff> {
  PurgeOldPreAuthStatement(sqlite::Db& db);
  std::string GetSelection();
};

struct PurgeAllPreAuthStatement : sqlite::MappedDelete<ByHostIndex> {
  PurgeAllPreAuthStatement(sqlite::Db& db);
  std::string GetSelection();
};

struct ClearReversalStatement : sqlite::MappedDelete<ByHostIndex> {
  ClearReversalStatement(sqlite::Db& db);
  std::string GetSelection();
};

struct IssuerSaleTotalsQuery : IssuerTotalsQuery {
  IssuerSaleTotalsQuery(sqlite::Db& db);
  std::string GetTables();
  std::string GetSelection();
};

struct IssuerVoidTotalsQuery : IssuerTotalsQuery {
  IssuerVoidTotalsQuery(sqlite::Db& db);
  std::string GetTables();
  std::string GetSelection();
};

struct IssuerRefundTotalsQuery : IssuerTotalsQuery {
  IssuerRefundTotalsQuery(sqlite::Db& db);
  std::string GetTables();
  std::string GetSelection();
};

struct IssuerRefundVoidTotalsQuery : IssuerTotalsQuery {
  IssuerRefundVoidTotalsQuery(sqlite::Db& db);
  std::string GetTables();
  std::string GetSelection();
};

struct TxBatchUpload : TxQuery<ByHostIndex> {
  TxBatchUpload(sqlite::Db& db);
  std::string GetTables();
  std::string GetSelection();
};

struct BatchStatusQuery : sqlite::MappedQuery<CountOnly, ByBatchStatus> {
  BatchStatusQuery(sqlite::Db& db);
  std::string GetTables();
  std::string GetSelection();
};

struct BatchInfoByHostIndexQuery : BatchInfoQuery {
  BatchInfoByHostIndexQuery(sqlite::Db& db);
  std::string GetTables();
  std::string GetSelection();
};


struct TxBatchDetail : TxQuery<ByHostIndex> {
	TxBatchDetail(sqlite::Db& db);
  std::string GetTables();
  std::string GetSelection();
};

}
#endif
