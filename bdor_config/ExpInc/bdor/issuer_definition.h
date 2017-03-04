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
#ifndef BDOR__ISSUER_DEFINITION_H_
#define BDOR__ISSUER_DEFINITION_H_

#include <registry/record.h>
#include <string>

namespace bdor {

class IssuerDefinition: public registry::Record {
 public:

  uint64_t index;
  std::string issuer_name;

  std::string tpdu;
  std::string nii;

  std::string pan_mask_display;
  std::string pan_mask_merchant_receipt;
  std::string pan_mask_customer_receipt;

  std::string expiry_date_mask_display;
  std::string expiry_date_mask_merchant_receipt;
  std::string expiry_date_mask_customer_receipt;

  typedef std::unique_ptr<IssuerDefinition> uptr;

  IssuerDefinition() {
  }

  virtual ~IssuerDefinition() {
  }

  virtual std::vector<registry::Mapper::uptr> BuildDataMap();
  virtual registry::Record::uptr GetNewInstance() const;
};

}

#endif
