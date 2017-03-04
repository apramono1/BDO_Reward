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

#include <bdor/password.h>

namespace bdor {

Password::Password(ApplicationSettings& app_setting, Ui& ui)
    : app_settings_(app_setting),
      ui_(ui) {
}

bool Password::PerformPasswordAccess(const bool is_void_proccessing) const {
  if (is_void_proccessing)
    return PerformPasswordAccess(GetVoidPasswordAccessLevel());
  else
    return true;
}

bool Password::PerformPasswordAccess(const TransactionType tx_type) const {
  return PerformPasswordAccess(GetPasswordAccessLevelByTxType(tx_type));
}

bool Password::PerformManagerPasswordAccess() const {
  return PerformPasswordAccess(types::PasswordAccessLevel::MANAGER);
}

bool Password::PerformSuperAdminPasswordAccess() const {
  return PerformPasswordAccess(types::PasswordAccessLevel::SUPER_ADMIN);
}

bool Password::PerformSettlementPasswordAccess() const {
  return PerformPasswordAccess(GetSettlementPasswordAccessLevel());
}

bool Password::PerformPasswordAccess(
    types::PasswordAccessLevel pw_level) const {
  switch (pw_level) {
    case types::PasswordAccessLevel::SUPER_ADMIN:
      return PerformSuperAdminAccess();

    case types::PasswordAccessLevel::MANAGER:
      return PerformManagerAccess();

    case types::PasswordAccessLevel::SUPERVISOR:
      return PerformSupervisorAccess();

    default:
      return true;
  }
}

bool Password::PerformSuperAdminAccess() const {

  std::string password;
  while (ui_.RequestPassword("PLS ENTER PASSWORD:", password)) {
    if (IsAllowedToAcessSuperAdmin(password)) {
      return true;
    } else {
      ui_.MessageInvalidPassword();
    }
  }

  return false;
}

bool Password::PerformManagerAccess() const {
  std::string password;
  while (ui_.RequestPassword("PLS ENTER PASSWORD:", password)) {
    if (IsAllowedToAcessManager(password)) {
      return true;
    } else {
      ui_.MessageInvalidPassword();
    }
  }

  return false;
}

bool Password::PerformSupervisorAccess() const {
  std::string password;
  while (ui_.RequestPassword("PLS ENTER PASSWORD:", password)) {
    if (IsAllowedToAcessSupervisor(password)) {
      return true;
    } else {
      ui_.MessageInvalidPassword();
    }
  }

  return false;
}

bool Password::IsAllowedToAcessSuperAdmin(const std::string password) const {
  if (IsSuperAdminPassword(password))
    return true;

  return false;
}

bool Password::IsAllowedToAcessManager(const std::string password) const {
  if (IsSuperAdminPassword(password))
    return true;
  else if (IsManagerPassword(password))
    return true;
  else
    return false;
}

bool Password::IsAllowedToAcessSupervisor(const std::string password) const {
  if (IsSuperAdminPassword(password))
    return true;
  else if (IsManagerPassword(password))
    return true;
  else if (IsSupervisorPassword(password))
    return true;
  else
    return false;
}

bool Password::IsSuperAdminPassword(const std::string password) const {
  return password.compare(
      app_settings_.managed_settings_->terminal_config.SuperAdminPassword())
      == 0;
}

bool Password::IsManagerPassword(const std::string password) const {
  return password.compare(
      app_settings_.managed_settings_->terminal_config.ManagerPassword()) == 0;
}

bool Password::IsSupervisorPassword(const std::string password) const {
  return password.compare(
      app_settings_.managed_settings_->terminal_config.SupervisorPassword())
      == 0;
}

types::PasswordAccessLevel Password::GetPasswordAccessLevelByTxType(
    const TransactionType tx_type) const {
  switch (tx_type) {
    case TransactionType::OFFLINE_SALE:
      case TransactionType::PREAUTH_COMPLETION_OFFLINE:
      return app_settings_.managed_settings_->terminal_config
          .GetOfflineSalePasswordLevel();

    case TransactionType::REFUND:
      return app_settings_.managed_settings_->terminal_config
          .GetRefundPasswordLevel();

    case TransactionType::PREAUTH_CANCELLATION:
      return app_settings_.managed_settings_->terminal_config
          .GetVoidPasswordLevel();
    default:
      return types::PasswordAccessLevel::NONE;
  }
}

types::PasswordAccessLevel Password::GetVoidPasswordAccessLevel() const {
  return app_settings_.managed_settings_->terminal_config
      .GetVoidPasswordLevel();
}

types::PasswordAccessLevel Password::GetSettlementPasswordAccessLevel() const {
  return app_settings_.managed_settings_->terminal_config
      .GetSettlementPasswordLevel();
}

}
