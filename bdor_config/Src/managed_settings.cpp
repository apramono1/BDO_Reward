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
#include <bdor/managed_settings.h>

using namespace bdor;

const char* ManagedSettings::kManagedSettingsProviderKey = "fdmsconf";
const char* ManagedSettings::kTerminalConfigurationKey = "TerminalConfiguration";
const char* ManagedSettings::kAmexConfigurationKey = "AmexConfiguration";

namespace {
const char* kCardTableKey = "CardTable";
const char* kIssuerTableKey = "IssuerTable";
const char* kHostTableKey = "HostTable";
const char* KInstalmentTableKey = "InstalmentTable";

template<class T>
std::vector<T> GetAllElementsFromTable(const registry::Reader& reader,
                                       const char* table_key) {
  T dummy_instance;
  auto table = reader.GetTable(table_key, dummy_instance);

  std::vector<T> output;
  for (auto const& item : table) {
    T* rec = static_cast<T*>(item.get());
    output.push_back(*rec);
  }
  return output;
}

template<class T>
stdx::optional<T> GetElementFromTableByIndex(const registry::Reader& reader,
                                             const char* table_key,
                                             uint64_t index) {
  T dummy_instance;
  auto table = reader.GetTable(table_key, dummy_instance);

  stdx::optional<T> output;
  for (auto const& item : table) {
    T* rec = static_cast<T*>(item.get());
    if (rec->index == index) {
      output = *rec;
      break;
    }
  }
  return output;
}

}

registry::Reader ManagedSettings::GetReader() {
  return reader_;
}

std::vector<CardDefinition> ManagedSettings::GetAllCardDefinitions() {
  return GetAllElementsFromTable<CardDefinition>(reader_, kCardTableKey);
}

stdx::optional<CardDefinition> ManagedSettings::GetCardDefinition(
    uint64_t card_range_index) {
  return GetElementFromTableByIndex<CardDefinition>(reader_, kCardTableKey,
                                                    card_range_index);
}

stdx::optional<CardDefinition> ManagedSettings::GetCardDefinitionFromPan(
    const types::Pan& pan) {

  auto table = GetAllCardDefinitions();
  stdx::optional<CardDefinition> output;

  for (const auto& item : table) {
    if (pan.IsInRange(item.pan_low_range, item.pan_high_range)) {
      output = item;
      break;
    }
  }

  return output;
}

std::vector<HostDefinition> ManagedSettings::GetAllHostDefinitions() {
  return GetAllElementsFromTable<HostDefinition>(reader_, kHostTableKey);
}

stdx::optional<HostDefinition> ManagedSettings::GetHostDefinition(
    uint64_t host_index) {
  return GetElementFromTableByIndex<HostDefinition>(reader_, kHostTableKey,
                                                    host_index);
}

std::vector<IssuerDefinition> ManagedSettings::GetAllIssuerDefinitions() {
  return GetAllElementsFromTable<IssuerDefinition>(reader_, kIssuerTableKey);
}
stdx::optional<IssuerDefinition> ManagedSettings::GetIssuerDefinition(
    uint64_t issuer_index) {
  return GetElementFromTableByIndex<IssuerDefinition>(reader_, kIssuerTableKey,
                                                      issuer_index);
}
std::vector<InstalmentDefinition> ManagedSettings::GetAllInstalmentDefinitions() {
  return GetAllElementsFromTable<InstalmentDefinition>(reader_, KInstalmentTableKey);
}

stdx::optional<InstalmentDefinition> ManagedSettings::GetInstalmentDefinition(
    uint64_t instalment_index) {
  return GetElementFromTableByIndex<InstalmentDefinition>(reader_, KInstalmentTableKey,
                                                          instalment_index);
}
std::vector<InstalmentDefinition> ManagedSettings::GetAllInstalmentDefinitionsFromPlan(
    unsigned int instalment_option) {

  auto table = GetAllInstalmentDefinitions();
  std::vector<InstalmentDefinition> output;
  unsigned int instalment_onus_offus;

  for (const auto& item : table) {

    instalment_onus_offus = (unsigned int) item.instalment_onus_offus;
    if (instalment_onus_offus == instalment_option) {
      output.push_back(item);
    }
  }

  return output;
}
