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
#include <bdor/host_switch.h>
#include <functional>
#include <utils/get_default.h>
#include <amex/amex_host.h>
#include "app_counter.h"

namespace bdor {

namespace {

HostSwitch::Status ConvertStatus(Host::Status status) {
  static std::map<Host::Status, HostSwitch::Status> map =
      {
          { Host::Status::COMPLETED, HostSwitch::Status::COMPLETED },
          { Host::Status::TRANSIENT_FAILURE,
              HostSwitch::Status::TRANSIENT_FAILURE },
          { Host::Status::PERM_FAILURE, HostSwitch::Status::PERM_FAILURE },
      };
  return utils::GetDefault(map, status, HostSwitch::Status::PERM_FAILURE);
}

HostSwitch::Status ConvertStatus(amex::AmexHost::Status status) {
  static std::map<amex::AmexHost::Status, HostSwitch::Status> map =
      {
          { amex::AmexHost::Status::COMPLETED, HostSwitch::Status::COMPLETED },
          { amex::AmexHost::Status::TRANSIENT_FAILURE,
              HostSwitch::Status::TRANSIENT_FAILURE },
          { amex::AmexHost::Status::PERM_FAILURE,
              HostSwitch::Status::PERM_FAILURE },
      };
  return utils::GetDefault(map, status, HostSwitch::Status::PERM_FAILURE);
}

amex::AmexTransactionType ConvertTxType(TransactionType tx_type) {
  amex::AmexTransactionType amex_tx_type = amex::AmexTransactionType::SALE;
  switch (tx_type) {
    case TransactionType::REFUND:
      amex_tx_type = amex::AmexTransactionType::REFUND;
      break;

    case TransactionType::PREAUTH:
      amex_tx_type = amex::AmexTransactionType::PREAUTH;
      break;

    case TransactionType::PREAUTH_COMPLETION_ONLINE:
      amex_tx_type = amex::AmexTransactionType::PREAUTH_COMPLETION_ONLINE;
      break;

    case TransactionType::OFFLINE_SALE:
      amex_tx_type = amex::AmexTransactionType::OFFLINE_SALE;
      break;

    case TransactionType::SALE:
      default:     // TODO: other types
      amex_tx_type = amex::AmexTransactionType::SALE;
      break;
  }

  return amex_tx_type;
}

amex::AmexTransaction BuildAmexTransactionFromTransaction(
    const Transaction tx) {
  amex::AmexTransaction amex_tx;

  amex_tx.pan = tx.pan;
  amex_tx.processing_code = tx.processing_code;
  amex_tx.amount = tx.amount;

  amex_tx.stan = tx.stan;
  amex_tx.expiration_date = tx.expiration_date;
  amex_tx.pos_entry_mode = tx.pos_entry_mode;
  amex_tx.nii = tx.nii;
  amex_tx.pos_condition_code = tx.pos_condition_code;

  if (tx.track2_equivalent_data) {
    amex_tx.track2 = tx.track2_equivalent_data->GetData();
  }

  amex_tx.tid = tx.tid;
  amex_tx.mid = tx.mid;

  if (!tx.track1_data.empty()) {
    amex_tx.track1_data = tx.track1_data;
  }

  if (!tx.pin_data.empty()) {
    amex_tx.pin_block = tx.pin_data;
  }

  amex_tx.additional_amount = tx.secondary_amount;
  amex_tx.icc_data = tx.icc_data;
  amex_tx.transaction_type = ConvertTxType(tx.transaction_type);
  amex_tx.tpdu = tx.tpdu;

  amex_tx.tx_datetime = tx.tx_datetime;
  amex_tx.rrn = tx.rrn;
  amex_tx.auth_id_response = tx.auth_id_response;
  amex_tx.response_code = tx.response_code;
  amex_tx.issuer_emv_response = tx.issuer_emv_response;

  amex_tx.invoice_number = tx.invoice_num;
  amex_tx.amex_4dbc = tx.cvv;
  amex_tx.preauth_amount = tx.preauth_amount;

  return amex_tx;
}

amex::AmexSettlementData BuildAmexSettlementData(
    const SettlementData & settle_data)
    {
  amex::AmexSettlementData amex_settle;
  amex_settle.tx_datetime = settle_data.tx_datetime;
  amex_settle.batch_number = settle_data.batch_number;
  amex_settle.nii = settle_data.nii;
  amex_settle.tpdu = settle_data.tpdu;
  amex_settle.tid = settle_data.tid;
  amex_settle.mid = settle_data.mid;
  amex_settle.rrn = settle_data.rrn;
  amex_settle.stan = settle_data.stan;

  amex_settle.batch_summary.credit_total.count = settle_data.batch_summary
      .refunds_total.count;
  amex_settle.batch_summary.credit_total.total = settle_data.batch_summary
      .refunds_total.total;
  amex_settle.batch_summary.debit_total.count = settle_data.batch_summary
      .sales_total.count;
  amex_settle.batch_summary.debit_total.total = settle_data.batch_summary
      .sales_total.total;
  amex_settle.batch_summary.currency = settle_data.batch_summary.currency;

  amex_settle.invoice_num = GetNextInvoiceNo();  //TODO: check / clarify amex requirement

  return amex_settle;
}

void FillSettlementDataWithAmexSettlementData(
    SettlementData & settle_data,
    const amex::AmexSettlementData amex_settle_data) {
  settle_data.processing_code = amex_settle_data.processing_code;
  settle_data.tx_datetime = amex_settle_data.tx_datetime;
  settle_data.rrn = amex_settle_data.rrn;
  settle_data.response_code = amex_settle_data.response_code;
  settle_data.stan = amex_settle_data.stan;
  settle_data.nii = amex_settle_data.nii;
  settle_data.tid = amex_settle_data.tid;
}

void FillTransactionWithAmexTransactionData(
    Transaction& tx, const amex::AmexTransaction amex_tx) {

  tx.processing_code = amex_tx.processing_code;
  tx.tx_datetime = amex_tx.tx_datetime;  // TODO: check that. Shall we have a host datetime somewhere?
  tx.rrn = amex_tx.rrn;
  tx.auth_id_response = amex_tx.auth_id_response;
  tx.response_code = amex_tx.response_code;
  tx.issuer_emv_response = amex_tx.issuer_emv_response;
}

}

amex::AmexHost& HostSwitch::GetAmexHost() {
  if (!host_amex_) {
    host_amex_ = stdx::make_unique<amex::AmexHost>(
        app_settings_.managed_settings_->amex_config.Origin(),
        app_settings_.managed_settings_->amex_config.CountryCode(),
        app_settings_.managed_settings_->amex_config.Region(),
        app_settings_.managed_settings_->amex_config.RoutingIndicator());
  }

  return *host_amex_;
}

template<typename T1, typename T2>
HostSwitch::Status HostSwitch::PerformActionWithTx(T1 f1, T2 f2,
                                                   Transaction& tx) {
  using namespace std::placeholders;

  if (!current_host_protocol_) {
    return HostSwitch::Status::PERM_FAILURE;
  }

  if (*current_host_protocol_ == HostProtocol::FDMS_BASE24) {
    auto f = std::bind < Host::Status > (f1, host_bdor_, _1);
    Host::Status status = f(tx);
    return ConvertStatus(status);

  } else if (*current_host_protocol_ == HostProtocol::AMEX_DIRECT) {
    auto f = std::bind < amex::AmexHost::Status > (f2, GetAmexHost(), _1);
    amex::AmexTransaction amex_tx = BuildAmexTransactionFromTransaction(tx);
    amex::AmexHost::Status status = f(amex_tx);
    FillTransactionWithAmexTransactionData(tx, amex_tx);
    return ConvertStatus(status);
  }

  current_host_protocol_ = stdx::nullopt;
  return HostSwitch::Status::PERM_FAILURE;
}

bool HostSwitch::PreConnect(unsigned int host_index) {
  stdx::optional<HostDefinition> host_config = app_settings_.managed_settings_
      ->GetHostDefinition(host_index);
  if (!host_config) {
    return false;
  }

  bool ret;
  current_host_protocol_ = host_config->host_protocol;
  if (*current_host_protocol_ == HostProtocol::FDMS_BASE24) {
    ret = host_bdor_.PreConnect(host_config->comms_host_name);

  } else if (*current_host_protocol_ == HostProtocol::AMEX_DIRECT) {
    ret = GetAmexHost().PreConnect(host_config->comms_host_name);
  }

  if (!ret) {
    current_host_protocol_ = stdx::nullopt;
  }

  return ret;
}

HostSwitch::Status HostSwitch::AuthorizeSale(Transaction& tx) {
  return PerformActionWithTx(&Host::AuthorizeSale,
                             &amex::AmexHost::AuthorizeSale,
                             tx);
}

HostSwitch::Status HostSwitch::AuthorizeSaleWithDccEnquiry(Transaction& tx) {
  return PerformActionWithTx(&Host::AuthorizeSaleWithDccEnquiry,
                             &amex::AmexHost::AuthorizeSale,
                             tx);
}

HostSwitch::Status HostSwitch::AuthorizeSaleWithDccAllowed(Transaction& tx) {
  return PerformActionWithTx(&Host::AuthorizeSaleWithDccAllowed,
                             &amex::AmexHost::AuthorizeSale,
                             tx);
}

HostSwitch::Status HostSwitch::PerformTcUpload(Transaction& tx) {
  return PerformActionWithTx(&Host::PerformTcUpload,
                             &amex::AmexHost::PerformTcUpload,
                             tx);
}

HostSwitch::Status HostSwitch::PerformVoid(Transaction& tx) {
  return PerformActionWithTx(&Host::PerformVoid,
                             &amex::AmexHost::PerformVoid,
                             tx);
}

HostSwitch::Status HostSwitch::PerformAmexBatchUpload(
    std::vector<Transaction>& transaction_list)
    {
  HostSwitch::Status output = HostSwitch::Status::PERM_FAILURE;
  std::vector<amex::AmexTransaction> amex_tx_list;
  std::transform(transaction_list.begin(),
                 transaction_list.end(),
                 std::back_inserter(amex_tx_list),
                 BuildAmexTransactionFromTransaction);

  amex::AmexHost::Status status;

  for (auto tx : amex_tx_list)
  {
    status = GetAmexHost().PerformBatchUpload(tx, GetNextStanNo());
    if (status != amex::AmexHost::Status::COMPLETED)
      break;
  }

  output = ConvertStatus(status);

  return output;
}

HostSwitch::Status HostSwitch::PerformBatchUpload(
    std::vector<Transaction>& transaction_list) {

  HostSwitch::Status output = HostSwitch::Status::PERM_FAILURE;

  if (!current_host_protocol_) {
    return output;
  }

  if (*current_host_protocol_ == HostProtocol::FDMS_BASE24) {
    Host::Status status = host_bdor_.PerformBatchUpload(transaction_list);
    output = ConvertStatus(status);

  } else if (*current_host_protocol_ == HostProtocol::AMEX_DIRECT) {
    output = PerformAmexBatchUpload(transaction_list);
  }

  return output;
}

HostSwitch::Status HostSwitch::SendReversal(Transaction& tx) {
  return PerformActionWithTx(&Host::SendReversal,
                             &amex::AmexHost::SendReversal,
                             tx);
}

HostSwitch::Status HostSwitch::AuthorizeRefund(Transaction& tx) {
  return PerformActionWithTx(&Host::AuthorizeRefund,
                             &amex::AmexHost::AuthorizeRefund,
                             tx);
}

HostSwitch::Status HostSwitch::PerformOfflineSale(Transaction& tx) {
  return PerformActionWithTx(&Host::PerformOfflineSale,
                             &amex::AmexHost::SendOfflineSale,
                             tx);
}

HostSwitch::Status HostSwitch::PerformOfflineWithDccEnquiry(Transaction& tx) {
  return PerformActionWithTx(&Host::PerformOfflineWithDccEnquiry,
                             &amex::AmexHost::SendOfflineSale,
                             tx);
}

HostSwitch::Status HostSwitch::PerformOfflineWithDccAllowed(Transaction& tx) {
  return PerformActionWithTx(&Host::PerformOfflineWithDccAllowed,
                             &amex::AmexHost::SendOfflineSale,
                             tx);
}

HostSwitch::Status HostSwitch::AuthorizePreAuth(Transaction& tx) {
  return PerformActionWithTx(&Host::AuthorizePreAuth,
                             &amex::AmexHost::AuthorizePreAuth,
                             tx);
}

HostSwitch::Status HostSwitch::AuthorizePreAuthWithDccEnquiry(Transaction& tx) {
  return PerformActionWithTx(&Host::AuthorizePreAuthWithDccEnquiry,
                             &amex::AmexHost::AuthorizePreAuth,
                             tx);
}

HostSwitch::Status HostSwitch::AuthorizePreAuthWithDccAllowed(Transaction& tx) {
  return PerformActionWithTx(&Host::AuthorizePreAuthWithDccAllowed,
                             &amex::AmexHost::AuthorizePreAuth,
                             tx);
}

HostSwitch::Status HostSwitch::PerformTipAdjust(Transaction& tx) {
  return PerformActionWithTx(&Host::SendTipAdjust,
                             &amex::AmexHost::SendTipAdjust,
                             tx);
}

HostSwitch::Status HostSwitch::AuthorizePreAuthCompletion(Transaction& tx) {
  return PerformActionWithTx(&Host::AuthorizePreAuthCompletion,
                             &amex::AmexHost::AuthorizePreAuthCompletion,
                             tx);
}

HostSwitch::Status HostSwitch::AuthorizePreAuthCompletionWithDccEnquiry(Transaction& tx) {
  return PerformActionWithTx(&Host::AuthorizePreAuthCompletionWithDccEnquiry,
                             &amex::AmexHost::AuthorizePreAuthCompletion,
                             tx);
}

HostSwitch::Status HostSwitch::AuthorizePreAuthCompletionWithDccAllowed(Transaction& tx) {
  return PerformActionWithTx(&Host::AuthorizePreAuthCompletionWithDccAllowed,
                             &amex::AmexHost::AuthorizePreAuthCompletion,
                             tx);
}

HostSwitch::Status HostSwitch::PerformFdmsPreAuthCancellation(Transaction& tx) {
  if (!current_host_protocol_
      || *current_host_protocol_ != HostProtocol::FDMS_BASE24) {
    return HostSwitch::Status::PERM_FAILURE;
  }

  Host::Status status = host_bdor_.PerformPreAuthCancellation(tx);
  return ConvertStatus(status);
}

HostSwitch::Status HostSwitch::AuthorizeFdmsInstalmentSale(Transaction& tx) {
  if (!current_host_protocol_
      || *current_host_protocol_ != HostProtocol::FDMS_BASE24) {
    return HostSwitch::Status::PERM_FAILURE;
  }

  Host::Status status = host_bdor_.AuthorizeInstalmentSale(tx);
  return ConvertStatus(status);
}

HostSwitch::Status HostSwitch::PerformFdmsKeyExchange(
    KeyExchange& key_exchange) {

  if (!current_host_protocol_
      || *current_host_protocol_ != HostProtocol::FDMS_BASE24) {
    return HostSwitch::Status::PERM_FAILURE;
  }

  Host::Status status = host_bdor_.PerformKeyExchange(key_exchange);
  return ConvertStatus(status);
}

HostSwitch::Status HostSwitch::PerformFdmsSettlement(SettlementData& settle_msg,
                                                     bool after_batch_upload) {
  if (!current_host_protocol_
      || *current_host_protocol_ != HostProtocol::FDMS_BASE24) {
    return HostSwitch::Status::PERM_FAILURE;
  }

  Host::Status status = host_bdor_.PerformSettlement(settle_msg,
                                                     after_batch_upload);
  return ConvertStatus(status);
}

HostSwitch::Status HostSwitch::PerformSettlement(SettlementData& settle_msg,
                                                 bool after_batch_upload) {

  HostSwitch::Status output = HostSwitch::Status::PERM_FAILURE;

  if (!current_host_protocol_) {
    return output;
  }

  if (*current_host_protocol_ == HostProtocol::FDMS_BASE24)
      {
    Host::Status status = host_bdor_.PerformSettlement(settle_msg,
                                                       after_batch_upload);
    output = ConvertStatus(status);
  }
  else if (*current_host_protocol_ == HostProtocol::AMEX_DIRECT)
      {
    amex::AmexSettlementData amex_settle;
    amex_settle = BuildAmexSettlementData(settle_msg);
    amex::AmexHost::Status status_amex = GetAmexHost().PerformSettlement(
        amex_settle, after_batch_upload);
    FillSettlementDataWithAmexSettlementData(settle_msg, amex_settle);
    output = ConvertStatus(status_amex);
  }
  return output;
}

HostSwitch::Status HostSwitch::PerformFdmsTestTransaction(
    TestTransaction & test_tx) {
  if (!current_host_protocol_
      || *current_host_protocol_ != HostProtocol::FDMS_BASE24) {
    return HostSwitch::Status::PERM_FAILURE;
  }

  Host::Status status = host_bdor_.PerformTestTransaction(test_tx);
  return ConvertStatus(status);
}

HostSwitch::Status HostSwitch::ValidateCard(Transaction& tx) {
  return PerformActionWithTx(&Host::ValidateCard,
                             &amex::AmexHost::AuthorizePreAuth,
                             tx);
}

bool HostSwitch::WaitForConnection() {
  if (!current_host_protocol_) {
    return false;
  }

  uint32_t timeout = 30000;  // TODO: don't hardcode

  bool ret = true;
  if (*current_host_protocol_ == HostProtocol::FDMS_BASE24) {
    ret = host_bdor_.WaitForConnection(timeout);

  } else if (*current_host_protocol_ == HostProtocol::AMEX_DIRECT) {
    ret = GetAmexHost().WaitForConnection(timeout);
  }

  if (!ret) {
    current_host_protocol_ = stdx::nullopt;
  }
  return ret;
}

bool HostSwitch::Disconnect() {
  if (!current_host_protocol_) {
    return true;
  }

  bool ret = true;
  if (*current_host_protocol_ == HostProtocol::FDMS_BASE24) {
    ret = host_bdor_.Disconnect();

  } else if (*current_host_protocol_ == HostProtocol::AMEX_DIRECT) {
    ret = GetAmexHost().Disconnect();
  }

  current_host_protocol_ = stdx::nullopt;
  return ret;
}

bool HostSwitch::isAmex() {

  if (*current_host_protocol_ == HostProtocol::AMEX_DIRECT)
    return true;
  else
    return false;
}
}

