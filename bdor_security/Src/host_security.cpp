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
#include <bdor/host_security.h>

#include <stdx/memory>
#include <crypto/crypto.h>

namespace bdor {

namespace {
const char* kTmkLocationKey = "TMK_LOCATION";
const char* kCupPinKeyLocationKey = "CUP_PIN_KEY_LOCATION";
}

struct HostSecurity::Impl {
  Impl(registry::Reader config_reader_)
      : config_reader(config_reader_) {
  }

  std::string GetTmkLocation() {
    return config_reader.GetString(kTmkLocationKey);
  }

  std::string GetCupPinWorkingKeyLocation() {
    return config_reader.GetString(kCupPinKeyLocationKey);
  }

  bool CheckTmk() {
    if (!tmk) {
      auto key_loc = GetTmkLocation();
      tmk = crypto::GetKeyEncryptionKey(key_loc);
    }
    return tmk->GetStatus() == crypto::Key::KEY_OK;
  }

  bool CheckCupPinWorkingKey() {
    if (!cup_pin_key) {
      auto key_loc = GetCupPinWorkingKeyLocation();
      cup_pin_key = crypto::GetPinEncryptionKey(key_loc);
    }
    return cup_pin_key->GetStatus() == crypto::Key::KEY_OK;
  }

  registry::Reader config_reader;
  crypto::KeyEncryptionKey::uptr tmk;
  crypto::PinEncryptionKey::uptr cup_pin_key;
};

HostSecurity::HostSecurity(registry::Reader config_reader)
    :
      impl_(stdx::make_unique<Impl>(config_reader)) {
}

HostSecurity::~HostSecurity() {
}

bool HostSecurity::HasTmk() {
  return impl_->CheckTmk();
}

bool HostSecurity::SetupCupPinWorkingKey(const uint8_t *cup_pin_key_tmk) {

  // make sure TMK is available
  if (!HasTmk())
    return false;

  // load key
  crypto::KeyEncryptionKey::LoadResult status = impl_->tmk->LoadKey(
      impl_->GetCupPinWorkingKeyLocation(),
      crypto::KeyEncryptionKey::Mode::CBC,
      crypto::Key::TDES_KEY,
      crypto::Key::CIPHER_PIN,
      cup_pin_key_tmk,
      nullptr);

  return status == crypto::KeyEncryptionKey::LoadResult::LOAD_OK;
}

bool HostSecurity::HasCupPinWorkingKey() {
  return impl_->CheckCupPinWorkingKey();
}

std::vector<uint8_t> HostSecurity::GetCupPinBlock(const types::Pan &pan) {
  if (!impl_->CheckCupPinWorkingKey())
    return std::vector<uint8_t>();

  impl_->cup_pin_key->Start_Iso9564_F0(pan);
  return impl_->cup_pin_key->GetResult();
}

}
