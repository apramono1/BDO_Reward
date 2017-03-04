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
#ifndef BDOR__AMEX_CONFIGURATION_H_
#define BDOR__AMEX_CONFIGURATION_H_

#include <registry/registry.h>

namespace bdor {

class AmexConfiguration {
 public:

  AmexConfiguration(registry::Reader reader)
      : reader_(reader) {
  }

 private:
  registry::Reader reader_;

 public:
  std::string Origin();
  std::string CountryCode();
  std::string Region();
  std::string RoutingIndicator();


};

}

#endif
