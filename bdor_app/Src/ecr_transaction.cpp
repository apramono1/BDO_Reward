/*
 ------------------------------------------------------------------------------
 INGENICO Technical Software Department
 ------------------------------------------------------------------------------
 Copyright (c) 2016 INGENICO S.A.
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
#include "ecr_manager.h"
#include <utils/logger.h>

namespace bdor {

using namespace posgate;

namespace {

Message GetTxInvalidResponse() {

  auto resp = GetBaseResponse(EiCommandType::eicFinancial,
                              EiResult::eiError,
                              "NA",
                              "INVALID TRANSACTION");
  SetEftApprove(false, resp);
  return resp;
}

Message GetTxNotSupportedResponse(EftTxnType eft_txn_type,
                                  const std::string& eft_txn_id) {

  auto resp = GetBaseResponse(EiCommandType::eicFinancial,
                              EiResult::eiError,
                              "NA",
                              "TRANSACTION NOT SUPPORTED");
  SetEftTxnType(eft_txn_type, resp);
  SetEftTxnId(eft_txn_id, resp);
  SetEftApprove(false, resp);
  return resp;
}

Message GetTxApprovedResponse(EftTxnType eft_txn_type,
                              const std::string& eft_txn_id,
                              const Transaction& tx) {

  auto resp = GetBaseResponse(EiCommandType::eicFinancial,
                              EiResult::eiSuccess,
                              tx.response_code,
                              "APPROVED");

  SetEftTxnType(eft_txn_type, resp);
  SetEftTxnId(eft_txn_id, resp);
  SetEftAuthCode(tx.auth_id_response, resp);
  SetEftRrn(tx.rrn, resp);
  SetEftStan(tx.stan, resp);
  SetEftCaid(tx.mid, resp);
  SetEftCatid(tx.tid, resp);
  SetEftApprove(true, resp);
  return resp;
}

Message GetTxFailedResponse(EftTxnType eft_txn_type,
                            const std::string& eft_txn_id,
                            Transaction& tx) {

  auto resp = GetBaseResponse(EiCommandType::eicFinancial,
                              EiResult::eiError,
                              tx.response_code,
                              "TRANSACTION FAILED");
  SetEftTxnType(eft_txn_type, resp);
  SetEftTxnId(eft_txn_id, resp);
  SetEftApprove(false, resp);
  return resp;
}

}

Message EcrManager::ProcessTransaction(const Message& message) {

  auto eft_txn_type = GetEftTxnType(message);
  auto eft_txn_id = GetEftTxnId(message);

  if (!eft_txn_type.has_value() || !eft_txn_id.has_value())
    return GetTxInvalidResponse();

  int initial_header = IsHeaderDisplayed();
  DisplayHeader(OFF);

  std::string currency_name = app_setting_.managed_settings_->terminal_config
      .TerminalBaseCurrency().GetName();

  std::unique_ptr<Transaction> tx_uptr = nullptr;

  switch (*eft_txn_type) {
    case EftTxnType::TT_PURCHASE: {
      auto eft_amount_purchase = GetEftAmountPurchase(message);
      if (!eft_amount_purchase.has_value())
        return GetTxInvalidResponse();
      const types::Amount amount(currency_name, *eft_amount_purchase);
      tx_uptr = ProcessSale(amount);
      break;
    }
    case EftTxnType::TT_REFUND: {
      auto eft_amount_refund = GetEftAmountRefund(message);
      if (!eft_amount_refund.has_value())
        return GetTxInvalidResponse();
      const types::Amount amount(currency_name, *eft_amount_refund);
      tx_uptr = ProcessRefund(amount);
      break;
    }
    case EftTxnType::TT_PRE_AUTH: {
      auto eft_amount_purchase = GetEftAmountPurchase(message);
      if (!eft_amount_purchase.has_value())
        return GetTxInvalidResponse();
      const types::Amount amount(currency_name, *eft_amount_purchase);
      tx_uptr = ProcessPreauth(amount);
      break;
    }
    default:
      break;
  }

  if (initial_header == 1)
    DisplayHeader(ON);

  if (tx_uptr) {
    if (tx_uptr->transaction_status == TransactionStatus::APPROVED
        || tx_uptr->transaction_status == TransactionStatus::TO_ADVISE)
      return GetTxApprovedResponse(*eft_txn_type,
                                   *eft_txn_id,
                                   *tx_uptr);
    else
      return GetTxFailedResponse(*eft_txn_type,
                                 *eft_txn_id,
                                 *tx_uptr);
  }
  return GetTxNotSupportedResponse(*eft_txn_type, *eft_txn_id);
}

std::unique_ptr<Transaction> EcrManager::ProcessSale(
    const types::Amount& amount) const {
  return acquirer_.StartSale(amount);
}

std::unique_ptr<Transaction> EcrManager::ProcessPreauth(
    const types::Amount& amount) const {
  return acquirer_.StartPreauth(amount);
}

int EcrManager::ProcessOfflineSale(const types::Amount& amount) {
  return 0;
}

std::unique_ptr<Transaction> EcrManager::ProcessRefund(
    const types::Amount& amount) const {
  return acquirer_.StartRefund(amount);
}

int EcrManager::ProcessCashAdvance(const types::Amount& amount) {
  return 0;
}

int EcrManager::ProcessMotoSale(const types::Amount& amount) {
  return 0;
}

int EcrManager::ProcessMotoRefund(const types::Amount& amount) {
  return 0;
}

int EcrManager::ProcessTipAdjust(const types::Amount& amount) {
  return 0;
}

int EcrManager::ProcessVoid(const std::string& invoice_number) {
  return 0;
}

int EcrManager::ProcessInstalmentSale(const types::Amount& amount) {
  return 0;
}

int EcrManager::ProcessAuthorizationCompletion(const types::Amount& amount) {
  return 0;
}

int EcrManager::GetLastTransaction(utils::bytes&) {
  return 0;
}

int EcrManager::CancelTransaction() {
  return 0;
}

}
