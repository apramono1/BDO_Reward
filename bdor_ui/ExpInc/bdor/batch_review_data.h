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
#ifndef BDOR__BATCH_REVIEW_DATA_H_
#define BDOR__BATCH_REVIEW_DATA_H_

#include <bdor/ui.h>
#include <types/amount.h>
#include <stdx/string>

namespace bdor {

struct TxReviewDetail {
  std::string tx_name;
  std::string card_name;
  std::string masked_pan;
  std::string masked_expiry_date;
  types::Amount amount;
  std::string auth_code;
  std::uint64_t stan;
};
}
#endif
