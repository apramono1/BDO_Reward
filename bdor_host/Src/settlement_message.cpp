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
#include "settlement_message.h"

#include <vector>
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
static void SetBatchNumber(uint32_t batch_number, iso8583::Apdu& apdu);

static void SetBatchTotal(const BatchTotalsForHost& batch_count,
                          iso8583::Apdu& apdu);

static std::string GetSettlementResponseMsg(const iso8583::Apdu& apdu);

// PRIVATE DATA
static const int kSettlementRequestMti = 500;
static const std::string kSettlementProcessingCode = "920000";
static const std::string kSettlementProcessingCodeBatchClose = "960000";

static const std::vector<int> kSettlementRspMandatoryFields = {
    kFieldProcessingCode,
    kFieldStan, kFieldTimeLocalTransaction, kFieldDateLocalTransaction,
    kFieldNii, kFieldRrn,
    kFieldResponseCode, kFieldCardAcceptorTerminalId };

// PUBLIC FUNCTIONS
iso8583::Apdu BuildSettlementRequest(SettlementData& settle_req,
                                     bool after_batch_upload) {
  iso8583::Apdu request_apdu(bdor_spec());

  request_apdu.SetMti(kSettlementRequestMti);

  if (after_batch_upload)
    SetProcessingCode(kSettlementProcessingCodeBatchClose, request_apdu);
  else
    SetProcessingCode(kSettlementProcessingCode, request_apdu);

  SetStan(settle_req.stan, request_apdu);
  SetNii(settle_req.nii, request_apdu);
  SetTid(settle_req.tid, request_apdu);
  SetMid(settle_req.mid, request_apdu);
  SetBatchNumber(settle_req.batch_number, request_apdu);
  SetBatchTotal(settle_req.batch_summary, request_apdu);

  return request_apdu;
}

bool ReadAndValidateSettlementResponse(
                                       const iso8583::Apdu& request_apdu,
                                       const std::vector<uint8_t> &data,
                                       SettlementData& settle_rsp) {

  iso8583::Apdu response_apdu(bdor_spec(), data.data(),
                              data.size());

  logger::debug(iso8583::Print(response_apdu).c_str());

  if (!CheckMandatoryFields(response_apdu, kSettlementRspMandatoryFields)
      || !ValidateBasicFields(request_apdu, response_apdu))
    return false;

  settle_rsp.processing_code = GetProcessingCode(request_apdu);
  settle_rsp.tx_datetime = *GetHostDatetime(response_apdu);
  settle_rsp.rrn = GetRrn(response_apdu);
  settle_rsp.response_code = GetResponseCode(response_apdu);
  settle_rsp.response_msg = GetSettlementResponseMsg(response_apdu);

  return true;
}

// PRIVATE FUNCTIONS
static void SetBatchNumber(uint32_t batch_number, iso8583::Apdu& apdu) {
  std::stringstream batch_num;
  batch_num << batch_number;
  apdu.SetField(kField60, iso8583::RightAligned(batch_num.str(), 6, '0'));
}

static void SetBatchTotal(const BatchTotalsForHost& batch_count,
                          iso8583::Apdu& apdu) {
  std::string batch_totals;
  batch_totals = utils::NumericPadded(batch_count.sales_total.count, 3) +
      utils::NumericPadded(batch_count.sales_total.total, 12) +
      utils::NumericPadded(batch_count.refunds_total.count, 3) +
      utils::NumericPadded(batch_count.refunds_total.total, 12);

  const std::string kEmptyBlock(30, '0');
  batch_totals += kEmptyBlock;
  batch_totals += kEmptyBlock;

  std::vector<uint8_t> batch_totals_in_bytes(
      batch_totals.c_str(),
      batch_totals.c_str() + batch_totals.length());

  apdu.SetField(kField63, batch_totals_in_bytes);
}

static std::string GetSettlementResponseMsg(const iso8583::Apdu& apdu) {
  std::vector<uint8_t> response_msg_in_bytes;

  if (apdu.HasField(kField63)) {
    std::vector < uint8_t > response_msg_in_bytes = apdu.GetFieldAsBytes(
        kField63);

    std::string response_msg((const char*) response_msg_in_bytes.data(),
                             response_msg_in_bytes.size());

    logger::debug("Settlement Response Msg: %s", response_msg.c_str());

    return response_msg;
  }

  return std::string();
}

}

