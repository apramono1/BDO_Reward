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
#include <bdor/terminal_configuration.h>

using namespace bdor;

namespace {
const char* kTerminalBaseCurrencyKey = "TerminalBaseCurrency";
const char* kMinimumAmount = "MinimumAmount";
const char* kMaximumAmount = "MaximumAmount";
const char* kReceiptHeaderLine1Key = "ReceiptHeaderLine1";
const char* kReceiptHeaderLine2Key = "ReceiptHeaderLine2";
const char* kReceiptHeaderLine3Key = "ReceiptHeaderLine3";
const char* kReceiptHeaderLine4Key = "ReceiptHeaderLine4";
const char* kReceiptHeaderLine5Key = "ReceiptHeaderLine5";
const char* kTipProcessingKey = "TipProcessing";
const char* kPreAuthClearPolicy = "PreAuthClearPolicy";
const char* kPreAuthMaxAgeKey = "PreAuthMaxAge";
const char* kPreAuthCompletionAllowedVarianceKey =
    "PreAuthCompletionAllowedVariance";
const char* kSuperAdminPasswordKey = "SuperAdminPassword";
const char* kSupervisorPasswordKey = "SupervisorPassword";
const char* kManagerPasswordKey = "ManagerPassword";
const char* kTleEnabledKey = "TleEnabled";
const char* kPiggybackEnabledKey = "PiggybackEnabled";
const char* kTxSaleEnabledKey = "TxSaleEnabled";
const char* kTxPreAuthEnabledKey = "TxPreAuthEnabled";
const char* kTxPreAuthCompletionOnlineEnabledKey =
    "TxPreAuthCompletionOnlineEnabled";
const char* kTxPreAuthCompletionOfflineEnabledKey =
    "TxPreAuthCompletionOfflineEnabled";
const char* kTxOfflineSaleEnabledKey = "TxOfflineSaleEnabled";
const char* kTxRefundEnabledKey = "TxRefundEnabled";
const char* kTxVoidEnabledKey = "TxVoidEnabled";
const char* kTxVoidPreAuthEnabledKey = "TxVoidPreAuthEnabled";
const char* kTxCashAdvanceEnabledKey = "TxCashAdvanceEnabled";
const char* kTxMotoSaleEnabledKey = "TxMotoSaleEnabled";
const char* kTxMotoRefundEnabledKey = "TxMotoRefundEnabled";
const char* kTxMotoPreauthEnabledKey = "TxMotoPreauthEnabled";
const char* kTxInstalmentSaleEnabledKey = "TxInstalmentSaleEnabled";
const char* kTxSettleAllHostEnabledKey = "TxSettleAllHostEnabled";
const char* kEmvEnabledKey = "EmvEnabled";
const char* kInstalmentDataToHostEnabledKey = "InstalmentDataToHostEnabled";
const char* kInstalmentReceiptDescriptionKey = "InstalmentReceiptDescription";
const char* kIdleMessageKey = "IdleMessage";
const char* kTerminalIdMaskingKey = "TerminalIdMasking";
const char* kMerchantIdMaskingKey = "MerchantIdMasking";
const char* kRefundMaxAmountKey = "RefundMaxAmount";
const char* kRefundPasswordFloorLimitKey = "RefundPasswordFloorLimit";
const char* kRefundClessEnabledKey = "RefundClessEnabled";
const char* kTipAdjustMaxNumberKey = "TipAdjustMaxNumber";
const char* kTipAdjustMaxVarianceKey = "TipAdjustMaxVariance";
const char* kBatchMaxTxNumberKey = "BatchMaxTxNumber";
const char* kDccEnabledKey = "DccEnabled";
const char* kDccProviderKey = "DccProvider";
const char* kDccStaffIdInputEnabledKey = "DccStaffIdInputEnabled";
const char* kSettlementPasswordLevelKey = "SettlementPasswordLevel";
const char* kOfflineSalePasswordLevelKey = "OfflineSalePasswordLevel";
const char* kRefundPasswordLevelKey = "RefundPasswordLevel";
const char* kVoidPasswordLevelKey = "VoidPasswordLevel";
const char* kOfflineSaleMaxAmountKey = "OfflineSaleMaxAmount";
const char* KSaleContactlessEnabledKey = "SaleContactlessEnabled";
}

types::Currency TerminalConfiguration::TerminalBaseCurrency() {
  int currency_code = reader_.GetInt(kTerminalBaseCurrencyKey, 999);
  return types::Currency(currency_code);
}

