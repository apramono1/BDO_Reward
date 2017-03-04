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
#include "table_dcc.h"
#include "dcc_message.h"

using namespace types;

namespace bdor {

// PRIVATE MEMBERS
static const std::string kDccLegRate = "1";
static const std::string kDccLegComplete = "2";
static const std::string kTwoZeroes = "00";

static const std::vector<int> kDccOfflineRspMandatoryFields = {
    kFieldProcessingCode,
    kFieldStan,
    // kFieldTimeLocalTransaction, kFieldDateLocalTransaction,
    kFieldNii,
    kFieldRrn, kFieldResponseCode, kFieldCardAcceptorTerminalId,
    kField63 };

static bool ValidateDccOfflineResponse(const iso8583::Apdu& request_apdu,
                                    const iso8583::Apdu& response_apdu);

// PUBLIC FUNCTIONS

iso8583::Apdu BuildOfflineRequestWithDccEnquiry(const Transaction& tx) {
  auto request_apdu = BuildOfflineSaleRequest(tx);

  AddDccDataField(tx, request_apdu);

  return request_apdu;
}

bool ReadAndValidateOfflineResponseWithDccEnquiry(
    const iso8583::Apdu& request_apdu,
    const std::vector<uint8_t>& data,
    Transaction& tx) {

  iso8583::Apdu response_apdu(bdor_spec(), data.data(),
                              data.size());

  logger::debug(iso8583::Print(response_apdu).c_str());

  if (!ValidateDccOfflineResponse(request_apdu, response_apdu))
    return false;

  auto field_content = GetDccResponseField(response_apdu);
  if (!ReadDccSingleRateData(field_content, tx)) {
    return false;
  }

  if (tx.dcc_leg != kDccLegRate && tx.dcc_leg != kDccLegComplete) {
    return false;
  }

  if (tx.dcc_leg == kDccLegComplete) {
    UpdateTransactionInfoFromHostResponse(response_apdu, tx);
    UpdateSystemDateTime(response_apdu);
    tx.dcc_status = DccStatus::COMPLETE_NON_DCC;
    return true;
  }

  if (GetResponseCode(response_apdu) != kTwoZeroes) {
    return false;
  }

  tx.dcc_status = DccStatus::DCC_ELIGIBLE;
  return true;
}

iso8583::Apdu BuildOfflineRequestWithDccAllowed(const Transaction& tx) {
  iso8583::Apdu request_apdu = BuildOfflineSaleRequest(tx);

  AddDccDataField(tx, request_apdu);

  return request_apdu;
}

bool ReadAndValidateOfflineResponseWithDccAllowed(
    const iso8583::Apdu& request_apdu,
    const std::vector<uint8_t>& data,
    Transaction& tx) {
  iso8583::Apdu response_apdu(bdor_spec(), data.data(),
                              data.size());

  logger::debug(iso8583::Print(response_apdu).c_str());

  if (!ValidateDccOfflineResponse(request_apdu, response_apdu))
    return false;

  auto field_content = GetDccResponseField(response_apdu);
  if (!ReadDccSingleRateData(field_content, tx)) {
    return false;
  }

  if (tx.dcc_leg != kDccLegComplete) {
    return false;
  }

  UpdateTransactionInfoFromHostResponse(response_apdu, tx);
  UpdateSystemDateTime(response_apdu);
  return true;
}

static bool ValidateDccOfflineResponse(const iso8583::Apdu& request_apdu,
                                    const iso8583::Apdu& response_apdu) {
  if (!CheckMandatoryFields(response_apdu, kDccOfflineRspMandatoryFields)
      || !ValidateBasicFields(request_apdu, response_apdu))
    return false;

  return true;
}
}

