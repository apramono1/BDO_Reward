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
#include <bdor/amex_configuration.h>

using namespace bdor;

namespace {
const char* kAmexOriginKey = "AmexOrigin";
const char* kAmexCountryCodeKey = "AmexCountryCode";
const char* kAmexRegionKey = "AmexRegion";
const char* kAmexRoutingIndicatorKey = "AmexRoutingIndicator";
}

std::string AmexConfiguration::Origin() {
  return reader_.GetString(kAmexOriginKey);
}

std::string AmexConfiguration::CountryCode() {
  return reader_.GetString(kAmexCountryCodeKey);
}

std::string AmexConfiguration::Region() {
  return reader_.GetString(kAmexRegionKey);
}

std::string AmexConfiguration::RoutingIndicator() {
  return reader_.GetString(kAmexRoutingIndicatorKey);
}
