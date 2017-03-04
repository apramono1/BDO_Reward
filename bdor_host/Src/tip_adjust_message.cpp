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

#include "tip_adjust_message.h"
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

namespace bdor {

// PRIVATE DECLARATIONS
static void SetTipAdjustOriginalAmount(
                                       const types::Amount& original_amount,
                                       iso8583::Apdu& apdu);

// PRIVATE DATA
static const int kTipAdjustRequestMti = 220;

static const std::vector<int> kTipAdjustRspMandatoryFields = {
    kFieldProcessingCode,
    kFieldStan,
    // kFieldTimeLocalTransaction, kFieldDateLocalTransaction,
    kFieldNii,
    kFieldRrn, kFieldResponseCode, kFieldCardAcceptorTerminalId };

// PUBLIC FUNCTIONS
iso8583::Apdu BuildTipAdjustRequest(const Transaction& tx) {
  iso8583::Apdu request_apdu(bdor_spec());

  request_apdu.SetMti(kTipAdjustRequestMti);

  SetPan(*tx.pan, request_apdu);

  SetProcessingCodeForAdjust(tx.transaction_type, request_apdu);

  SetAmount(*tx.GetTotalAmount(), request_apdu);

  SetStan(tx.stan, request_apdu);

  SetOriginalHostDatetime(tx.tx_datetime, request_apdu);
  SetExpirationDate(tx.expiration_date, request_apdu);
  SetPosEntryMode(tx.pos_entry_mode, request_apdu);
  SetNii(tx.nii, request_apdu);
  SetPosConditionCode(tx.pos_condition_code, request_apdu);
  SetRrn(tx.rrn, request_apdu);
  SetAuthorizationCode(tx.auth_id_response, request_apdu);

  SetTid(tx.tid, request_apdu);
  SetMid(tx.mid, request_apdu);

  if (tx.secondary_amount) {
    SetAdditionalAmount(*tx.secondary_amount, request_apdu);
  }

  if (tx.dcc_status == DccStatus::OPT_IN) {
    SetDccAmount(*tx.dcc_amount, request_apdu);
    SetDccConversionRate(tx.dcc_conversion_rate, request_apdu);
    SetDccCurrencyCode((*tx.dcc_amount).GetCurrencyCode(), request_apdu);
    SetTipAdjustOriginalAmount(*tx.dcc_amount_original, request_apdu);
  }
  else {
    SetTipAdjustOriginalAmount(*tx.GetTotalOriginalAmount(), request_apdu);
  }

  SetInvoiceNumber(tx.invoice_num, request_apdu);

  return request_apdu;
}

bool ReadAndValidateTipAdjustResponse(const iso8583::Apdu& request_apdu,
                                      const std::vector<uint8_t>& data,
                                      Transaction& tx) {

  iso8583::Apdu response_apdu(bdor_spec(), data.data(),
                              data.size());

  logger::debug(iso8583::Print(response_apdu).c_str());

  if (!CheckMandatoryFields(response_apdu, kTipAdjustRspMandatoryFields)
      || !ValidateBasicFields(request_apdu, response_apdu))
    return false;

  tx.processing_code = GetProcessingCode(request_apdu);

  tx.rrn = GetRrn(response_apdu);

  auto auth_id_response = GetAuthIdResponse(response_apdu);
  if (auth_id_response)
    tx.auth_id_response_original = tx.auth_id_response = *auth_id_response;

  tx.response_code = GetResponseCode(response_apdu);

  return true;
}

// PRIVATE FUNCTIONS
static void SetTipAdjustOriginalAmount(
                                       const types::Amount& original_amount,
                                       iso8583::Apdu& apdu) {
  auto original_amount_str = utils::ToString(original_amount.GetValue());
  auto value = iso8583::RightAligned(original_amount_str, 12, '0');
  apdu.SetField(kField60, value);
}

}
