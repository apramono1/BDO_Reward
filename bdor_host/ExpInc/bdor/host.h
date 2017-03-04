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
#ifndef BDOR__HOST_H_
#define BDOR__HOST_H_

#include <vector>
#include <comms/client.h>

#include <bdor/transaction.h>
#include <bdor/batch.h>
#include <bdor/settlement_data.h>

#include <bdor/key_exchange.h>
#include <bdor/test_transaction.h>
#include <iso8583/apdu.h>

namespace bdor {

template<typename T>
using BuildRequestFunc = iso8583::Apdu (*)(const T& tx);

template<typename T>
using ReadAndValidateResponseFunc = bool (*)(const iso8583::Apdu& request_apdu,
                                    const std::vector<uint8_t>& data, T& tx);

class Host {
 public:
  typedef enum {
    COMPLETED,
    TRANSIENT_FAILURE,
    PERM_FAILURE
  } Status;

 public:
  Host()
      : comms_("") {
  }
  ~Host() {
  }

  bool PreConnect(const std::string& host_name);

  Status PerformKeyExchange(KeyExchange& key_exchange);

  Status AuthorizeRedemption(Transaction& tx);
  Status AuthorizeSale(Transaction& tx);
  Status AuthorizeSaleWithDccEnquiry(Transaction& tx);
  Status AuthorizeSaleWithDccAllowed(Transaction& tx);

  Status PerformTcUpload(Transaction& tx);
  Status PerformVoid(Transaction& tx);
  Status PerformBatchUpload(std::vector<Transaction>& transaction_list);
  Status PerformSettlement(SettlementData& settle_msg, bool after_batch_upload);
  Status SendReversal(Transaction& tx);
  Status AuthorizeRefund(Transaction& tx);

  Status PerformOfflineSale(Transaction& tx);
  Status PerformOfflineWithDccEnquiry(Transaction& tx);
  Status PerformOfflineWithDccAllowed(Transaction& tx);

  Status AuthorizePreAuth(Transaction& tx);
  Status AuthorizePreAuthWithDccEnquiry(Transaction& tx);
  Status AuthorizePreAuthWithDccAllowed(Transaction& tx);

  Status PerformPreAuthCancellation(Transaction& tx);

  Status AuthorizePreAuthCompletion(Transaction& tx);
  Status AuthorizePreAuthCompletionWithDccEnquiry(Transaction& tx);
  Status AuthorizePreAuthCompletionWithDccAllowed(Transaction& tx);

  Status SendTipAdjust(Transaction& tx);
  Status AuthorizeInstalmentSale(Transaction& tx);
  Status PerformTestTransaction(TestTransaction& test_tx);
  Status ValidateCard(Transaction& tx);

  bool WaitForConnection(uint32_t timeout);
  bool Disconnect();

 private:
  comms::Client comms_;

  Status SendMessage(const std::vector<uint8_t>& msg, const std::string tpdu);
  Status ReceiveMessage(std::vector<uint8_t>& msg);

  template<typename T>
  Host::Status PerformOnline(BuildRequestFunc<T> request_func,
                             ReadAndValidateResponseFunc<T> response_func,
                             T& tx);

  template<typename T>
  Host::Status PerformOnline(iso8583::Apdu request,
                             ReadAndValidateResponseFunc<T> response_func,
                             T& tx);
};

}
;

#endif
