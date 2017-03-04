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

#include "tc_upload_message.h"
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

// PRIVATE DECLARATIONS
static void SetTcUploadProcessingCode(const bool& more_message, iso8583::Apdu& apdu);

// PRIVATE DATA
static const int kTcUploadRequestMti = 320;
static const std::string kTcUploadProcessingCode = "940000";

static const std::vector<int> kTcUploadRspMandatoryFields = {
    kFieldProcessingCode,
    kFieldStan, kFieldTimeLocalTransaction, kFieldDateLocalTransaction,
    kFieldNii,
    kFieldRrn, kFieldResponseCode, kFieldCardAcceptorTerminalId };

// PUBLIC FUNCTIONS
iso8583::Apdu BuildTcUploadRequest(const Transaction& tx) {
  iso8583::Apdu request_apdu(bdor_spec());

  request_apdu.SetMti(kTcUploadRequestMti);

  if (tx.pan) {
    SetPan(*tx.pan, request_apdu);
  }

  SetTcUploadProcessingCode(false, request_apdu);

  SetAmount(*tx.GetTotalAmount(), request_apdu);

  SetStan(tx.tc_upload_stan, request_apdu);

  if (!tx.expiration_date.empty()) {
    SetExpirationDate(tx.expiration_date, request_apdu);
  }

  SetPosEntryMode(tx.pos_entry_mode, request_apdu);
  SetNii(tx.nii, request_apdu);

  SetPosConditionCode(tx.pos_condition_code, request_apdu);

  SetRrn(tx.rrn, request_apdu);
  SetAuthorizationCode(tx.auth_id_response, request_apdu);
  SetResponseCode(tx.response_code, request_apdu);

  SetTid(tx.tid, request_apdu);
  SetMid(tx.mid, request_apdu);

  if (!tx.icc_data.empty()) {
    SetEmvData(tx.icc_data, request_apdu);
  }

  SetOriginalAmount(*tx.GetTotalAmount(), request_apdu);  // TODO: check with FDMS

  SetInvoiceNumber(tx.invoice_num, request_apdu);

  return request_apdu;
}

bool ReadAndValidateTcUploadResponse(const iso8583::Apdu& request_apdu,
                                     const std::vector<uint8_t>& data,
                                     Transaction& tx) {

  iso8583::Apdu response_apdu(bdor_spec(), data.data(),
                              data.size());

  logger::debug(iso8583::Print(response_apdu).c_str());

  if (!CheckMandatoryFields(response_apdu, kTcUploadRspMandatoryFields)
      || !ValidateBasicFields(request_apdu, response_apdu))
    return false;

  if (GetResponseCode(response_apdu) != "00")
    return false;

  if (HasDateTime(response_apdu)) {
    tpcore::SetSystemDatetime(tx.tx_datetime);
  }

  return true;
}

// PRIVATE FUNCTIONS
static void SetTcUploadProcessingCode(const bool& more_message, iso8583::Apdu& apdu) {
  std::string processing_code = kTcUploadProcessingCode;
  if (more_message) {
    processing_code += "1";
  } else {
    processing_code += "0";
  }
  apdu.SetField(kFieldProcessingCode, processing_code);
}

}
