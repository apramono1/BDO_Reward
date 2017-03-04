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
#ifndef BDOR__BATCH_INFO_H_
#define BDOR__BATCH_INFO_H_

namespace bdor {

enum BatchStatus {
  BATCH_CLOSED,
  BATCH_OPEN,
  BATCH_SETTLE_IN_PROGRESS
};

struct BatchInfo {
  BatchInfo()
      : rowid(0),
        current_batch_num(0),
        host_index(0),
        batch_status(BATCH_CLOSED),
        opening_datetime(0) {
  }

  int64_t rowid;
  uint64_t current_batch_num;
  unsigned int host_index;
  BatchStatus batch_status;
  time_t opening_datetime;
};

}

#endif
