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
#include "ecr_manager.h"
#include <utils/logger.h>

namespace bdor {

using namespace posgate;

posgate::Message EcrManager::ProcessSettlement() {

  int initial_header = IsHeaderDisplayed();
  DisplayHeader(OFF);

  //TODO: consider to return the card total and grand total if required
  bool result = acquirer_.StartSettlement(true);

  if (initial_header == 1)
    DisplayHeader(ON);

  return GetSettlementResponse(result);
}

}
