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
#ifndef BDOR__APDU_UTILS_H_
#define BDOR__APDU_UTILS_H_

#include <vector>
#include <cstdint>
#include <memory>
#include <ctime>

#include <stdx/optional>
#include <iso8583/apdu.h>

#include <types/pan.h>
#include <types/amount.h>
#include <types/pos_entry_mode.h>
#include <types/pos_condition_code.h>
#include <bdor/transaction_type.h>
#include <bdor/transaction.h>

namespace bdor {

void SetPan(const types::Pan& pan, iso8583::Apdu& apdu);

void SetProcessingCode(const std::string& processing_code, iso8583::Apdu& apdu);

void SetProcessingCode(const TransactionType& tx_type, iso8583::Apdu& apdu);

void SetProcessingCodeForVoid(const TransactionType& tx_type,
                              iso8583::Apdu& apdu);

void SetProcessingCodeForAdjust(const TransactionType& tx_type,
                                iso8583::Apdu& apdu);

void SetAmount(const types::Amount& amount, iso8583::Apdu& apdu);

void SetOriginalAmount(const types::Amount& original_amount,
                       iso8583::Apdu& apdu);

void SetStan(const uint32_t& stan, iso8583::Apdu& apdu);

void SetOriginalHostDatetime(const time_t& timestamp, iso8583::Apdu& apdu);

void SetNii(const uint32_t& nii, iso8583::Apdu& apdu);

void SetPosConditionCode(
                         const types::PosConditionCode& pos_condition_code,
                         iso8583::Apdu& apdu);

void SetExpirationDate(const std::string& expiration_date, iso8583::Apdu& apdu);

void SetPosEntryMode(const types::PosEntryMode& pos_entry_mode,
                     iso8583::Apdu& apdu);

void SetRrn(const std::string& rrn, iso8583::Apdu& apdu);

void SetAuthorizationCode(
                          const std::string& authorization_code,
                          iso8583::Apdu& apdu);

void SetResponseCode(const std::string& response_code, iso8583::Apdu& apdu);

void SetTid(const std::string& tid, iso8583::Apdu& apdu);

void SetMid(const std::string& mid, iso8583::Apdu& apdu);

void SetTrack2(const std::vector<uint8_t>& track2, iso8583::Apdu& apdu);

void SetPinBlock(const std::vector<uint8_t>& pin_block, iso8583::Apdu& apdu);

void SetCurrencyCode(const uint32_t& currency_code, iso8583::Apdu& apdu);

void SetAdditionalAmount(const types::Amount& additional_amount,
                         iso8583::Apdu& apdu);

void SetEmvData(const std::vector<uint8_t>& emv_data, iso8583::Apdu& apdu);

void SetInvoiceNumber(const uint32_t& invoice, iso8583::Apdu& apdu);

void SetCvv2Data(const std::string& cvv, iso8583::Apdu& apdu);

void SetPreAuthData(const Transaction& tx, iso8583::Apdu& apdu);

void SetInstalmentData(const Transaction& tx, iso8583::Apdu& apdu);

std::string GetProcessingCode(const iso8583::Apdu& apdu);

uint64_t GetAmount(const iso8583::Apdu& apdu);

uint32_t GetStan(const iso8583::Apdu& apdu);

stdx::optional<time_t> GetHostDatetime(const iso8583::Apdu& apdu);

uint32_t GetNii(const iso8583::Apdu& apdu);

std::string GetRrn(const iso8583::Apdu& apdu);

stdx::optional<std::string> GetAuthIdResponse(const iso8583::Apdu& apdu);

std::string GetResponseCode(const iso8583::Apdu& apdu);

std::string GetTid(const iso8583::Apdu& apdu);

stdx::optional<std::vector<uint8_t>> GetEmvData(const iso8583::Apdu& apdu);

bool HasDateTime(const iso8583::Apdu& apdu);

bool ValidateBasicFields(const iso8583::Apdu& request_apdu,
                         const iso8583::Apdu& response_apdu);

bool CheckMandatoryFields(const iso8583::Apdu& apdu,
                          const std::vector<int>& fields);

void UpdateSystemDateTime(const iso8583::Apdu& response_apdu);

void AddTableDataToField63(
                           const std::vector<uint8_t>& full_table,
                           iso8583::Apdu& apdu);

void UpdateTransactionInfoFromHostResponse(const iso8583::Apdu& response_apdu,
                                           Transaction& tx);

}

#endif
