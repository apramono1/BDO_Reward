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
#include <bdor/host_definition.h>
#include <stdx/memory>

#include <registry/mapper/mapper_uint.h>
#include <registry/mapper/mapper_bool.h>
#include <registry/mapper/mapper_string.h>
#include <registry/mapper/mapper_enum.h>


using namespace bdor;

namespace {
const char* kIndexKey = "Index";
const char* kHostNameKey = "HostName";
const char* kHostProtocolKey = "HostProtocol";
const char* kCommsHostNameKey = "CommsHostName";
const char* kTerminalIdKey = "TerminalId";
const char* kMerchantIdKey = "MerchantId";
const char* kTpduKey = "Tpdu";
const char* kNiiKey = "Nii";
const char* kPreDefinedAuthCodeKey = "PreDefinedAuthCode";
const char* kBatchUploadEnabledKey = "BatchUploadEnabled";
const char* kTimeoutKey = "Timeout";
const char* kPredialEnabledKey = "PredialEnabled";
}


std::vector<registry::Mapper::uptr> HostDefinition::BuildDataMap(){
  std::vector<registry::Mapper::uptr> map;

  map.push_back(stdx::make_unique<registry::MapperUInt>(kIndexKey, index));
  map.push_back(stdx::make_unique<registry::MapperString>(kHostNameKey, host_name));
  map.push_back(stdx::make_unique<registry::MapperEnum<HostProtocol>>(kHostProtocolKey, host_protocol));
  map.push_back(stdx::make_unique<registry::MapperString>(kCommsHostNameKey, comms_host_name));
  map.push_back(stdx::make_unique<registry::MapperString>(kTerminalIdKey, terminal_id));
  map.push_back(stdx::make_unique<registry::MapperString>(kMerchantIdKey, merchant_id));
  map.push_back(stdx::make_unique<registry::MapperString>(kTpduKey, settlement_tpdu));
  map.push_back(stdx::make_unique<registry::MapperString>(kNiiKey, settlement_nii));
  map.push_back(stdx::make_unique<registry::MapperString>(kPreDefinedAuthCodeKey, pre_defined_auth_code));
  map.push_back(stdx::make_unique<registry::MapperBool>(kBatchUploadEnabledKey, batch_upload_enabled));
  map.push_back(stdx::make_unique<registry::MapperUInt>(kTimeoutKey, timeout));
  map.push_back(stdx::make_unique<registry::MapperBool>(kPredialEnabledKey, predial_enabled));

  return map;
}

registry::Record::uptr HostDefinition::GetNewInstance() const{
  return stdx::make_unique<HostDefinition>();
}
