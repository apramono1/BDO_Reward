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
#ifndef BDOR__APPLICATION_SETTINGS_H_
#define BDOR__APPLICATION_SETTINGS_H_

#include <stdx/memory>
#include <stdx/optional>

#include <registry/registry.h>
#include <registry/provider_sqlite.h>

#include <bdor/managed_settings.h>

namespace bdor {

class ApplicationSettings {
 public:
  ApplicationSettings(const std::string& partition,
                      const std::string& app_version_,
                      const std::string& build_date_)
      : app_version(app_version_),
        build_date(build_date_) {
    Initialize(partition);
  }

 public:

  void Initialize();
  void Terminate();
  std::string GetSettlementReceiptBmpPath() const;
  std::string GetLastCustomerTxReceiptBmpPath() const;
  std::string GetLastMerchantTxReceiptBmpPath() const;
  std::vector<HostDefinition> GetAllHostDefinitions() const;
  TerminalConfiguration& GetTerminalConfiguration() const;

  ManagedSettings::uptr managed_settings_;

  std::string app_version;
  std::string build_date;

 private:
  std::unique_ptr<registry::SqliteProvider> managed_setting_provider_;
  std::unique_ptr<registry::Registry> managed_registry_;

  void Initialize(const std::string& partition);
};

}

#endif
