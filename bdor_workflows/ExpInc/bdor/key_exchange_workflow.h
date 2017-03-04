/*
 ------------------------------------------------------------------------------
 INGENICO Technical Software Department
 ------------------------------------------------------------------------------
 Copyright (c) 2015 INGENICO S.A.
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
#ifndef BDOR__KEY_EXCHANGE_WORKFLOW_H_
#define BDOR__KEY_EXCHANGE_WORKFLOW_H_

#include <bdor/application_settings.h>
#include <bdor/ui.h>
#include <bdor/host_switch.h>

namespace bdor {

class KeyExchangeWorkflow {
 public:
  KeyExchangeWorkflow(ApplicationSettings& app_setting,
                      HostSwitch& host,
                      Ui& ui);

  void Start();

 private:
  ApplicationSettings& app_setting_;
  HostSwitch& host_;
  Ui& ui_;

  KeyExchange exchange_info_;

  void GetKeyExchangeInfo();
  bool SetUpWorkingKeys();
  void UpdateKeyExchangeStatus(const bool status);
};

}

#endif
