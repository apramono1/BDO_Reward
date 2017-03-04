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

#include "offline_sale_message.h"
#include "protocol.h"
#include "field_utils.h"
#include <iso8583/field_types.h>
#include <iso8583/encoder.h>
#include <utils/converter.h>
#include <utils/logger.h>
#include <iso8583/printer.h>
#include "apdu_utils.h"
#include <tpcore/calendar.h>

namespace bdor {

// PRIVATE MEMBERS
static bool ValidateOfflineSaleResponse(const iso8583::Apdu& request_apdu,
                          const iso8583::Apdu& response_apdu);

static const int kOfflineSaleRequestMti = 220;

static const std::vector<int> kOfflineSaleRspMandatoryFields = {
    kFieldProcessingCode,
    kFieldStan,
    // kFieldTimeLocalTransaction, kFieldDateLocalTransaction,
    kFieldNii,
    kFieldRrn, kFieldResponseCode, kFieldCardAcceptorTerminalId };

// PUBLIC FUNCTIONS
iso8583::Apdu BuildOfflineSaleRequest(const Transaction& tx) {
  iso8583::Apdu request_apdu(bdor_spec());

  request_apdu.SetMti(kOfflineSaleRequestMti);

  SetPan(*tx.pan, request_apdu);

  SetProcessingCode(tx.transaction_type, request_apdu);

  if (tx.amount)
    SetAmount(*tx.GetTotalAmount(), request_apdu);

  SetStan(tx.stan, request_apdu);
  SetOriginalHostDatetime(tx.tx_datetime, request_apdu);

  if (!tx.expiration_date.empty()) {
    SetExpirationDate(tx.expiration_date, request_apdu);
  }

  SetPosEntryMode(tx.pos_entry_mode, request_apdu);
  SetNii(tx.nii, request_apdu);
  SetPosConditionCode(tx.pos_condition_code, request_apdu);

  SetAuthorizationCode(tx.auth_id_response, request_apdu);
  SetResponseCode(tx.response_code, request_apdu);
  SetTid(tx.tid, request_apdu);
  SetMid(tx.mid, request_apdu);

  if (tx.secondary_amount) {
    SetAdditionalAmount(*tx.secondary_amount, request_apdu);
  }

  if (!tx.icc_data.empty()) {
    SetEmvData(tx.icc_data, request_apdu);
  }

  SetInvoiceNumber(tx.invoice_num, request_apdu);

  if (tx.transaction_type == TransactionType::PREAUTH_COMPLETION_OFFLINE) {
    SetPreAuthData(tx, request_apdu);
  }

  return request_apdu;
}

bool ReadAndValidateOfflineSaleResponse(const iso8583::Apdu& request_apdu,
                                 const std::vector<uint8_t>& data,
                                 Transaction& tx) {

  iso8583::Apdu response_apdu(bdor_spec(), data.data(),
                              data.size());

  logger::debug(iso8583::Print(response_apdu).c_str());

  if (!ValidateOfflineSaleResponse(request_apdu, response_apdu))
    return false;

  UpdateTransactionInfoFromHostResponse(response_apdu, tx);

  if (GetResponseCode(response_apdu) != tx.response_code
        && GetResponseCode(response_apdu) != "00")
      return false;

  UpdateSystemDateTime(response_apdu);

  return true;
}

// PRIVATE FUNCTIONS
static bool ValidateOfflineSaleResponse(const iso8583::Apdu& request_apdu,
                          const iso8583::Apdu& response_apdu) {
  if (!CheckMandatoryFields(response_apdu, kOfflineSaleRspMandatoryFields)
      || !ValidateBasicFields(request_apdu, response_apdu))
    return false;

  return true;
}
}

