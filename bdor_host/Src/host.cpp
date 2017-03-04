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
#include <bdor/host.h>

#include <string>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <algorithm>
#include <stdx/ctime>

#include "comms/status.h"
#include "idalloc/idalloc.h"
#include <message.h>
#include <iso8583/encoder.h>
#include <iso8583/apdu.h>
#include <iso8583/printer.h>

#include "card_validation_message.h"
#include "sale_message.h"
#include "instalment_sale_message.h"
#include "tc_upload_message.h"
#include "void_transaction_message.h"
#include "batch_upload_message.h"
#include "Key_exchange_message.h"
#include "settlement_message.h"
#include "reversal_message.h"
#include "refund_message.h"
#include "offline_sale_message.h"
#include "preauth_message.h"
#include "preauth_cancellation_message.h"
#include "preauth_completion_message.h"
#include "tip_adjust_message.h"
#include "test_transaction_message.h"

#include <utils/logger.h>
#include <utils/converter.h>
#include <tpcore/telium_manager.h>

using namespace bdor;

namespace {

const size_t kTpduSize = 5;

std::vector<uint8_t> AddTpdu(const std::vector<uint8_t>& msg,
                             const std::string& tpdu) {

  std::vector<uint8_t> output = utils::HexStringToBytes(tpdu);
  output.insert(output.end(), msg.begin(), msg.end());
  return output;
}
}

/* Host::Status Host::AuthorizeRedemption(Transaction& tx) {
  return PerformOnline(&BuildRedemptionRequest, &ReadAndValidateRedemptionResponse, tx);
} */

Host::Status Host::AuthorizeSale(Transaction& tx) {
  return PerformOnline(&BuildSaleRequest, &ReadAndValidateSaleResponse, tx);
}

Host::Status Host::AuthorizeSaleWithDccEnquiry(Transaction& tx) {
  return PerformOnline(&BuildSaleRequestWithDccEnquiry, &ReadAndValidateSaleResponseWithDccEnquiry, tx);
}

Host::Status Host::AuthorizeSaleWithDccAllowed(Transaction& tx) {
  return PerformOnline(&BuildSaleRequestWithDccAllowed, &ReadAndValidateSaleResponseWithDccAllowed, tx);
}

Host::Status Host::PerformTcUpload(Transaction& tx) {
  return PerformOnline(&BuildTcUploadRequest, &ReadAndValidateTcUploadResponse, tx);
}

Host::Status Host::PerformVoid(Transaction& tx) {
  return PerformOnline(&BuildVoidTransactionRequest,
                       &ReadAndValidateVoidTransactionResponse,
                       tx);
}

Host::Status Host::PerformBatchUpload(
    std::vector<Transaction>& transaction_list) {
  Status status;

  for (uint32_t i = 0; i < transaction_list.size(); i++) {
    status = PerformOnline(&BuildBatchUploadRequest, &ReadAndValidateBatchUploadResponse,
                           transaction_list[i]);

    if (status != COMPLETED) {
      logger::debug("FDMS - Batch Upload - Error during transaction uploaded");
      return status;
    }
    else
      logger::debug("FDMS - Batch Upload - Transaction successfully uploaded");

  }

  return COMPLETED;
}

Host::Status Host::PerformKeyExchange(KeyExchange& key_exchange) {

  return PerformOnline(&BuildKeyExchangeRequest,
                       &ReadAndValidateKeyExchangeResponse,
                       key_exchange);
}

Host::Status Host::PerformSettlement(SettlementData& settle_msg,
                                     bool after_batch_upload) {

  iso8583::Apdu settlement_request = BuildSettlementRequest(settle_msg,
                                                            after_batch_upload);

  return PerformOnline(settlement_request, &ReadAndValidateSettlementResponse, settle_msg);
}

Host::Status Host::SendReversal(Transaction& tx) {
  return PerformOnline(&BuildReversalRequest, &ReadAndValidateReversalResponse, tx);
}

Host::Status Host::AuthorizeRefund(Transaction& tx) {
  return PerformOnline(&BuildRefundRequest, &ReadAndValidateRefundResponse, tx);
}

Host::Status Host::PerformOfflineSale(Transaction& tx) {
  return PerformOnline(&BuildOfflineSaleRequest, &ReadAndValidateOfflineSaleResponse, tx);
}

Host::Status Host::PerformOfflineWithDccEnquiry(Transaction& tx) {
  return PerformOnline(&BuildOfflineRequestWithDccEnquiry, &ReadAndValidateOfflineResponseWithDccEnquiry, tx);
}

Host::Status Host::PerformOfflineWithDccAllowed(Transaction& tx) {
  return PerformOnline(&BuildOfflineRequestWithDccAllowed, &ReadAndValidateOfflineResponseWithDccAllowed, tx);
}

Host::Status Host::AuthorizePreAuth(Transaction& tx) {
  return PerformOnline(&BuildPreAuthRequest, &ReadAndValidatePreAuthResponse, tx);
}

Host::Status Host::AuthorizePreAuthWithDccEnquiry(Transaction& tx) {
  return PerformOnline(&BuildPreAuthRequestWithDccEnquiry, &ReadAndValidatePreAuthResponseWithDccEnquiry, tx);
}

