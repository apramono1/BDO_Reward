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
#include <bdor/issuer_definition.h>
#include <stdx/memory>

#include <registry/mapper/mapper_uint.h>
#include <registry/mapper/mapper_string.h>


using namespace bdor;

namespace {
const char* kIndexKey = "Index";
const char* kIssuerNameKey = "IssuerName";
const char* kTpduKey = "Tpdu";
const char* kNiiKey = "Nii";
const char* kPanMaskDisplayKey = "PanMaskDisplay";
const char* kPanMaskMerchantReceiptKey = "PanMaskMerchantReceipt";
const char* kPanMaskCustomerReceiptKey = "PanMaskCustomerReceipt";
const char* kExpiryDateMaskDisplayKey = "ExpiryDateMaskDisplay";
const char* kExpiryDateMaskMerchantReceiptKey = "ExpiryDateMaskMerchantReceipt";
const char* kExpiryDateMaskCustomerReceiptKey = "ExpiryDateMaskCustomerReceipt";
}

std::vector<registry::Mapper::uptr> IssuerDefinition::BuildDataMap(){
  std::vector<registry::Mapper::uptr> map;

  map.push_back(stdx::make_unique<registry::MapperUInt>(kIndexKey, index));
  map.push_back(stdx::make_unique<registry::MapperString>(kIssuerNameKey, issuer_name));
  map.push_back(stdx::make_unique<registry::MapperString>(kTpduKey, tpdu));
  map.push_back(stdx::make_unique<registry::MapperString>(kNiiKey, nii));
  map.push_back(stdx::make_unique<registry::MapperString>(kPanMaskDisplayKey, pan_mask_display));
  map.push_back(stdx::make_unique<registry::MapperString>(kPanMaskMerchantReceiptKey, pan_mask_merchant_receipt));
  map.push_back(stdx::make_unique<registry::MapperString>(kPanMaskCustomerReceiptKey, pan_mask_customer_receipt));
  map.push_back(stdx::make_unique<registry::MapperString>(kExpiryDateMaskDisplayKey, expiry_date_mask_display));
  map.push_back(stdx::make_unique<registry::MapperString>(kExpiryDateMaskMerchantReceiptKey, expiry_date_mask_merchant_receipt));
  map.push_back(stdx::make_unique<registry::MapperString>(kExpiryDateMaskCustomerReceiptKey, expiry_date_mask_customer_receipt));

  return map;
}

registry::Record::uptr IssuerDefinition::GetNewInstance() const{
  return stdx::make_unique<IssuerDefinition>();
}
