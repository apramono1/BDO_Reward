/*
 ------------------------------------------------------------------------------
 INGENICO Technical Software Department
 ------------------------------------------------------------------------------
 Copyright (c) 2014 INGENICO S.A.
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
#ifndef BDOR__TERMINAL_LINE_ENCRYPTION_IMPL_H_
#define BDOR__TERMINAL_LINE_ENCRYPTION_IMPL_H_

#include <iso8583/apdu.h>
#include <bdor/application_setting.h>
#include <bdor/host_security.h>

#include <bdor/terminal_line_encryption.h>

namespace bdor {

class TerminalLineEncryptionImpl : public TerminalLineEncryption {
 public:
  TerminalLineEncryptionImpl(bdor::ApplicationSetting& app_setting)
      : app_setting_(app_setting) {
  }

  /**
   * Wrap a message for the host
   *
   * The caller must ensure message is well formed,
   * The message is modified in place.
   *
   * @param msg the message to wrap
   */
  bool Wrap(iso8583::Apdu& message);

 private:
  bdor::ApplicationSetting& app_setting_;
  std::unique_ptr<bdor::HostSecurity> host_sec_;

  bool IsEncryptionAllowed();
  bool InitializeHostSecurity(iso8583::Apdu& message);
  bool FieldShouldBeEncrypted(int bitn, iso8583::Apdu& message);
  std::vector<uint8_t> GetDataToEncrypt(int bitn, iso8583::Apdu& message);
  iso8583::Apdu::FieldAccessStatus SetEncryptedField(int bitn,
                                                     iso8583::Apdu& message);
  bool EncryptMessage(iso8583::Apdu& message);
};

}

#endif