Host::Status Host::AuthorizePreAuthWithDccAllowed(Transaction& tx) {
  return PerformOnline(&BuildPreAuthRequestWithDccAllowed, &ReadAndValidatePreAuthResponseWithDccAllowed, tx);
}

Host::Status Host::PerformPreAuthCancellation(Transaction& tx) {
  return PerformOnline(&BuildPreAuthCancellationRequest,
                       &ReadAndValidatePreAuthCancellationResponse,
                       tx);
}

Host::Status Host::AuthorizePreAuthCompletion(Transaction& tx) {
  return PerformOnline(&BuildPreAuthCompletionRequest,
                       &ReadAndValidatePreAuthCompletionResponse,
                       tx);
}

Host::Status Host::AuthorizePreAuthCompletionWithDccEnquiry(Transaction& tx) {
  return PerformOnline(&BuildPreAuthCompletionRequestWithDccEnquiry,
                       &ReadAndValidatePreAuthCompletionResponseWithDccEnquiry,
                       tx);
}

Host::Status Host::AuthorizePreAuthCompletionWithDccAllowed(Transaction& tx) {
  return PerformOnline(&BuildPreAuthCompletionRequestWithDccAllowed,
                       &ReadAndValidatePreAuthCompletionResponseWithDccAllowed,
                       tx);
}

Host::Status Host::SendTipAdjust(Transaction& tx) {
  return PerformOnline(&BuildTipAdjustRequest, &ReadAndValidateTipAdjustResponse, tx);
}

Host::Status Host::AuthorizeInstalmentSale(Transaction& tx) {
  return PerformOnline(&BuildInstalmentSaleRequest, &ReadAndValidateInstalmentSaleResponse,
                       tx);
}

Host::Status Host::PerformTestTransaction(TestTransaction& test_tx) {
  return PerformOnline(&BuildTestTransactionRequest,
                       &ReadAndValidateTestTransactionResponse,
                       test_tx);
}

Host::Status Host::SendMessage(const std::vector<uint8_t>& msg,
                               const std::string tpdu) {
  uint64_t byte_sent;
  comms::CommsStatus comms_status;

  std::vector<uint8_t> msg_to_send = AddTpdu(msg, tpdu);

  logger::xdebug(msg_to_send.data(), msg_to_send.size());

  comms_status = comms_.Send(msg_to_send, &byte_sent);
  if (comms_status != comms::COMMS_OK) {
    logger::error("FDMS - Error when sending message");
    comms_.Disconnect();
    return TRANSIENT_FAILURE;
  }

  if (byte_sent != msg_to_send.size()) {
    logger::error("FDMS - Message not fully sent");
    comms_.Disconnect();
    return TRANSIENT_FAILURE;
  }

  return COMPLETED;
}

Host::Status Host::ReceiveMessage(std::vector<uint8_t>& msg) {

  size_t max_bytes = 9999;
  comms::CommsStatus comms_status = comms_.Receive(msg, max_bytes);

  if (comms_status != comms::COMMS_OK) {
    comms_.Disconnect();
    return TRANSIENT_FAILURE;;
  }

  logger::xdebug(msg.data(), msg.size());

  if (msg.size() < kTpduSize) {
    comms_.Disconnect();
    return TRANSIENT_FAILURE;
  }

// strip TPDU
  msg.erase(msg.begin(), msg.begin() + kTpduSize);
  return COMPLETED;
}

Host::Status Host::ValidateCard(Transaction& tx) {
  return PerformOnline(&BuildCardValidationRequest, &ReadAndValidateCardValidationResponse, tx);
}

bool Host::PreConnect(const std::string& host_name) {
  comms_ = comms::Client(host_name.c_str());
  comms::CommsStatus status = comms_.PreConnect();
  if (status == comms::COMMS_OK)
    return true;
  return false;
}

bool Host::Disconnect() {
  comms::CommsStatus status = comms_.Disconnect();
  if (status == comms::COMMS_OK)
    return true;
  return false;
}

bool Host::WaitForConnection(uint32_t timeout) {
  return comms_.WaitConnected(timeout) == comms::COMMS_CONNECTED;
}

template<typename T>
Host::Status Host::PerformOnline(BuildRequestFunc<T> request_func,
                                 ReadAndValidateResponseFunc<T> response_func,
                                 T& tx) {
  return PerformOnline(request_func(tx), response_func, tx);
}

template<typename T>
Host::Status Host::PerformOnline(iso8583::Apdu request,
                                 ReadAndValidateResponseFunc<T> response_func,
                                 T& tx) {
  uint32_t timeout = 10000;

  if (comms_.WaitConnected(timeout) != comms::COMMS_CONNECTED) {
    comms_.Disconnect();
    return TRANSIENT_FAILURE;
  }

  logger::debug(iso8583::Print(request).c_str());
  if (SendMessage(request.text, tx.tpdu) != COMPLETED)
    return TRANSIENT_FAILURE;

  std::vector<uint8_t> msg_response_v;
  if (ReceiveMessage(msg_response_v) != COMPLETED)
    return TRANSIENT_FAILURE;

  if (!response_func(request, msg_response_v, tx))
    return Status::PERM_FAILURE;

  return COMPLETED;
}
