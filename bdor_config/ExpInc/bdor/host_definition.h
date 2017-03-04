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
#ifndef BDOR__HOST_DEFINITION_H_
#define BDOR__HOST_DEFINITION_H_

#include <registry/record.h>

namespace bdor {

enum HostProtocol {
  FDMS_BASE24 = 0,
  AMEX_DIRECT = 1
};

class HostDefinition: public registry::Record {
 public:

  uint64_t index;
  std::string host_name;
  HostProtocol host_protocol;
  std::string comms_host_name;

  std::string terminal_id;
  std::string merchant_id;

  std::string settlement_tpdu;
  std::string settlement_nii;

  std::string pre_defined_auth_code;

  bool batch_upload_enabled;

  uint64_t timeout;
  bool predial_enabled;

  typedef std::unique_ptr<HostDefinition> uptr;

  HostDefinition(){
  }

  virtual ~HostDefinition(){
  }

  virtual std::vector<registry::Mapper::uptr> BuildDataMap();
  virtual registry::Record::uptr GetNewInstance() const;

};

}

#endif
