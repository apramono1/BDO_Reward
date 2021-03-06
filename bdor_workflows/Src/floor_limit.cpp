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
#include "floor_limit.h"

namespace bdor {

bool IsFloorLimitExceeded(std::uint64_t floor_limit, Transaction* tx) {
  if (tx->amount->GetValue() < floor_limit) {
    tx->online_authorization_required = false;
    tx->pin_entered = false;
    tx->transaction_status = TransactionStatus::TO_ADVISE;
    tx->auth_id_response = {};
    return false;
  }
  return true;
}

}
