/*
 ------------------------------------------------------------------------------
 INGENICO Technical Software Department
 ------------------------------------------------------------------------------
 Copyright (c) 2015-2016 INGENICO S.A.
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
#ifndef BDOR__PASSWORD_H_
#define BDOR__PASSWORD_H_

#include <cstdio>
#include <string>
#include <cstring>
#include <bdor/application_settings.h>
#include <bdor/transaction.h>
#include <bdor/ui.h>

namespace bdor {

class Password {
 public:
  Password(ApplicationSettings& app_setting, Ui& ui);

  bool PerformPasswordAccess(const TransactionType tx_type) const;
  bool PerformPasswordAccess(const bool is_void_proccessing = true) const;
  bool PerformManagerPasswordAccess() const;
  bool PerformSuperAdminPasswordAccess() const;
  bool PerformSettlementPasswordAccess() const;

 private:
  ApplicationSettings& app_settings_;
  Ui& ui_;

  bool PerformPasswordAccess(const types::PasswordAccessLevel pw_level) const;
  bool PerformSuperAdminAccess() const;
  bool PerformManagerAccess() const;
  bool PerformSupervisorAccess() const;

  bool IsSuperAdminPassword(const std::string password) const;
  bool IsManagerPassword(const std::string password) const;
  bool IsSupervisorPassword(const std::string password) const;

  bool IsAllowedToAcessSuperAdmin(const std::string password) const;
  bool IsAllowedToAcessManager(const std::string password) const;
  bool IsAllowedToAcessSupervisor(const std::string password) const;

  types::PasswordAccessLevel GetPasswordAccessLevelByTxType(
      const TransactionType tx_type) const;
  types::PasswordAccessLevel GetVoidPasswordAccessLevel() const;
  types::PasswordAccessLevel GetSettlementPasswordAccessLevel() const;
};

}

#endif
