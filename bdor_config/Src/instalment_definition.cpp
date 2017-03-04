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

#include <bdor/instalment_definition.h>
#include <stdx/memory>

#include <registry/mapper/mapper_uint.h>
#include <registry/mapper/mapper_bool.h>
#include <registry/mapper/mapper_string.h>
#include <registry/mapper/mapper_enum.h>

using namespace bdor;

namespace {
const char* kInstalmentIndexKey = "InstalmentIndex";
const char* kDescriptionKey = "Description";
const char* kInstalmentPeriodKey = "InstalmentPeriod";
const char* kMinimumAmount = "MinimumAmount";
const char* kMaximumAmount = "MaximumAmount";
const char* kFromDateKey = "FromDate";
const char* kToDateKey = "ToDate";
const char* kProgramIdKey = "ProgramId";
const char* kProductIdKey = "ProductId";
const char* kHostIndexKey = "HostIndex";
const char* kInstalmentPlanKey = "InstalmentOnUSOFFUS";

}

std::vector<registry::Mapper::uptr> InstalmentDefinition::BuildDataMap() {
  std::vector<registry::Mapper::uptr> map;

  map.push_back(
      stdx::make_unique<registry::MapperUInt>(kInstalmentIndexKey, index));
  map.push_back(
      stdx::make_unique<registry::MapperString>(kDescriptionKey, description));
  map.push_back(
      stdx::make_unique<registry::MapperUInt>(kInstalmentPeriodKey,
                                              instalment_period));
  map.push_back(
      stdx::make_unique<registry::MapperUInt>(kMinimumAmount, min_amount));
  map.push_back(
      stdx::make_unique<registry::MapperUInt>(kMaximumAmount, max_amount));
  map.push_back(
      stdx::make_unique<registry::MapperString>(kFromDateKey, from_date));
  map.push_back(stdx::make_unique<registry::MapperString>(kToDateKey, to_date));
  map.push_back(
      stdx::make_unique<registry::MapperString>(kProgramIdKey, program_id));
  map.push_back(
      stdx::make_unique<registry::MapperString>(kProductIdKey, product_id));
  map.push_back(
      stdx::make_unique<registry::MapperUInt>(kHostIndexKey, host_index));
  map.push_back(
        stdx::make_unique<registry::MapperBool>(kInstalmentPlanKey, instalment_onus_offus));
  return map;
}

registry::Record::uptr InstalmentDefinition::GetNewInstance() const {
  return stdx::make_unique<InstalmentDefinition>();
}
