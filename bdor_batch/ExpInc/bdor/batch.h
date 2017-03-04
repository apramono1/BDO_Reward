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
#ifndef BDOR__BATCH_H_
#define BDOR__BATCH_H_

#include <bdor/transaction.h>
#include <bdor/batch_info.h>
#include <bdor/batch_data_types.h>

namespace bdor {

class Batch {
 public:
  virtual ~Batch() {
  }

  // BATCH LIFECYCLE
  virtual bool OpenBatch(unsigned int host_index) = 0;
  virtual bool LockBatch(unsigned int host_index) = 0;
  virtual bool CloseBatch(unsigned int host_index) = 0;

  // BATCH INFO
  virtual BatchStatus GetBatchStatus(unsigned int host_index) = 0;
  virtual BatchInfo GetBatchInfo(unsigned int host_index) = 0;
  virtual int GetCurrentBatchNumber(unsigned int host_index) = 0;
  virtual bool IsBatchOpen(unsigned int host_index) = 0;
  virtual bool IsBatchLocked(unsigned int host_index) = 0;
  virtual bool IsBatchClosed(unsigned int host_index) = 0;
  virtual bool IsAnyBatchLocked() = 0;
  virtual bool IsAnyBatchOpen() = 0;

  // TRANSACTION MANAGEMENT
  virtual bool Insert(Transaction& txn) = 0;
  virtual bool Update(Transaction& txn) = 0;
  virtual bool InsertOrUpdate(Transaction& txn) = 0;
  virtual bool Delete(Transaction& txn) = 0;

  // BASIC QUERIES
  virtual Transactions GetAllTransactions(unsigned int host_index) = 0;  // returns every transactions for that host
  // virtual Transactions GetAllApprovedTransactions(unsigned int host_index) = 0;  // return approved transaction. Offline Transaction in TO_ADVISE state are also returned
  virtual Transactions FindTransactionByInvoice(unsigned int invoice_num) = 0;
  virtual Transactions FindTransactionByStan(unsigned int stan) = 0;
  virtual Transactions FindTransactionByPan(const types::Pan& pan) = 0;
  virtual Transactions FindTransactionByAmount(const types::Amount& amount) = 0;

  // ADVICES QUERIES
  virtual Transactions GetPendingAdvices(unsigned int host_index) = 0;
  virtual Transactions GetPendingTcUpload(unsigned int host_index) = 0;

  // PREAUTH
  virtual Transactions GetPendingPreAuth(unsigned int host_index) = 0;
  virtual bool IsPreAuthPending(unsigned int host_index) = 0;
  virtual bool PurgeOldPreAuth(unsigned int host_index) = 0;
  virtual bool PurgeAllPreAuth(unsigned int host_index) = 0;

  // REVERSALS
  virtual bool IsReversalPending(unsigned int host_index) = 0;
  virtual Transactions GetReversal(unsigned int host_index) = 0;

  // ADMIN
  virtual bool ClearReversal(unsigned int host_index) = 0;
  virtual bool ClearBatch(unsigned int host_index) = 0;

  // SETTLEMENT
  virtual bool IsSettlementNeeded(unsigned int host_index) = 0;
  virtual BatchTotalsForHost GetBatchTotalsForHost(unsigned int host_index) = 0;
  virtual Transactions GetBatchUploadTransactions(unsigned int host_index) = 0;
  virtual BatchTotalsReport GetBatchTotalsForReport(
      unsigned int host_index) = 0;

  virtual AllHostBatchTotalsReport GetAllHostsBatchTotalsForReport() = 0;
  virtual DetailReportSingleHost GetBatchDetailTransactions(unsigned int host_index) = 0;
};

}

#endif
