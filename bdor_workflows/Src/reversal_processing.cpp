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
#include "reversal_processing.h"

namespace bdor {

namespace {

bool ProcessReversal(Batch& batch, HostSwitch& host, Transaction* tx) {

  HostSwitch::Status status = host.SendReversal(*tx);
  if (status != HostSwitch::Status::COMPLETED) {
    host.Disconnect();  // we disconnect since we shouldn't continue any processing if reversal fails
    return false;
  }
  // Reversal successful
  if (tx->in_progress_status == InProgressStatus::IN_PROGRESS_VOID) {
    tx->SetTransactionStatus(TransactionStatus::APPROVED);
    tx->is_voided = false;
    tx->stan = tx->stan_original;
    batch.InsertOrUpdate(*tx);
  } else {
    batch.Delete(*tx);
  }
  return true;
}
}

bool ProcessPendingReversals(Batch& batch,
                             HostSwitch& host,
                             unsigned int host_index) {

  std::vector<Transaction> reversals = batch.GetReversal(host_index);

  bool ret = true;
  for (auto& reversal : reversals) {
    ret = ProcessReversal(batch, host, &reversal);
    if (ret == false) {
      break;
    }
  }

  return ret;
}

}
