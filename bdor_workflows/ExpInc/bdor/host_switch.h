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
#ifndef BDOR__HOST_SWITCH_H_
#define BDOR__HOST_SWITCH_H_

#include <vector>
#include <stdx/optional>
#include <bdor/application_settings.h>
#include <bdor/host.h>
#include <amex/amex_host.h>

#include <bdor/transaction.h>

namespace bdor {

class HostSwitch {
 public:
  enum class Status {
    COMPLETED,
    TRANSIENT_FAILURE,
    PERM_FAILURE
  };

 public:
  HostSwitch(ApplicationSettings& app_setting)
      : app_settings_(app_setting) {
  }
  ~HostSwitch() {
  }

  bool PreConnect(unsigned int host_index);

  Status AuthorizeRedemption(Transaction& tx);
  Status AuthorizeSale(Transaction& tx);
  Status AuthorizeSaleWithDccEnquiry(Transaction& tx);
  Status AuthorizeSaleWithDccAllowed(Transaction& tx);

  Status PerformTcUpload(Transaction& tx);
  Status PerformVoid(Transaction& tx);
  Status PerformBatchUpload(std::vector<Transaction>& transaction_list);
  Status SendReversal(Transaction& tx);
  Status AuthorizeRefund(Transaction& tx);

  Status PerformOfflineSale(Transaction& tx);
  Status PerformOfflineWithDccEnquiry(Transaction& tx);
  Status PerformOfflineWithDccAllowed(Transaction& tx);

  Status AuthorizePreAuth(Transaction& tx);
  Status AuthorizePreAuthWithDccEnquiry(Transaction& tx);
  Status AuthorizePreAuthWithDccAllowed(Transaction& tx);

  Status PerformTipAdjust(Transaction& tx);

  Status AuthorizePreAuthCompletion(Transaction& tx);
  Status AuthorizePreAuthCompletionWithDccEnquiry(Transaction& tx);
  Status AuthorizePreAuthCompletionWithDccAllowed(Transaction& tx);

  Status PerformSettlement(SettlementData& settle_msg, bool after_batch_upload);

  Status PerformFdmsPreAuthCancellation(Transaction& tx);
  Status AuthorizeFdmsInstalmentSale(Transaction& tx);
  Status PerformFdmsKeyExchange(KeyExchange& key_exchange);
  Status PerformFdmsSettlement(SettlementData& settle_msg, bool after_batch_upload);
  Status PerformFdmsTestTransaction(TestTransaction& test_tx);
  Status ValidateCard (Transaction& tx);

  bool isAmex();
  bool WaitForConnection();
  bool Disconnect();

 private:
  ApplicationSettings& app_settings_;
  Host host_bdor_;
  std::unique_ptr<amex::AmexHost> host_amex_;
  stdx::optional<HostProtocol> current_host_protocol_;

  amex::AmexHost& GetAmexHost();  // for lazy loading

  template<typename T1, typename T2>
  Status PerformActionWithTx(T1 f1, T2 f2, Transaction& tx);
  Status PerformAmexBatchUpload(std::vector<Transaction>& transaction_list);
};

using HostAuthorizedFunc = HostSwitch::Status (HostSwitch::*)(Transaction& tx);

}

#endif