uint64_t TerminalConfiguration::MinAmount() {
  return reader_.GetUInt(kMinimumAmount);
}

uint64_t TerminalConfiguration::MaxAmount() {
  return reader_.GetUInt(kMaximumAmount);
}

// Receipt
std::string TerminalConfiguration::ReceiptHeaderLine1() {
  return reader_.GetString(kReceiptHeaderLine1Key);
}

std::string TerminalConfiguration::ReceiptHeaderLine2() {
  return reader_.GetString(kReceiptHeaderLine2Key);
}

std::string TerminalConfiguration::ReceiptHeaderLine3() {
  return reader_.GetString(kReceiptHeaderLine3Key);
}

std::string TerminalConfiguration::ReceiptHeaderLine4() {
  return reader_.GetString(kReceiptHeaderLine4Key);
}

std::string TerminalConfiguration::ReceiptHeaderLine5() {
  return reader_.GetString(kReceiptHeaderLine5Key);
}

TerminalConfiguration::TipProcessingMode TerminalConfiguration::TipProcessing() {
  uint64_t value = reader_.GetUInt(kTipProcessingKey,
                                   TipProcessingMode::TIP_PROMPTED_AND_PRINTED);
  return static_cast<TipProcessingMode>(value);
}

TerminalConfiguration::PreAuthClearMode TerminalConfiguration::PreAuthClearPolicy() {
  uint64_t value = reader_.GetUInt(
      kPreAuthClearPolicy,
      PreAuthClearMode::SETTLE_NO_PROMPT_AND_CLEAR);
  return static_cast<PreAuthClearMode>(value);
}

uint64_t TerminalConfiguration::PreAuthMaxAge() {  // in days
  return reader_.GetUInt(kPreAuthMaxAgeKey);
}

uint64_t TerminalConfiguration::PreAuthCompletionAllowedVariance() {  // in %
  return reader_.GetUInt(kPreAuthCompletionAllowedVarianceKey);
}

std::string TerminalConfiguration::SuperAdminPassword() {
  return reader_.GetString(kSuperAdminPasswordKey, "123456");  // TODO: remove the default value
}

std::string TerminalConfiguration::SupervisorPassword() {
  return reader_.GetString(kSupervisorPasswordKey);
}

std::string TerminalConfiguration::ManagerPassword() {
  return reader_.GetString(kManagerPasswordKey);
}

bool TerminalConfiguration::TleEnabled() {
  return reader_.GetBoolean(kTleEnabledKey, false);
}

bool TerminalConfiguration::PiggybackEnabled() {
  return reader_.GetBoolean(kPiggybackEnabledKey, true);
}

bool TerminalConfiguration::TxSaleEnabled() {
  return reader_.GetBoolean(kTxSaleEnabledKey, true);
}

bool TerminalConfiguration::TxPreAuthEnabled() {
  return reader_.GetBoolean(kTxPreAuthEnabledKey, true);
}

bool TerminalConfiguration::TxPreAuthCompletionOnlineEnabled() {
  return reader_.GetBoolean(kTxPreAuthCompletionOnlineEnabledKey, true);
}

bool TerminalConfiguration::TxPreAuthCompletionOfflineEnabled() {
  return reader_.GetBoolean(kTxPreAuthCompletionOfflineEnabledKey, true);
}

bool TerminalConfiguration::TxOfflineSaleEnabled() {
  return reader_.GetBoolean(kTxOfflineSaleEnabledKey, true);
}

bool TerminalConfiguration::TxRefundEnabled() {
  return reader_.GetBoolean(kTxRefundEnabledKey, true);
}

bool TerminalConfiguration::TxVoidEnabled() {
  return reader_.GetBoolean(kTxVoidEnabledKey, true);
}

bool TerminalConfiguration::TxVoidPreAuthEnabled() {
  return reader_.GetBoolean(kTxVoidPreAuthEnabledKey, true);
}

bool TerminalConfiguration::TxCashAdvanceEnabled() {
  return reader_.GetBoolean(kTxCashAdvanceEnabledKey, true);
}

bool TerminalConfiguration::TxMotoSaleEnabled() {
  return reader_.GetBoolean(kTxMotoSaleEnabledKey, true);
}

bool TerminalConfiguration::TxMotoRefundEnabled() {
  return reader_.GetBoolean(kTxMotoRefundEnabledKey, true);
}

bool TerminalConfiguration::TxMotoPreauthEnabled() {
  return reader_.GetBoolean(kTxMotoPreauthEnabledKey, true);
}

