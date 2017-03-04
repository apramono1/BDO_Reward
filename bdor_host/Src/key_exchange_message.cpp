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

#include "Key_exchange_message.h"
#include <bdor/key_exchange.h>

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
static stdx::optional<std::vector<uint8_t>> GetEncryptedPinKey(
    const iso8583::Apdu& apdu);
static stdx::optional<std::vector<uint8_t>> GetEncryptedTleKey(
    const iso8583::Apdu& apdu);

// PRIVATE DATA
static const int kKeyExchangeRequestMti = 800;
static const std::string kKeyExchangeProcessingCode = "920000";

static const std::vector<int> kKeyExchangeRspMandatoryFields = {
    kFieldProcessingCode,
    kFieldStan, kFieldTimeLocalTransaction, kFieldDateLocalTransaction,
    kFieldNii,
    kFieldResponseCode, kFieldCardAcceptorTerminalId, kField63 };

// PUBLIC FUNCTIONS
iso8583::Apdu BuildKeyExchangeRequest(const KeyExchange& key_exchange) {
  iso8583::Apdu request_apdu(bdor_spec());

  request_apdu.SetMti(kKeyExchangeRequestMti);

  SetProcessingCode(kKeyExchangeProcessingCode, request_apdu);
  SetStan(key_exchange.stan, request_apdu);
  SetNii(key_exchange.nii, request_apdu);
  SetTid(key_exchange.tid, request_apdu);
  SetMid(key_exchange.mid, request_apdu);

  return request_apdu;
}

bool ReadAndValidateKeyExchangeResponse(const iso8583::Apdu& request_apdu,
                                        const std::vector<uint8_t>& data,
                                        KeyExchange& key_exchange) {

  iso8583::Apdu response_apdu(bdor_spec(), data.data(),
                              data.size());

  logger::debug(iso8583::Print(response_apdu).c_str());

  if (!CheckMandatoryFields(response_apdu, kKeyExchangeRspMandatoryFields)
      || !ValidateBasicFields(request_apdu, response_apdu))
    return false;

  key_exchange.processing_code = GetProcessingCode(request_apdu);
  key_exchange.tx_datetime = *GetHostDatetime(response_apdu);

  key_exchange.response_code = GetResponseCode(response_apdu);

  if (GetResponseCode(response_apdu) != "00")
    return false;

  if (HasDateTime(response_apdu)) {
    tpcore::SetSystemDatetime(key_exchange.tx_datetime);
  }

  auto pin_key = GetEncryptedPinKey(response_apdu);
  if (pin_key)
    key_exchange.pin_key = *pin_key;

  auto tle_key = GetEncryptedTleKey(response_apdu);
  if (tle_key)
    key_exchange.tle_key = *tle_key;

  return true;

}

// PRIVATE FUNCTIONS
static stdx::optional<std::vector<uint8_t>> GetEncryptedPinKey(
    const iso8583::Apdu& apdu) {
  if (apdu.HasField(kField63)) {
    std::vector<uint8_t> field_content = apdu.GetFieldAsBytes(kField63);

    std::vector<uint8_t> pvt_key = ReadFieldFromTable("KP", field_content);
    std::vector<uint8_t> value(pvt_key.begin(), pvt_key.begin() + 16);

    return value;
  }

  return stdx::nullopt;
}

static stdx::optional<std::vector<uint8_t>> GetEncryptedTleKey(
    const iso8583::Apdu& apdu) {
  if (apdu.HasField(kField63)) {
    std::vector<uint8_t> field_content = apdu.GetFieldAsBytes(kField63);

    std::vector<uint8_t> pvt_key = ReadFieldFromTable("KP", field_content);
    std::vector<uint8_t> value(pvt_key.begin() + 16, pvt_key.begin() + 32);

    return value;
  }

  return stdx::nullopt;
}

}
