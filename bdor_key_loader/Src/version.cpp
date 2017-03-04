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

#include "version.h"

std::string Version::GetBuildNumber() {
  return "0";  // $BUILD_NUMBER$
}

std::string Version::GetVersion() {
  return "0.0.0";  // $VERSION$
}

time_t Version::GetBuildDatetime() {
  return 0;  // $BUILD_DATETIME$
}

std::vector<versioning::Revision> Version::GetRevisions() {
  return { {"", ""}};  // $REVISIONS$
}