bool TerminalConfiguration::TxInstalmentSaleEnabled() {
  return reader_.GetBoolean(kTxInstalmentSaleEnabledKey, true);
}

bool TerminalConfiguration::TxSettleAllHostEnabled() {
  return reader_.GetBoolean(kTxSettleAllHostEnabledKey, true);
}

// TODO: what's that? bool TxTxCardPayementEnabled();

bool TerminalConfiguration::EmvEnabled() {
  return reader_.GetBoolean(kEmvEnabledKey, true);
}

bool TerminalConfiguration::SaleContactlessEnabled() {
  return reader_.GetBoolean(KSaleContactlessEnabledKey, true);
}

// TODO: is it needed: bool SeparateMotoHostEnabled();
bool TerminalConfiguration::InstalmentDataToHostEnabled() {
  return reader_.GetBoolean(kInstalmentDataToHostEnabledKey, true);
}

std::string TerminalConfiguration::InstalmentReceiptDescription() {
  return reader_.GetString(kInstalmentReceiptDescriptionKey);
}

std::string TerminalConfiguration::IdleMessage() {
  return reader_.GetString(kIdleMessageKey);
}

std::string TerminalConfiguration::TerminalIdMasking() {
  return reader_.GetString(kTerminalIdMaskingKey);
}

std::string TerminalConfiguration::MerchantIdMasking() {
  return reader_.GetString(kMerchantIdMaskingKey);
}

uint64_t TerminalConfiguration::RefundMaxAmount() {
  return reader_.GetUInt(kRefundMaxAmountKey);
}

uint64_t TerminalConfiguration::RefundPasswordFloorLimit() {
  return reader_.GetUInt(kRefundPasswordFloorLimitKey);
}

bool TerminalConfiguration::RefundClessEnabled() {
  return reader_.GetBoolean(kRefundClessEnabledKey, true);
}

uint64_t TerminalConfiguration::TipAdjustMaxNumber() {
  return reader_.GetUInt(kTipAdjustMaxNumberKey, 1);
}

uint64_t TerminalConfiguration::TipAdjustMaxVariance() {  // in %
  return reader_.GetUInt(kTipAdjustMaxVarianceKey);
}

uint64_t TerminalConfiguration::BatchMaxTxNumber() {  // TODO: check if needed? should be related to memory available
  return reader_.GetUInt(kBatchMaxTxNumberKey);
}

bool TerminalConfiguration::DccEnabled() {
  return reader_.GetBoolean(kDccEnabledKey, true);
}

TerminalConfiguration::DccProviderOption TerminalConfiguration::DccProvider() {
  uint64_t value = reader_.GetUInt(kDccProviderKey,
                                   DccProviderOption::FEXCO);
  return static_cast<DccProviderOption>(value);
}

bool TerminalConfiguration::DccStaffIdInputEnabled() {
  return reader_.GetBoolean(kDccStaffIdInputEnabledKey, true);
}

types::PasswordAccessLevel TerminalConfiguration::GetSettlementPasswordLevel() {
  uint64_t pw_level = reader_.GetUInt(kSettlementPasswordLevelKey,
                                      types::PasswordAccessLevel::SUPERVISOR);
  return static_cast<types::PasswordAccessLevel>(pw_level);
}

types::PasswordAccessLevel TerminalConfiguration::GetOfflineSalePasswordLevel() {
  uint64_t pw_level = reader_.GetUInt(kOfflineSalePasswordLevelKey,
                                      types::PasswordAccessLevel::SUPERVISOR);
  return static_cast<types::PasswordAccessLevel>(pw_level);
}

types::PasswordAccessLevel TerminalConfiguration::GetRefundPasswordLevel() {
  uint64_t pw_level = reader_.GetUInt(kRefundPasswordLevelKey,
                                      types::PasswordAccessLevel::MANAGER);
  return static_cast<types::PasswordAccessLevel>(pw_level);
}

types::PasswordAccessLevel TerminalConfiguration::GetVoidPasswordLevel() {
  uint64_t pw_level = reader_.GetUInt(kVoidPasswordLevelKey,
                                      types::PasswordAccessLevel::SUPERVISOR);
  return static_cast<types::PasswordAccessLevel>(pw_level);
}

uint64_t TerminalConfiguration::OfflineSaleMaxAmount() {
  return reader_.GetUInt(kOfflineSaleMaxAmountKey, MaxAmount());
}

