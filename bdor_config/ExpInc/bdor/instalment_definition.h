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
#ifndef BDOR__INSTALMENT_DEFINITION_H_
#define BDOR__INSTALMENT_DEFINITION_H_

#include <registry/record.h>

namespace bdor {

class InstalmentDefinition : public registry::Record {
 public:

  typedef std::unique_ptr<InstalmentDefinition> uptr;

  virtual std::vector<registry::Mapper::uptr> BuildDataMap();
  virtual registry::Record::uptr GetNewInstance() const;

  std::uint64_t index;
  std::string description;
  std::uint64_t instalment_period;
  std::uint64_t min_amount;
  std::uint64_t max_amount;
  std::string from_date;
  std::string to_date;
  std::string program_id;
  std::string product_id;

  std::uint64_t host_index;
  bool instalment_onus_offus;

};

}

#endif
