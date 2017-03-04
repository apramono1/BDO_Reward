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

#include "preauth_cancellation_message.h"
#include "protocol.h"
#include "field_utils.h"
#include <iso8583/field_types.h>
#include <iso8583/encoder.h>
#include <utils/converter.h>
#include <utils/logger.h>
#include <iso8583/printer.h>
#include "apdu_utils.h"
#include <tpcore/calendar.h>

using namespace types;

namespace bdor {

// PRIVATE DATA
static const int kPreAuthCancellationRequestMti = 100;

static const std::vector<int> kPreAuthCancellationRspMandatoryFields = {
    kFieldProcessingCode, kFieldAmount,
    kFieldStan, kFieldTimeLocalTransaction, kFieldDateLocalTransaction,
    kFieldNii,
    kFieldRrn, kFieldResponseCode, kFieldCardAcceptorTerminalId };

// PUBLIC FUNCTIONS
iso8583::Apdu BuildPreAuthCancellationRequest(const Transaction& tx) {
  iso8583::Apdu request_apdu(bdor_spec());

  request_apdu.SetMti(kPreAuthCancellationRequestMti);

  if (tx.track2_equivalent_data.has_value()) {
    SetTrack2((*tx.track2_equivalent_data).GetData(), request_apdu);
  }
  else {
    if (tx.pan) {
      SetPan(*tx.pan, request_apdu);
    }
    if (!tx.expiration_date.empty()) {
      SetExpirationDate(tx.expiration_date, request_apdu);
    }
  }

  SetProcessingCodeForVoid(tx.transaction_type, request_apdu);

  SetAmount(*tx.GetTotalPreauthAmount(), request_apdu);

  SetStan(tx.stan, request_apdu);

  SetPosEntryMode(tx.pos_entry_mode, request_apdu);
  SetNii(tx.nii, request_apdu);

  SetPosConditionCode(tx.pos_condition_code, request_apdu);

  SetAuthorizationCode(tx.auth_id_response, request_apdu);

  SetTid(tx.tid, request_apdu);
  SetMid(tx.mid, request_apdu);

  if (!tx.pin_data.empty()) {
    SetPinBlock(tx.pin_data, request_apdu);
  }

  if (tx.pos_entry_mode == CHIP) {
    SetEmvData(tx.icc_data, request_apdu);
  }

  SetInvoiceNumber(tx.invoice_num, request_apdu);

  if (!tx.cvv.empty()) {
    SetCvv2Data(tx.cvv, request_apdu);
  }

  SetPreAuthData(tx, request_apdu);

  return request_apdu;
}

bool ReadAndValidatePreAuthCancellationResponse(const iso8583::Apdu& request_apdu,
                                    const std::vector<uint8_t>& data,
                                    Transaction& tx) {

  iso8583::Apdu response_apdu(bdor_spec(), data.data(),
                                              data.size());

  logger::debug(iso8583::Print(response_apdu).c_str());

  if (!CheckMandatoryFields(response_apdu, kPreAuthCancellationRspMandatoryFields)
      || !ValidateBasicFields(request_apdu, response_apdu))
    return false;

  tx.processing_code = GetProcessingCode(request_apdu);

  //      (GetAmount() != tx.GetTotalPreauthAmount()->GetValue()) ||

  auto host_date_time = GetHostDatetime(response_apdu);
  if (host_date_time)
    tx.tx_datetime = *host_date_time;

  tx.rrn = GetRrn(response_apdu);

  auto auth_id_response = GetAuthIdResponse(response_apdu);
  if (auth_id_response)
    tx.auth_id_response_original = tx.auth_id_response = *auth_id_response;

  tx.response_code = GetResponseCode(response_apdu);

  if (tx.response_code.compare("00") == 0 && HasDateTime(response_apdu)) {
    tpcore::SetSystemDatetime(tx.tx_datetime);
  }

  auto emv_data = GetEmvData(response_apdu);
  if (emv_data)
    tx.issuer_emv_response = *emv_data;

  return true;
}

}
