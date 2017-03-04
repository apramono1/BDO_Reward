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
#include "void_transaction_message.h"
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

// PRIVATE DATA
static const int kVoidRequestMti = 200;

static const std::vector<int> kVoidRspMandatoryFields = {
    kFieldProcessingCode, kFieldAmount,
    kFieldStan, kFieldTimeLocalTransaction, kFieldDateLocalTransaction,
    kFieldNii,
    kFieldRrn, kFieldResponseCode, kFieldCardAcceptorTerminalId };

// PUBLIC FUNCTIONS
iso8583::Apdu BuildVoidTransactionRequest(const Transaction &tx) {
  iso8583::Apdu request_apdu(bdor_spec());

  request_apdu.SetMti(kVoidRequestMti);

  if (tx.pan)
    SetPan(*tx.pan, request_apdu);

  SetProcessingCodeForVoid(tx.transaction_type, request_apdu);

  SetAmount(*tx.GetTotalAmount(), request_apdu);

  SetStan(tx.stan, request_apdu);

  SetOriginalHostDatetime(tx.tx_datetime, request_apdu);

  if (!tx.expiration_date.empty())
    SetExpirationDate(tx.expiration_date, request_apdu);

  SetPosEntryMode(tx.pos_entry_mode, request_apdu);
  SetNii(tx.nii, request_apdu);
  SetPosConditionCode(PosConditionCode::NORMAL, request_apdu);

  SetRrn(tx.rrn, request_apdu);
  SetAuthorizationCode(tx.auth_id_response, request_apdu);
  SetResponseCode(tx.response_code, request_apdu);

  SetTid(tx.tid, request_apdu);
  SetMid(tx.mid, request_apdu);

  SetInvoiceNumber(tx.invoice_num, request_apdu);

  if (tx.transaction_type == TransactionType::INSTALMENT_SALE)
    SetInstalmentData(tx, request_apdu);

  AddDccDataField(tx, request_apdu);

  return request_apdu;
}

bool ReadAndValidateVoidTransactionResponse(const iso8583::Apdu& request_apdu,
                                            const std::vector<uint8_t>& data,
                                            Transaction& tx) {

  iso8583::Apdu response_apdu(bdor_spec(), data.data(),
                              data.size());

  logger::debug(iso8583::Print(response_apdu).c_str());

  if (!CheckMandatoryFields(response_apdu, kVoidRspMandatoryFields)
      || !ValidateBasicFields(request_apdu, response_apdu))
    return false;

  tx.rrn = GetRrn(response_apdu);

  auto auth_id_response = GetAuthIdResponse(response_apdu);
  if (auth_id_response)
    tx.auth_id_response = *auth_id_response;

  tx.response_code = GetResponseCode(response_apdu);

  if (tx.response_code.compare("00") == 0 && HasDateTime(response_apdu)) {
    tpcore::SetSystemDatetime(tx.tx_datetime);
  }

  return true;
}
}

