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

#include "reversal_message.h"
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

// PRIVATE DECLARATIONS
static void SetProcessingCodeForReversal(
    const TransactionType& tx_type,
    const InProgressStatus& in_progress_status,
    iso8583::Apdu& request_apdu);

// PRIVATE DATA
static const int kReversalRequestMti = 400;

static const std::vector<int> kReversalRspMandatoryFields = {
    kFieldProcessingCode,
    kFieldStan,
    // kFieldTimeLocalTransaction, kFieldDateLocalTransaction,
    kFieldNii,
    kFieldRrn, kFieldResponseCode, kFieldCardAcceptorTerminalId };

// PUBLIC FUNCTIONS
iso8583::Apdu BuildReversalRequest(const Transaction &tx) {
  iso8583::Apdu request_apdu(bdor_spec());

  request_apdu.SetMti(kReversalRequestMti);

  if (tx.pan)
    SetPan(*tx.pan, request_apdu);

  SetProcessingCodeForReversal(tx.transaction_type, tx.in_progress_status,
                               request_apdu);

  if (tx.amount)
    SetAmount(*tx.GetTotalAmount(), request_apdu);
  else if (tx.preauth_amount)
    SetAmount(*tx.GetTotalPreauthAmount(), request_apdu);

  SetStan(tx.stan, request_apdu);

  if (!tx.expiration_date.empty())
    SetExpirationDate(tx.expiration_date, request_apdu);

  SetPosEntryMode(tx.pos_entry_mode, request_apdu);
  SetNii(tx.nii, request_apdu);
  SetPosConditionCode(tx.pos_condition_code, request_apdu);

  SetTid(tx.tid, request_apdu);
  SetMid(tx.mid, request_apdu);

  /* TODO: waiting for answer from FDMS, question #40
   if (tx.secondary_amount)
   SetAdditionalAmounts(*tx.secondary_amount, request_apdu);*/

  if (!tx.icc_data.empty()) {
    SetEmvData(tx.icc_data, request_apdu);
  }

  SetInvoiceNumber(tx.invoice_num, request_apdu);

  AddDccDataField(tx, request_apdu);

  if (tx.transaction_type == TransactionType::PREAUTH_COMPLETION_OFFLINE ||
      tx.transaction_type == TransactionType::PREAUTH_CANCELLATION) {
    SetPreAuthData(tx, request_apdu);
  }

  return request_apdu;
}

bool ReadAndValidateReversalResponse(const iso8583::Apdu& request_apdu,
                                     const std::vector<uint8_t>& data,
                                     Transaction& tx) {

  iso8583::Apdu response_apdu(bdor_spec(), data.data(),
                              data.size());

  logger::debug(iso8583::Print(response_apdu).c_str());

  if (!CheckMandatoryFields(response_apdu, kReversalRspMandatoryFields)
      || !ValidateBasicFields(request_apdu, response_apdu))
    return false;

  return GetResponseCode(response_apdu) == "00";
}

// PRIVATE FUNCTIONS
static void SetProcessingCodeForReversal(
    const TransactionType& tx_type,
    const InProgressStatus& in_progress_status,
    iso8583::Apdu& apdu) {
  std::string processing_code = GetProcessingCodeValue(
      tx_type, in_progress_status == InProgressStatus::IN_PROGRESS_VOID);
  apdu.SetField(kFieldProcessingCode, processing_code);
}

}

