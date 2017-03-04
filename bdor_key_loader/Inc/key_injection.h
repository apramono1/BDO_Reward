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

#ifndef ATOS_KEY_LOADER_KEY_INJECTION_H_
#define ATOS_KEY_LOADER_KEY_INJECTION_H_

extern "C" {
#include <SEC_interface.h>
#include <tlvVar_def.h>
}

#include <tpcore/goal_handle.h>

class KeyInjection {
 public:
  KeyInjection(tpcore::GoalHandle& gl)
      : gl_(gl) {
  }

  void InjectTmk();
  void ClearSecretArea();

 private:
  tpcore::GoalHandle& gl_;

  static const int kSecureAreaId;
  static const uint kBankId;
  static const ushort kTmkKeyNumber;
  static const int kCardId;
  static const int kVarId;
  static const T_SEC_PARTTYPE kSecureType;

  bool IsSecretAreaCreated();
  bool CreateSecretArea();

  void DisplaySuccess();
  void DisplayError();
  void DisplayMessage(const char* message);
};

#endif
