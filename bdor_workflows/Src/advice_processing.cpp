/*
 ------------------------------------------------------------------------------
 INGENICO Technical Software Department
 ------------------------------------------------------------------------------
 Copyright (c) 2016 INGENICO S.A.
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
#include "advice_processing.h"

namespace bdor {

namespace {

bool ProcessSingleAdvice(Batch& batch,
                         HostSwitch& host,
                         Transaction* tx) {
  bool result = false;
  HostSwitch::Status status = HostSwitch::Status::PERM_FAILURE;

  if (tx->is_adjusted) {
    status = host.PerformTipAdjust(*tx);

  } else if (tx->transaction_type == TransactionType::SALE ||
      tx->transaction_type == TransactionType::OFFLINE_SALE ||
      tx->transaction_type == TransactionType::PREAUTH_COMPLETION_OFFLINE) {

    status = host.PerformOfflineSale(*tx);
  }

  if (status == HostSwitch::Status::COMPLETED) {
    tx->SetTransactionStatus(TransactionStatus::APPROVED);
    batch.InsertOrUpdate(*tx);
    result = true;
  }

  return result;
}

}

bool ProcessAllPendingAdvices(Batch& batch,
                              HostSwitch& host,
                              unsigned int host_index) {

  auto advices = batch.GetPendingAdvices(host_index);
  bool result = true;

  for (auto& advice : advices) {
    result = ProcessSingleAdvice(batch, host, &advice);
    if (!result) {
      break;
    }
  }

  return result;
}

bool ProcessNextPendingAdvice(Batch& batch,
                              HostSwitch& host,
                              unsigned int host_index) {
  auto advices = batch.GetPendingAdvices(host_index);
  if (advices.empty()) {
    return true;
  }

  return ProcessSingleAdvice(batch, host, &advices[0]);
}

}
