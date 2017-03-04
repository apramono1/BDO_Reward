
#include "card_validation_message.h"
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

// PRIVATE MEMBERS
static bool ValidateCardValidationResponse(const iso8583::Apdu& request_apdu,
                          const iso8583::Apdu& response_apdu);

static const int kCardValidationRequestMti = 100;

static const std::vector<int> kCardValidationRspMandatoryFields = {
    kFieldProcessingCode,
    kFieldStan, kFieldTimeLocalTransaction, kFieldDateLocalTransaction,
    kFieldNii,
    kFieldResponseCode, kFieldCardAcceptorTerminalId, kField63 };

// PUBLIC FUNCTIONS
iso8583::Apdu BuildCardValidationRequest(const Transaction& tx) {
  iso8583::Apdu request_apdu(bdor_spec());

  request_apdu.SetMti(kCardValidationRequestMti);

  if (tx.pos_entry_mode == MANUAL && tx.pan) {
    SetPan(*tx.pan, request_apdu);
  }

  SetProcessingCode(tx.transaction_type, request_apdu);

  SetStan(tx.stan, request_apdu);

  if (tx.pos_entry_mode == MANUAL && !tx.expiration_date.empty()) {
    SetExpirationDate(tx.expiration_date, request_apdu);
  }

  SetPosEntryMode(tx.pos_entry_mode, request_apdu);
  SetNii(tx.nii, request_apdu);

  SetPosConditionCode(tx.pos_condition_code, request_apdu);

  if (IsTrack2ToBeSent(tx.pos_entry_mode)) {
    SetTrack2((*tx.track2_equivalent_data).GetData(), request_apdu);
  }

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

  return request_apdu;
}

bool ReadAndValidateCardValidationResponse(const iso8583::Apdu& request_apdu,
                                 const std::vector<uint8_t>& data,
                                 Transaction& tx) {

  iso8583::Apdu response_apdu(bdor_spec(), data.data(),
                              data.size());

  logger::debug(iso8583::Print(response_apdu).c_str());

  if (!ValidateCardValidationResponse(request_apdu, response_apdu))
    return false;

  UpdateTransactionInfoFromHostResponse(response_apdu, tx);
  UpdateSystemDateTime(response_apdu);

  return true;
}


// PRIVATE FUNCTIONS
static bool ValidateCardValidationResponse(const iso8583::Apdu& request_apdu,
                          const iso8583::Apdu& response_apdu) {
  if (!CheckMandatoryFields(response_apdu, kCardValidationRspMandatoryFields)
      || !ValidateBasicFields(request_apdu, response_apdu))
    return false;

  return true;
}

}
