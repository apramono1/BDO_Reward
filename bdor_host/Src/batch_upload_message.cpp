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

#include <batch_upload_message.h>
#include "protocol.h"
#include "field_utils.h"
#include <iso8583/field_types.h>
#include <iso8583/encoder.h>
#include <utils/converter.h>
#include <utils/logger.h>
#include <iso8583/printer.h>
#include "apdu_utils.h"
#include <tpcore/calendar.h>
#include "dcc_message.h"

using namespace types;

namespace bdor {

// PRIVATE DECLARATIONS
static void SetOriginalTransactionData(const TransactionType& transaction_type,
                                       const uint32_t& stan,
                                       const std::string& rrn,
                                       iso8583::Apdu& apdu);

// PRIVATE DATA
static const int kBatchUploadRequestMti = 320;

static const std::vector<int> kBatchUploadRspMandatoryFields = {
    kFieldProcessingCode,
    kFieldStan, kFieldTimeLocalTransaction, kFieldDateLocalTransaction,
    kFieldNii,
    kFieldRrn, kFieldResponseCode, kFieldCardAcceptorTerminalId };

// PUBLIC FUNCTIONS
iso8583::Apdu BuildBatchUploadRequest(const Transaction &tx) {
  iso8583::Apdu request_apdu(bdor_spec());

  request_apdu.SetMti(kBatchUploadRequestMti);

  if (tx.pan)
    SetPan(*tx.pan, request_apdu);

  SetProcessingCode(tx.processing_code, request_apdu);

  SetAmount(*tx.GetTotalAmount(), request_apdu);

  SetStan(tx.stan, request_apdu);
  SetOriginalHostDatetime(tx.tx_datetime, request_apdu);

  if (!tx.expiration_date.empty())
    SetExpirationDate(tx.expiration_date, request_apdu);

  SetPosEntryMode(tx.pos_entry_mode, request_apdu);
  SetNii(tx.nii, request_apdu);
  SetPosConditionCode(tx.pos_condition_code, request_apdu);

  // not allowed to store Track2 in batch

  SetRrn(tx.rrn, request_apdu);
  SetAuthorizationCode(tx.auth_id_response, request_apdu);
  SetResponseCode(tx.response_code, request_apdu);
  SetTid(tx.tid, request_apdu);
  SetMid(tx.mid, request_apdu);

  if (!tx.icc_data.empty()) {
     SetEmvData(tx.icc_data, request_apdu);
   }

  SetOriginalTransactionData(tx.transaction_type, tx.stan, tx.rrn,
                             request_apdu);

  SetInvoiceNumber(tx.invoice_num, request_apdu);

  // not allowed to store CVV

  AddDccDataField(tx, request_apdu);

  return request_apdu;
}

bool ReadAndValidateBatchUploadResponse(const iso8583::Apdu& request_apdu,
                                        const std::vector<uint8_t>& data,
                                        Transaction& tx) {

  iso8583::Apdu response_apdu(bdor_spec(), data.data(),
                              data.size());

  logger::debug(iso8583::Print(response_apdu).c_str());

  if (!CheckMandatoryFields(response_apdu, kBatchUploadRspMandatoryFields)
      || !ValidateBasicFields(request_apdu, response_apdu))
    return false;

  tx.rrn = GetRrn(response_apdu);

  auto auth_id_response = GetAuthIdResponse(response_apdu);
  if (auth_id_response)
    tx.auth_id_response = *auth_id_response;

  if (GetResponseCode(response_apdu) != "00"
      && GetResponseCode(response_apdu) != tx.response_code)
    return false;

  if (HasDateTime(response_apdu))
    tpcore::SetSystemDatetime(tx.tx_datetime);

  return true;
}

// PRIVATE FUNCTIONS
static void SetOriginalTransactionData(const TransactionType& transaction_type,
                                       const uint32_t& stan,
                                       const std::string& rrn,
                                       iso8583::Apdu& apdu) {
  //TODO: Transaction types MOTO_PREAUTH, MOTO_SALE, MOTO_REFUND -> do we need to specify?

  std::stringstream transaction_data;
  std::stringstream padded_stan;

  padded_stan << stan;

  switch (transaction_type) {
    case SALE:
      case CASH_ADVANCE:
      case REFUND:
      case PREAUTH_COMPLETION_ONLINE:
      case INSTALMENT_SALE:
      transaction_data << "0200"
                       << iso8583::RightAligned(padded_stan.str(), 6, '0')
                       << rrn;
      break;
    case PREAUTH:
      case OFFLINE_SALE:
      case PREAUTH_COMPLETION_OFFLINE:
      transaction_data << "0220"
                       << iso8583::RightAligned(padded_stan.str(), 6, '0')
                       << rrn;
    default:
      break;
  }

  apdu.SetField(kField60, transaction_data.str());
}

}

