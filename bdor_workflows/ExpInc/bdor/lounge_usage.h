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
#ifndef BDOR__LOUNGE_USAGE_H_
#define BDOR__LOUNGE_USAGE_H_

#include <memory>
#include <bdor/ui.h>
#include <bdor/receipts.h>
#include <bdor/password.h>
#include <types/password_access_level.h>
#include "acquirer.h"

namespace bdor {

class LoungeUsage{
public:

  LoungeUsage();
  bool OpenLoungeUsageMenu(Transaction& tx);
  bool ProcessValidCard(Transaction& tx);
  void ProceedToExcessVisit(Transaction& tx);
  void ProceedToWithinLimitVisit(Transaction& tx);

private:
  MenuDesc selection_menu1_;
  MenuDesc selection_menu2_;
  MenuDesc guest_number_menu_;
  MenuDesc payment_option_menu_;

};

}

#endif

