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
#include <bdor/key_exchange_workflow.h>

namespace bdor {

KeyExchangeWorkflow::KeyExchangeWorkflow(ApplicationSettings& app_setting,
                                         HostSwitch& host,
                                         Ui& ui)
    : app_setting_(app_setting),
      host_(host),
      ui_(ui) {
}

void KeyExchangeWorkflow::Start() {
  return ui_.MessageLogonSuccessful();
}

void KeyExchangeWorkflow::GetKeyExchangeInfo() {
}

bool KeyExchangeWorkflow::SetUpWorkingKeys() {
  return false;
}

void KeyExchangeWorkflow::UpdateKeyExchangeStatus(const bool status) {
}

}
