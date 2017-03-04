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

#include <cstdio>
#include <vector>
#include <sdk.h>
#include <string>
#include <utils/strings.h>

#include "key_injection.h"

#define __OEM_VFS_DEF_H_INCLUDED__

const unsigned int kKcvLength = 3;
const int KeyInjection::kSecureAreaId = 0x01182130;
const uint KeyInjection::kBankId = 0x80000000;
const ushort KeyInjection::kTmkKeyNumber = 0x3F0;
const int KeyInjection::kCardId = 0x10000683;
const int KeyInjection::kVarId = 0x0118;
const T_SEC_PARTTYPE KeyInjection::kSecureType = C_SEC_CIPHERING;

void KeyInjection::InjectTmk() {
  DisplayHeader(OFF);

  if (!IsSecretAreaCreated())
    if (!CreateSecretArea())
      return;

  T_SEC_DATAKEY_ID key_id;
  key_id.iSecretArea = kSecureAreaId;
  key_id.cAlgoType = TLV_TYPE_KTDES;
  key_id.usNumber = kTmkKeyNumber/4;
  key_id.uiBankId = kBankId;

  unsigned char key_value[] = { 0x0E, 0x01, 0x9B, 0xA4, 0xD0, 0xDA, 0xBA, 0xBC,
      0x51, 0x9D, 0xE6, 0x0D, 0x1F, 0x13, 0x46, 0x79 };
  unsigned char expected_kcv[] = { 0x26, 0xF6, 0x28 };

  GL_Dialog_Message(gl_.GetHandle(), "Key Injection", "Loading Key",
                    GL_ICON_INFORMATION, GL_BUTTON_NONE, 0);
  int ret = SEC_LoadKey(kSecureType, nullptr, &key_id, key_value,
                        CIPHERING_KEY);

  if (ret == OK) {
    unsigned char kcv_result[kKcvLength];
    unsigned int kcv_length = kKcvLength;
    ret = SEC_KeyVerify(kSecureType, &key_id, kcv_result, &kcv_length);

    if (memcpy(kcv_result, expected_kcv, kKcvLength) == 0) {
      DisplaySuccess();
    } else {
      DisplayError();
    }
  } else
    DisplayError();

  DisplayHeader(ON);
}

void KeyInjection::ClearSecretArea() {
  DisplayHeader(OFF);

  if (!IsSecretAreaCreated()) {
    GL_Dialog_Message(gl_.GetHandle(), "Clear Secret Area", "No Secret Area!!!",
                      GL_ICON_INFORMATION, GL_BUTTON_NONE, 2000);
    return;
  }

  T_SEC_DATAKEY_ID key_id;
  key_id.iSecretArea = kSecureAreaId;
  key_id.cAlgoType = 0;
  key_id.usNumber = 0;
  key_id.uiBankId = 0;

  GL_Dialog_Message(gl_.GetHandle(), "Clear Secret Area", "Clearing Secret Area",
                    GL_ICON_INFORMATION, GL_BUTTON_NONE, 0);
  int ret = SEC_FreeSecret(kSecureType, &key_id);

  if (ret == OK)
    DisplaySuccess();
  else
    DisplayError();

  DisplayHeader(ON);
}

bool KeyInjection::IsSecretAreaCreated() {
  if (SEC_isSecretArea(kSecureType, (SEG_ID) kSecureAreaId) == OK)
    return true;

  return false;
}

bool KeyInjection::CreateSecretArea() {
  GL_Dialog_Message(gl_.GetHandle(), "Key Injection", "Creating Secret Area",
                    GL_ICON_INFORMATION,
                    GL_BUTTON_NONE, 0);
  if (SEC_CreateSecretArea(kSecureType, (SEG_ID) kSecureAreaId, kCardId,
                           kVarId) == OK)
    return true;

  return false;
}

void KeyInjection::DisplaySuccess() {
  GL_Dialog_Message(gl_.GetHandle(), "Key Injection", "SUCCESS",
                    GL_ICON_INFORMATION,
                    GL_BUTTON_NONE, 2000);
}

void KeyInjection::DisplayError() {
  GL_Dialog_Message(gl_.GetHandle(), "Key Injection", "FAILURE", GL_ICON_ERROR,
                    GL_BUTTON_NONE,
                    2000);
}

void KeyInjection::DisplayMessage(const char* message) {
  GL_Dialog_Message(gl_.GetHandle(), "Key Injection", message, GL_ICON_NONE,
                    GL_BUTTON_NONE,
                    2000);
}
