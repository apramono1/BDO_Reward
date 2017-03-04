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
#include <bdor/application_settings.h>

namespace bdor {

void ApplicationSettings::Initialize(const std::string& partition) {
  managed_setting_provider_ = stdx::make_unique<registry::SqliteProvider>(
      ManagedSettings::kManagedSettingsProviderKey, partition.c_str(), false,
      true);			//TODO, use partition

  managed_registry_ = stdx::make_unique<registry::Registry>(
      managed_setting_provider_.get());

  managed_settings_ = stdx::make_unique<ManagedSettings>(
      managed_registry_->GetReader(
          ManagedSettings::kManagedSettingsProviderKey));
}

void ApplicationSettings::Terminate() {
  managed_settings_ = nullptr;
  managed_registry_ = nullptr;
  managed_setting_provider_ = nullptr;
}

std::vector<HostDefinition> ApplicationSettings::GetAllHostDefinitions() const {
  return managed_settings_->GetAllHostDefinitions();
}

TerminalConfiguration& ApplicationSettings::GetTerminalConfiguration() const {
  return managed_settings_->terminal_config;
}

std::string ApplicationSettings::GetSettlementReceiptBmpPath() const {
  return std::string("file://flash/HOST/SETTLE.BMP");
}

std::string ApplicationSettings::GetLastCustomerTxReceiptBmpPath() const {
  return std::string("file://flash/HOST/CUS_RCPT.BMP");
}

std::string ApplicationSettings::GetLastMerchantTxReceiptBmpPath() const {
  return std::string("file://flash/HOST/MER_RCPT.BMP");
}

}
