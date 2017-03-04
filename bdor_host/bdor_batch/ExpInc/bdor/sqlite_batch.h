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

#ifndef BDOR__SQLITE_BATCH_H_
#define BDOR__SQLITE_BATCH_H_

#include <bdor/batch.h>
#include <bdor/application_settings.h>
#include <bdor/sqlite_batch_mapper.h>

namespace bdor {

class SqliteBatch : public Batch {
 public:
  SqliteBatch(ApplicationSettings& app_settings, const char* path,
              bool migrate_batch);
  SqliteBatch(ApplicationSettings& app_settings, const std::string& path,
              bool migrate_batch)
      : SqliteBatch(app_settings, path.c_str(), migrate_batch) {
  }

  // BATCH LIFECYCLE
  virtual bool OpenBatch(unsigned int host_index) override;
  virtual bool LockBatch(unsigned int host_index) override;
  virtual bool CloseBatch(unsigned int host_index) override;

  // BATCH INFO
  virtual BatchStatus GetBatchStatus(unsigned int host_index) override;
  virtual BatchInfo GetBatchInfo(unsigned int host_index) override;
  virtual int GetCurrentBatchNumber(unsigned int host_index) override;
  virtual bool IsBatchOpen(unsigned int host_index) override;
  virtual bool IsBatchLocked(unsigned int host_index) override;
  virtual bool IsBatchClosed(unsigned int host_index) override;
  virtual bool IsAnyBatchLocked() override;
  virtual bool IsAnyBatchOpen() override;

  // TRANSACTION MANAGEMENT
  virtual bool Insert(Transaction& txn) override;
  virtual bool Update(Transaction& txn) override;
  virtual bool InsertOrUpdate(Transaction& txn) override;
  virtual bool Delete(Transaction& txn) override;

  // BASIC QUERIES
  virtual Transactions GetAllTransactions(unsigned int host_index) override;  // returns every transactions for that host
  // virtual Transactions GetAllApprovedTransactions(unsigned int host_index) override;  // return approved transaction. Offline Transaction in TO_ADVISE state are also returned
  virtual Transactions FindTransactionByInvoice(unsigned int invoice_num)
      override;
  virtual Transactions FindTransactionByStan(unsigned int stan) override;
  virtual Transactions FindTransactionByPan(const types::Pan& pan) override;
  virtual Transactions FindTransactionByAmount(const types::Amount& amount) override;

  // ADVICES QUERIES
  virtual Transactions GetPendingAdvices(unsigned int host_index) override;
  virtual Transactions GetPendingTcUpload(unsigned int host_index) override;

  // PREAUTH
  virtual Transactions GetPendingPreAuth(unsigned int host_index) override;
  virtual bool IsPreAuthPending(unsigned int host_index) override;
  virtual bool PurgeOldPreAuth(unsigned int host_index) override;
  virtual bool PurgeAllPreAuth(unsigned int host_index) override;

  // REVERSALS
  virtual bool IsReversalPending(unsigned int host_index) override;
  virtual Transactions GetReversal(unsigned int host_index) override;

  // ADMIN
  virtual bool ClearReversal(unsigned int host_index) override;
  virtual bool ClearBatch(unsigned int host_index) override;

  // SETTLEMENT
  virtual bool IsSettlementNeeded(unsigned int host_index) override;
  virtual BatchTotalsForHost GetBatchTotalsForHost(unsigned int host_index) override;
  virtual Transactions GetBatchUploadTransactions(unsigned int host_index)
      override;
  virtual BatchTotalsReport GetBatchTotalsForReport(unsigned int host_index) override;
  virtual AllHostBatchTotalsReport GetAllHostsBatchTotalsForReport() override;
  virtual DetailReportSingleHost GetBatchDetailTransactions(unsigned int host_index) override;

 private:
  ApplicationSettings& app_settings_;
  sqlite::Db db_;

  TxTable tx_table_;
  BatchInfoTable batch_info_table_;

  sqlite::Holder<TxByInvoiceQuery> GetTxByInvoiceQuery;
  sqlite::Holder<TxByStanQuery> GetTxByStanQuery;
  sqlite::Holder<TxByPanQuery> GetTxByPanQuery;
  sqlite::Holder<TxByAmountQuery> GetTxByAmountQuery;
  sqlite::Holder<TxPendingAdviceQuery> GetTxPendingAdviceQuery;
  sqlite::Holder<TxPendingPreAuthQuery> GetTxPendingPreAuthQuery;
  sqlite::Holder<TxTcUploadQuery> GetTxTcUploadQuery;
  sqlite::Holder<TxReversalQuery> GetTxReversalQuery;
  sqlite::Holder<TxAllQuery> GetTxAllQuery;
  sqlite::Holder<TxSuccessQuery> GetTxSuccessQuery;

  sqlite::Holder<SaleTotalsForHostQuery> GetSaleTotalsForHostQuery;
  sqlite::Holder<RefundTotalsForHostQuery> GetRefundTotalsForHostQuery;

  sqlite::Holder<UniqueIssuerIndexQuery> GetUniqueIssuerIndexQuery;

  sqlite::Holder<SaleTotalsForReportQuery> GetSaleTotalsForReportQuery;
  sqlite::Holder<TipTotalsForReportQuery> GetTipTotalsForReportQuery;
  sqlite::Holder<RefundTotalsForReportQuery> GetRefundTotalsForReportQuery;
  sqlite::Holder<OfflineSaleTotalsForReportQuery> GetOfflineSaleTotalsForReportQuery;
  sqlite::Holder<VoidSaleTotalsForReportQuery> GetVoidSaleTotalsForReportQuery;
  sqlite::Holder<VoidRefundTotalsForReportQuery> GetVoidRefundTotalsForReportQuery;

  sqlite::Holder<VoidTotalsQuery> GetVoidTotalsQuery;
  sqlite::Holder<RefundVoidTotalsQuery> GetRefundVoidTotalsQuery;
  sqlite::Holder<BatchCountQuery> GetBatchCountQuery;
  sqlite::Holder<ClearBatchStatement> GetClearBatchStatement;
  sqlite::Holder<PurgeOldPreAuthStatement> GetPurgeOldPreAuthStatement;
  sqlite::Holder<PurgeAllPreAuthStatement> GetPurgeAllPreAuthStatement;
  sqlite::Holder<ClearReversalStatement> GetClearReversalStatement;
  sqlite::Holder<IssuerSaleTotalsQuery> GetIssuerSaleTotalsQuery;
  sqlite::Holder<IssuerVoidTotalsQuery> GetIssuerVoidTotalsQuery;
  sqlite::Holder<IssuerRefundTotalsQuery> GetIssuerRefundTotalsQuery;
  sqlite::Holder<IssuerRefundVoidTotalsQuery> GetIssuerRefundVoidTotalsQuery;
  sqlite::Holder<TxBatchUpload> GetTxBatchUpload;
  sqlite::Holder<BatchStatusQuery> GetBatchStatusQuery;
  sqlite::Holder<BatchInfoByHostIndexQuery> GetBatchInfoByHostIndexQuery;
  sqlite::Holder<TxBatchDetail> GetTxBatchDetail;

  bool SetBatchStatus(unsigned int host_index, BatchStatus batch_status);
};

}
#endif
