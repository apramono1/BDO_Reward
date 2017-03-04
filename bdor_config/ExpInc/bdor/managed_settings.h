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
#ifndef BDOR__MANAGED_SETTINGS_H_
#define BDOR__MANAGED_SETTINGS_H_

#include <bdor/card_definition.h>
#include <bdor/host_definition.h>
#include <bdor/issuer_definition.h>
#include <bdor/instalment_definition.h>
#include <bdor/terminal_configuration.h>
#include <bdor/amex_configuration.h>

#include <stdx/memory>
#include <stdx/optional>
#include <types/pan.h>

#include <registry/registry.h>
#include <registry/provider_sqlite.h>

namespace bdor {

class ManagedSettings {
 public:
  static const char* kManagedSettingsProviderKey;
  static const char* kTerminalConfigurationKey;
  static const char* kAmexConfigurationKey;

  typedef std::unique_ptr<ManagedSettings> uptr;

  ManagedSettings(registry::Reader reader)
      : reader_(reader),
        sub_reader_terminal_config_(reader, kTerminalConfigurationKey),
        sub_reader_amex_config_(reader, kAmexConfigurationKey),
        terminal_config(sub_reader_terminal_config_),
        amex_config(sub_reader_amex_config_) {
  }

 private:
  registry::Reader reader_;
  registry::Reader sub_reader_terminal_config_;
  registry::Reader sub_reader_amex_config_;

 public:
  TerminalConfiguration terminal_config;
  AmexConfiguration amex_config;

  registry::Reader GetReader();

  std::vector<CardDefinition> GetAllCardDefinitions();
  stdx::optional<CardDefinition> GetCardDefinition(uint64_t card_range_index);
  stdx::optional<CardDefinition> GetCardDefinitionFromPan(
      const types::Pan& pan);

  std::vector<HostDefinition> GetAllHostDefinitions();
  stdx::optional<HostDefinition> GetHostDefinition(uint64_t host_index);

  std::vector<IssuerDefinition> GetAllIssuerDefinitions();
  stdx::optional<IssuerDefinition> GetIssuerDefinition(uint64_t issuer_index);

  std::vector<InstalmentDefinition> GetAllInstalmentDefinitions();
  stdx::optional<InstalmentDefinition> GetInstalmentDefinition(
      uint64_t instalment_index);
  std::vector<InstalmentDefinition> GetAllInstalmentDefinitionsFromPlan(
      unsigned int instalment_option);

};

}

#endif
