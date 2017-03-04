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

#include <bdor/card_definition.h>
#include <stdx/memory>

#include <registry/mapper/mapper_uint.h>
#include <registry/mapper/mapper_bool.h>
#include <registry/mapper/mapper_string.h>
#include <registry/mapper/mapper_enum.h>


using namespace bdor;

namespace {
const char* kIndexKey =  "Index";
const char* kPanLowRangeKey = "PanLowRange";
const char* kPanHighRangeKey = "PanHighRange";
const char* kPanMinLengthKey = "PanMinLength";
const char* kPanMaxLengthKey = "PanMaxLength";
const char* kHostIndexKey = "HostIndex";
// TODO: to check if needed uint64_t manual_entry_host_index;
const char* kMotoHostIndexKey = "MotoHostIndex";
const char* kIssuerIndexKey = "IssuerIndex";
const char* kFloorLimitKey = "FloorLimit";
const char* kMotoFloorLimitKey = "MotoFloorLimit";
const char* kQpsFloorLimitKey = "QpsFloorLimit";
const char* kCvvManualEntryKey = "CvvManualEntry";
const char* kCvvMotoSaleKey = "CvvMotoSale";
const char* kCvvMagstripeEntryKey = "CvvMagstripeEntry";
// TODO: check if needed: bool expiry_date_required;
const char* kTxSaleEnabledKey = "TxSaleEnabled";
const char* kTxPreAuthEnabledKey = "TxPreAuthEnabled";
const char* kTxOfflineSaleEnabledKey = "TxOfflineSaleEnabled";
const char* kTxRefundEnabledKey = "TxRefundEnabled";
const char* kTxVoidEnabledKey = "TxVoidEnabled";
const char* kTxVoidPreAuthEnabledKey = "TxVoidPreAuthEnabled";
const char* kTxPreAuthCompOnlineEnabledKey = "TxPreAuthCompOnlineEnabled";
const char* kTxPreAuthCompOfflineEnabledKey = "TxPreAuthCompOfflineEnabled";
const char* kTxCashAdvanceEnabledKey = "TxCashAdvanceEnabled";
const char* kTxMotoSaleEnabledKey = "TxMotoSaleEnabled";
const char* kTxMotoRefundEnabledKey = "TxMotoRefundEnabled";
const char* kTxMotoPreauthEnabledKey = "TxMotoPreauthEnabled";
const char* kTxInstalmentSaleEnabledKey = "TxInstalmentSaleEnabled";
const char* kTxManualEntryEnabledKey = "TxManualEntryEnabled";
// TODO: what's that? bool tx_tx_card_payment_enabled;
const char* kLuhnCheckRequiredKey = "LuhnCheckRequired";
const char* kDccEnabledKey = "DccEnabled";
}


std::vector<registry::Mapper::uptr> CardDefinition::BuildDataMap(){
  std::vector<registry::Mapper::uptr> map;

  map.push_back(stdx::make_unique<registry::MapperUInt>(kIndexKey, index));
  map.push_back(stdx::make_unique<registry::MapperString>(kPanLowRangeKey, pan_low_range));
  map.push_back(stdx::make_unique<registry::MapperString>(kPanHighRangeKey, pan_high_range));
  map.push_back(stdx::make_unique<registry::MapperUInt>(kPanMinLengthKey, pan_min_length));
  map.push_back(stdx::make_unique<registry::MapperUInt>(kPanMaxLengthKey, pan_max_length));
  map.push_back(stdx::make_unique<registry::MapperUInt>(kHostIndexKey, host_index));
  map.push_back(stdx::make_unique<registry::MapperUInt>(kMotoHostIndexKey, moto_host_index));
  map.push_back(stdx::make_unique<registry::MapperUInt>(kIssuerIndexKey, issuer_index));
  map.push_back(stdx::make_unique<registry::MapperUInt>(kFloorLimitKey, floor_limit));
  map.push_back(stdx::make_unique<registry::MapperUInt>(kMotoFloorLimitKey, moto_floor_limit));
  map.push_back(stdx::make_unique<registry::MapperUInt>(kQpsFloorLimitKey, qps_floor_limit));
  map.push_back(stdx::make_unique<registry::MapperEnum<CvvMode>>(kCvvManualEntryKey, cvv_manual_entry));
  map.push_back(stdx::make_unique<registry::MapperEnum<CvvMode>>(kCvvMotoSaleKey, cvv_moto_sale));
  map.push_back(stdx::make_unique<registry::MapperEnum<CvvMode>>(kCvvMagstripeEntryKey, cvv_magstripe_entry));
  map.push_back(stdx::make_unique<registry::MapperBool>(kTxSaleEnabledKey, tx_sale_enabled));
  map.push_back(stdx::make_unique<registry::MapperBool>(kTxPreAuthEnabledKey, tx_preauth_enabled));
  map.push_back(stdx::make_unique<registry::MapperBool>(kTxOfflineSaleEnabledKey, tx_offline_sale_enabled));
  map.push_back(stdx::make_unique<registry::MapperBool>(kTxRefundEnabledKey, tx_refund_enabled));
  map.push_back(stdx::make_unique<registry::MapperBool>(kTxVoidEnabledKey, tx_void_enabled));
  map.push_back(stdx::make_unique<registry::MapperBool>(kTxVoidPreAuthEnabledKey, tx_void_preauth_enabled));
  map.push_back(stdx::make_unique<registry::MapperBool>(kTxPreAuthCompOnlineEnabledKey, tx_preauth_completion_online_enabled));
  map.push_back(stdx::make_unique<registry::MapperBool>(kTxPreAuthCompOfflineEnabledKey, tx_preauth_completion_offline_enabled));
  map.push_back(stdx::make_unique<registry::MapperBool>(kTxCashAdvanceEnabledKey, tx_cash_advance_enabled));
  map.push_back(stdx::make_unique<registry::MapperBool>(kTxMotoSaleEnabledKey, tx_moto_sale_enabled));
  map.push_back(stdx::make_unique<registry::MapperBool>(kTxMotoRefundEnabledKey, tx_moto_refund_enabled));
  map.push_back(stdx::make_unique<registry::MapperBool>(kTxMotoPreauthEnabledKey, tx_moto_preauth_enabled));
  map.push_back(stdx::make_unique<registry::MapperBool>(kTxInstalmentSaleEnabledKey, tx_instalment_sale_enabled));
  map.push_back(stdx::make_unique<registry::MapperBool>(kTxManualEntryEnabledKey, manual_entry_enabled));
  map.push_back(stdx::make_unique<registry::MapperBool>(kLuhnCheckRequiredKey, luhn_check_required));
  map.push_back(stdx::make_unique<registry::MapperBool>(kDccEnabledKey, dcc_enabled));

  return map;
}

registry::Record::uptr CardDefinition::GetNewInstance() const{
  return stdx::make_unique<CardDefinition>();
}
