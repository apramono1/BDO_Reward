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

#include <apdu_utils.h>
#include <iso8583/encoder.h>
#include <iso8583/apdu.h>
#include <utils/logger.h>
#include <utils/converter.h>
#include "protocol.h"
#include <bdor/transaction.h>
#include "field_utils.h"
#include "table.h"
#include "table_cvv.h"
#include "table_instalment.h"
#include "table_preauth.h"
#include <tpcore/calendar.h>
#include <bdor/terminal_configuration.h>

using namespace types;

namespace bdor {

// PRIVATE MEMBERS
static const std::string kCvv2TableId = "16";

// PUBLIC FUNCTIONS
void SetPan(const types::Pan& pan, iso8583::Apdu& apdu) {
  apdu.SetField(kFieldPan, pan.ToString());
}

void SetProcessingCode(const std::string& processing_code,
                       iso8583::Apdu& apdu) {
  apdu.SetField(kFieldProcessingCode, processing_code);
}

void SetProcessingCode(const TransactionType& tx_type, iso8583::Apdu& apdu) {
  std::string processing_code = GetProcessingCodeValue(tx_type, false);
  SetProcessingCode(processing_code, apdu);
}

void SetProcessingCodeForVoid(const TransactionType& tx_type,
                              iso8583::Apdu& apdu) {
  std::string processing_code = GetProcessingCodeValue(tx_type, true);
  SetProcessingCode(processing_code, apdu);
}

void SetProcessingCodeForAdjust(const TransactionType& tx_type,
                                iso8583::Apdu& apdu) {
  std::string processing_code = GetProcessingCodeValue(tx_type, true);
  SetProcessingCode(processing_code, apdu);
}

void SetAmount(const types::Amount& amount, iso8583::Apdu& apdu) {
  apdu.SetField(kFieldAmount, amount.GetValue());
}

void SetOriginalAmount(const types::Amount& original_amount,
                       iso8583::Apdu& apdu) {
  apdu.SetField(kFieldAmount, original_amount.GetValue());
}

void SetStan(const uint32_t& stan, iso8583::Apdu& apdu) {
  apdu.SetField(kFieldStan, stan);
}

void SetOriginalHostDatetime(const time_t& timestamp, iso8583::Apdu& apdu) {
  apdu.SetField(kFieldTimeLocalTransaction,
                iso8583::IsoTimeFromTimestamp(timestamp));
  apdu.SetField(kFieldDateLocalTransaction,
                iso8583::IsoDateFromTimestamp(timestamp));
}

void SetNii(const uint32_t& nii, iso8583::Apdu& apdu) {
  apdu.SetField(kFieldNii, nii);
}

void SetPosConditionCode(
                         const types::PosConditionCode& pos_condition_code,
                         iso8583::Apdu& apdu) {
  auto value = GetPosConditionCodeValue(pos_condition_code);
  apdu.SetField(kFieldPosConditionCode, value);
}

void SetExpirationDate(const std::string& expiration_date,
                       iso8583::Apdu& apdu) {
  if (!expiration_date.empty())
    apdu.SetField(kFieldDateExpiration, expiration_date);
}

void SetPosEntryMode(const types::PosEntryMode& pos_entry_mode, iso8583::Apdu& apdu) {
  std::string value = GetPosEntryModeValue(pos_entry_mode);
  apdu.SetField(kFieldPosEntryMode, value.c_str());
}

void SetRrn(const std::string& rrn, iso8583::Apdu& apdu) {
  apdu.SetField(kFieldRrn, rrn);
}

void SetAuthorizationCode(
                          const std::string& authorization_code,
                          iso8583::Apdu& apdu) {
  apdu.SetField(kFieldAutorizationId, authorization_code);
}

void SetResponseCode(const std::string& response_code, iso8583::Apdu& apdu) {
  apdu.SetField(kFieldResponseCode, response_code);
}

void SetTid(const std::string& tid, iso8583::Apdu& apdu) {
  apdu.SetField(kFieldCardAcceptorTerminalId, tid);
}

void SetMid(const std::string& mid, iso8583::Apdu& apdu) {
  apdu.SetField(kFieldCardAcceptorId, mid);
}

void SetTrack2(const std::vector<uint8_t>& track2, iso8583::Apdu& apdu) {
  apdu.SetField(kFieldTrack2Data, track2);
}

void SetPinBlock(const std::vector<uint8_t>& pin_block, iso8583::Apdu& apdu) {
  apdu.SetField(kFieldPinBlock, pin_block);
}

void SetAdditionalAmount(const types::Amount& additional_amount,
                         iso8583::Apdu& apdu) {
  auto additional_amount_str = utils::ToString(additional_amount.GetValue());
  auto value = iso8583::RightAligned(additional_amount_str, 12, '0');
  apdu.SetField(kFieldAdditionalAmount, value);
}

void SetEmvData(const std::vector<uint8_t>& emv_data, iso8583::Apdu& apdu) {
  logger::die_unless(!emv_data.empty(), "EMV data not present");
  apdu.SetField(kFieldIccData, emv_data);
}

void SetInvoiceNumber(const uint32_t& invoice, iso8583::Apdu& apdu) {
  auto invoice_str = utils::ToString(invoice);
  std::string value = iso8583::RightAligned(invoice_str, 6, '0');
  apdu.SetField(kField62, value);
}

void SetCvv2Data(const std::string& cvv, iso8583::Apdu& apdu) {
  auto cvv_table_data = BuildCvvTable(cvv);
  AddTableDataToField63(cvv_table_data, apdu);
}

void SetPreAuthData(const Transaction& tx, iso8583::Apdu& apdu) {
  auto invoice_str = utils::ToString(tx.original_invoice_number);
  auto invoice_str_padded = iso8583::RightAligned(invoice_str, 6, '0');

  auto pan_str = (*tx.pan).ToString();
  auto last_four_digits = pan_str.substr(pan_str.length() - 4, 4);

  auto auth_code_padded = iso8583::LeftAligned(tx.auth_id_response_original, 6, ' ');

  auto preauth_table_data = BuildPreAuthTable(invoice_str_padded, last_four_digits, auth_code_padded);
  AddTableDataToField63(preauth_table_data, apdu);
}

void SetInstalmentData(const Transaction& tx, iso8583::Apdu& apdu) {
  auto instalment_table_data = BuildInstalmentTable(tx);
  AddTableDataToField63(instalment_table_data, apdu);
}


std::string GetProcessingCode(const iso8583::Apdu& apdu) {
  return apdu.GetFieldAsString(kFieldProcessingCode);
}

uint64_t GetAmount(const iso8583::Apdu& apdu) {
  return apdu.GetFieldAsInteger(kFieldAmount);
}

uint32_t GetStan(const iso8583::Apdu& apdu) {
  return apdu.GetFieldAsInteger(kFieldStan);
}

stdx::optional<time_t> GetHostDatetime(const iso8583::Apdu& apdu) {
  if (apdu.HasField(kFieldTimeLocalTransaction)
      && apdu.HasField(kFieldDateLocalTransaction)) {
    std::string date = apdu.GetFieldAsString(kFieldDateLocalTransaction);
    date = iso8583::RightAligned(date, 4, '0');
    std::string time = apdu.GetFieldAsString(kFieldTimeLocalTransaction);
    time = iso8583::RightAligned(time, 6, '0');
    return utils::GetDatetimefromIso8583Format(date, time);
  }

  return stdx::nullopt;
}

uint32_t GetNii(const iso8583::Apdu& apdu) {
  return apdu.GetFieldAsInteger(kFieldNii);
}

std::string GetRrn(const iso8583::Apdu& apdu) {
  return apdu.GetFieldAsString(kFieldRrn);
}

stdx::optional<std::string> GetAuthIdResponse(const iso8583::Apdu& apdu) {
  if (apdu.HasField(kFieldAutorizationId))
    return apdu.GetFieldAsString(kFieldAutorizationId);

  return stdx::nullopt;
}

std::string GetResponseCode(const iso8583::Apdu& apdu) {
  return apdu.GetFieldAsString(kFieldResponseCode);
}

std::string GetTid(const iso8583::Apdu& apdu) {
  return apdu.GetFieldAsString(kFieldCardAcceptorTerminalId);
}

stdx::optional<std::vector<uint8_t>> GetEmvData(const iso8583::Apdu& apdu) {
  if (apdu.HasField(kFieldIccData))
    return apdu.GetFieldAsBytes(kFieldIccData);

  return stdx::nullopt;
}

std::string GetField63Data(const iso8583::Apdu& apdu){
	return apdu.GetFieldAsString(kField63);
}

bool HasDateTime(const iso8583::Apdu& apdu) {
  return apdu.HasField(kFieldTimeLocalTransaction)
      && apdu.HasField(kFieldDateLocalTransaction);
}

bool ValidateBasicFields(const iso8583::Apdu& request_apdu,
                         const iso8583::Apdu& response_apdu) {
  if (!response_apdu.HasMti())
    return false;

  if (response_apdu.GetMti() != request_apdu.GetMti() + 10)
    return false;

  if (GetProcessingCode(response_apdu) != GetProcessingCode(request_apdu) ||
      GetStan(response_apdu) != GetStan(request_apdu) ||
      GetNii(response_apdu) != GetNii(request_apdu) ||
      GetTid(response_apdu) != GetTid(request_apdu))
    return false;

  // may want to compare transaction amounts
  return true;
}

bool CheckMandatoryFields(const iso8583::Apdu& apdu,
                          const std::vector<int>& fields) {
  for (auto field_num : fields) {
    if (!apdu.HasField(field_num))
      return false;
  }

  return true;
}

void UpdateSystemDateTime(const iso8583::Apdu& response_apdu) {
  if (GetResponseCode(response_apdu) == "00" && HasDateTime(response_apdu)) {
    tpcore::SetSystemDatetime(*GetHostDatetime(response_apdu));
  }
}

void AddTableDataToField63(
                           const std::vector<uint8_t>& full_table,
                           iso8583::Apdu& apdu) {
  auto field_content = apdu.GetFieldAsBytes(kField63);

  field_content.insert(field_content.end(), full_table.begin(), full_table.end());

  apdu.SetField(kField63, field_content);
}

void UpdateTransactionInfoFromHostResponse(const iso8583::Apdu& response_apdu,
                                           Transaction& tx) {

  tx.processing_code = GetProcessingCode(response_apdu);

  auto host_date_time = GetHostDatetime(response_apdu);
  if (host_date_time)
    tx.tx_datetime = *host_date_time;

  tx.rrn = GetRrn(response_apdu);

  auto auth_id_response = GetAuthIdResponse(response_apdu);
  if (auth_id_response) {
    tx.auth_id_response = *auth_id_response;

    if (tx.auth_id_response_original.empty())
      tx.auth_id_response_original = tx.auth_id_response;
  }

  tx.response_code = GetResponseCode(response_apdu);
  tx.field63_data = GetField63Data(response_apdu);

  auto emv_data = GetEmvData(response_apdu);
  if (emv_data)
    tx.issuer_emv_response = *emv_data;
}
}
