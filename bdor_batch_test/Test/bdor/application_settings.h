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

namespace bdor {

class TerminalConfiguration {
 public:
  uint64_t PreAuthMaxAge() {
    return 21;
  }

  types::Currency TerminalBaseCurrency() {
    return types::Currency("SGD");
  }
};

class HostDefinition {
 public:

  uint64_t index;
  std::string host_name;
  std::string comms_host_name;
};

class IssuerDefinition {
 public:

  uint64_t index;
  std::string issuer_name;

  std::string pan_mask_display;
  std::string pan_mask_merchant_receipt;
  std::string pan_mask_customer_receipt;
};

class ManagedSettings {
 public:
  typedef std::unique_ptr<ManagedSettings> uptr;

  std::vector<HostDefinition> GetAllHostDefinitions() {
    std::vector<HostDefinition> output;
    HostDefinition host;
    host.index = 1;

    output.push_back(host);
    return output;
  }

  stdx::optional<IssuerDefinition> GetIssuerDefinition(uint64_t issuer_index) {
    IssuerDefinition issuer;
    issuer.index = 1;
    issuer.issuer_name = "ABC";
    return issuer;
  }

};

class ApplicationSettings {
 public:
  TerminalConfiguration GetTerminalConfiguration() {
    return TerminalConfiguration();
  }

  ManagedSettings::uptr managed_settings_;
};

}
