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

#ifndef VERSION_H_
#define VERSION_H_

#include <versioning/telium_app_version.h>

class Version : public versioning::TeliumAppVersion {

 public:
  virtual std::string GetBuildNumber();
  virtual std::string GetVersion();
  virtual time_t GetBuildDatetime();
  virtual std::vector<versioning::Revision> GetRevisions();

};

#endif
