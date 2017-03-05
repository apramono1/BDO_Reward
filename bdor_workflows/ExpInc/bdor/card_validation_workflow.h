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
#ifndef BDOR__CARD_VALIDATION_WORKFLOW_H_
#define BDOR__CARD_VALIDATION_WORKFLOW_H_

#include <cstdio>
#include <types/magstripe.h>
#include <tpcore/pin_entry.h>
#include <tpcore/telium_event.h>
#include <bdor/application_settings.h>
#include <bdor/transaction.h>
#include <bdor/ui.h>
#include <bdor/host_switch.h>

namespace bdor {

class CardValidationWorkflow {

 public:
  CardValidationWorkflow(ApplicationSettings& app_settings, HostSwitch& host, tpcore::PinEntry& pin_entry, Ui& ui);
  void Start();
  bool RunManualFlow(Transaction& tx);
  bool RunMagFlow(types::MagStripe mag_stripe, Transaction& tx);

 private:
  ApplicationSettings& app_settings_;
  HostSwitch& host_;
  tpcore::PinEntry& pin_entry_;
  Ui& ui_;
};

}

#endif
