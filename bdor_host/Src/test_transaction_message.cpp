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

#include "test_transaction_message.h"
#include <bdor/test_transaction.h>
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

// PRIVATE DATA
static const int kTestTransactionRequestMti = 800;
static const std::string kTestTransactionProcessingCode = "990000";

static const std::vector<int> kTestTransactionRspMandatoryFields = {
    kFieldProcessingCode,
    kFieldTimeLocalTransaction, kFieldDateLocalTransaction,
    kFieldNii,
    kFieldCardAcceptorTerminalId };

// PUBLIC FUNCTIONS
iso8583::Apdu BuildTestTransactionRequest(const TestTransaction& test_tx) {
  iso8583::Apdu request_apdu(bdor_spec());

  request_apdu.SetMti(kTestTransactionRequestMti);

  SetProcessingCode(kTestTransactionProcessingCode, request_apdu);

  SetNii(test_tx.nii, request_apdu);

  SetTid(test_tx.tid, request_apdu);
  SetMid(test_tx.mid, request_apdu);

  return request_apdu;
}

bool ReadAndValidateTestTransactionResponse(const iso8583::Apdu& request_apdu,
                                            const std::vector<uint8_t>& data,
                                            TestTransaction& test_tx) {

  iso8583::Apdu response_apdu(bdor_spec(), data.data(),
                              data.size());

  logger::debug(iso8583::Print(response_apdu).c_str());

  if (!CheckMandatoryFields(response_apdu, kTestTransactionRspMandatoryFields))
    return false;

  if (!response_apdu.HasMti())
    return false;

  if (response_apdu.GetMti() != request_apdu.GetMti() + 10)
    return false;

  if (GetProcessingCode(response_apdu) != GetProcessingCode(request_apdu) ||
      GetNii(response_apdu) != GetNii(request_apdu) ||
      GetTid(response_apdu) != GetTid(request_apdu))
    return false;

  test_tx.processing_code = GetProcessingCode(request_apdu);

  auto host_date_time = GetHostDatetime(response_apdu);
  if (host_date_time)
    test_tx.host_datetime = *host_date_time;

  if (HasDateTime(response_apdu)) {
    tpcore::SetSystemDatetime(test_tx.host_datetime);
  }

  return true;
}

}
