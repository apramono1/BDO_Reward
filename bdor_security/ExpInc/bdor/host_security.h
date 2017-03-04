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
#ifndef BDOR__HOST_SECURITY_H_
#define BDOR__HOST_SECURITY_H_

#include <vector>
#include <memory>
#include <algorithm>
#include <types/pan.h>
#include <registry/registry.h>

namespace bdor {

/**
 * Host security facade class.
 */
class HostSecurity {
 public:
  HostSecurity(registry::Reader config_reader);
  ~HostSecurity();

  /*
   * Key management.
   */

  /**
   * Check TMK.
   *
   * @return true if a TMK has been set
   */
  bool HasTmk();

  /**
   * Load CUP PIN Working Key
   *
   * @param cup_pin_key_tmk CUP PIN key wrapped with TMK
   * @return true if successful, false otherwise
   */
  bool SetupCupPinWorkingKey(const uint8_t *cup_pin_key_tmk);

  /**
   * Check CUP PIN Working Key
   *
   * @return true if a CUP PIN Working Key has been set
   */
  bool HasCupPinWorkingKey();

  /*
   * Cryptography
   */

  std::vector<uint8_t> GetCupPinBlock(const types::Pan &pan);

  /*
   * Implementation
   */

 private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};
}

#endif
