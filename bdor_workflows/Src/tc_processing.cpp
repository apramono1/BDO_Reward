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

bool ProcessSingleTc(Batch& batch,
                     HostSwitch& host,
                     Transaction* tx) {
  bool result = false;
  HostSwitch::Status status = HostSwitch::Status::PERM_FAILURE;

  status = host.PerformTcUpload(*tx);

  if (status == HostSwitch::Status::COMPLETED) {
    tx->SetTcUploadStatus(TcUploadStatus::TC_UPLOAD_COMPLETED);
    batch.InsertOrUpdate(*tx);
    result = true;
  }

  return result;
}

}

bool ProcessAllPendingTcs(Batch& batch,
                          HostSwitch& host,
                          unsigned int host_index) {

  auto tcs = batch.GetPendingTcUpload(host_index);
  bool result = true;

  for (auto& tc : tcs) {
    result = ProcessSingleTc(batch, host, &tc);
    if (!result) {
      break;
    }
  }

  return result;
}

bool ProcessCurrentTc(Batch& batch,
                      HostSwitch& host,
                      unsigned int host_index) {
  auto tcs = batch.GetPendingTcUpload(host_index);
  if (tcs.empty()) {
    return true;
  }

  return ProcessSingleTc(batch, host, &tcs[tcs.size() - 1]);
}

}
