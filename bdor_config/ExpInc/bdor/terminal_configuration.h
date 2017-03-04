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
#ifndef BDOR__TERMINAL_CONFIGURATION_H_
#define BDOR__TERMINAL_CONFIGURATION_H_

#include <types/currency.h>
#include <types/password_access_level.h>
#include <registry/registry.h>

namespace bdor {

class TerminalConfiguration {
 public:

  TerminalConfiguration(registry::Reader reader)
      :
        reader_(reader) {
  }

 private:
  registry::Reader reader_;

 public:

  types::Currency TerminalBaseCurrency();
  uint64_t MinAmount();
  uint64_t MaxAmount();

  // Receipt
  std::string ReceiptHeaderLine1();
  std::string ReceiptHeaderLine2();
  std::string ReceiptHeaderLine3();
  std::string ReceiptHeaderLine4();
  std::string ReceiptHeaderLine5();

  enum TipProcessingMode {
    TIP_DISABLED = 0,
    TIP_PROMPTED_AND_PRINTED = 1,
    TIP_PRINTED_ONLY = 2
  };
  TipProcessingMode TipProcessing();

  enum PreAuthClearMode {
    SETTLE_NO_PROMPT_NO_CLEAR = 0,
    SETTLE_NO_PROMPT_AND_CLEAR = 1,
    SETTLE_PROMPT_AND_CLEAR_IF_REQUESTED = 2
  };
  PreAuthClearMode PreAuthClearPolicy();
  uint64_t PreAuthMaxAge();  // in days
  uint64_t PreAuthCompletionAllowedVariance();  // in %

  std::string SuperAdminPassword();
  std::string SupervisorPassword();
  std::string ManagerPassword();

  types::PasswordAccessLevel GetSettlementPasswordLevel();
  types::PasswordAccessLevel GetOfflineSalePasswordLevel();
  types::PasswordAccessLevel GetRefundPasswordLevel();
  types::PasswordAccessLevel GetVoidPasswordLevel();

  bool TleEnabled();
  bool PiggybackEnabled();
  bool TcUploadEnabled() {
    return false;   // TODO: add config in TMS
  }
  bool TxSaleEnabled();
  bool TxPreAuthEnabled();
  bool TxOfflineSaleEnabled();
  bool TxRefundEnabled();
  bool TxVoidEnabled();
  bool TxVoidPreAuthEnabled();
  bool TxPreAuthCompletionOnlineEnabled();
  bool TxPreAuthCompletionOfflineEnabled();
  bool TxCashAdvanceEnabled();
  bool TxMotoSaleEnabled();
  bool TxMotoRefundEnabled();
  bool TxMotoPreauthEnabled();
  bool TxInstalmentSaleEnabled();
  bool TxSettleAllHostEnabled();
  // TODO: what's that? bool TxTxCardPayementEnabled();

  bool EmvEnabled();
  bool SaleContactlessEnabled();
  // TODO: is it needed: bool SeparateMotoHostEnabled();
  bool InstalmentDataToHostEnabled();

  std::string IdleMessage();
  std::string InstalmentReceiptDescription();

  std::string TerminalIdMasking();
  std::string MerchantIdMasking();

  uint64_t RefundMaxAmount();
  uint64_t RefundPasswordFloorLimit();
  bool RefundClessEnabled();

  uint64_t TipAdjustMaxNumber();  // should be 1
  uint64_t TipAdjustMaxVariance();  // in %

  uint64_t BatchMaxTxNumber();  // TODO: check if needed? should be related to memory available

  bool DccEnabled();

  enum DccProviderOption {
      FEXCO,
      PURE_COMMERCE,
      FINTRAX
    };
  DccProviderOption DccProvider();

  bool DccStaffIdInputEnabled();

  uint64_t OfflineSaleMaxAmount();

  unsigned int GetEventTimeout() {
    return 30 * 1000;
  }

  bool FailedReceiptEnabled() {
    return false;   // TODO: consider making it configurable
  }

  bool EnableEcrConnection() {
    return true;
  }

  int GetEcrServerPort() {
    return 4643;
  }
};

}

#endif
