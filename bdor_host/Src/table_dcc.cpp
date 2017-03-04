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

#include <map>
#include <iso8583/encoder.h>
#include <utils/converter.h>
#include "table.h"
#include <bdor/transaction_type.h>
#include <bdor/transaction.h>
#include <utils/get_default.h>
#include "table_dcc.h"
#include <utils/logger.h>
#include <bdor/transaction_status.h>

namespace bdor {

// PRIVATE MEMBERS
static std::string GetFetchTableId(const TransactionType& tx_type);
static std::string GetDccIndicator(const DccProvider& provider);
static bool ReadDccMultiRateData(const std::vector<uint8_t>& table_content,
                                 const uint32_t& start_pos,
                                 Transaction& tx);

static const std::string kDccFetchAuthTableId = "PS";
static const std::string kDccFetchSaleTableId = "DX";
static const std::string kDccFetchAdviceTableId = "EC";

static const std::string kDccAuthorizedTableId = "DC";
static const std::string kDccMultiRateTableId = "DE";

static const std::string kDccIndicatorFexco = "Y";
static const std::string kDccIndicatorPureCommerce = "2";
static const std::string kDccIndicatorFintrax = "3";

static const std::string kDccStaffIdTableId = "SI";

static const uint32_t kAmountLen_12 = 12;
static const uint32_t kConversionLen_8 = 8;
static const uint32_t kCurrencyLen_3 = 3;

// PUBLIC FUNCTIONS
std::vector<uint8_t> BuildDccStaffIdTable(const std::string& staff_id) {
  std::string table_content_str = staff_id;

  std::vector<uint8_t> table_content_bytes(
      table_content_str.c_str(),
      table_content_str.c_str() + table_content_str.length());

  return BuildTable(kDccStaffIdTableId, table_content_bytes);
}

std::vector<uint8_t> BuildDccFetchTable(
                                        const TransactionType& tx_type,
                                        const DccProvider& provider) {
  std::string table_content_str = GetDccIndicator(provider);

  std::vector<uint8_t> table_content_bytes(
      table_content_str.c_str(),
      table_content_str.c_str() + table_content_str.length());

  return BuildTable(GetFetchTableId(tx_type), table_content_bytes);
}

std::vector<uint8_t> BuildDccAuthorizedTable(std::string dcc_rate,
                                             std::string margin) {

  std::string table_content_str = dcc_rate + margin;

  std::vector<uint8_t> table_content_bytes(
      table_content_str.c_str(),
      table_content_str.c_str() + table_content_str.length());

  return BuildTable(kDccAuthorizedTableId, table_content_bytes);
}

bool ReadDccEnquiryResponseData(const std::vector<uint8_t>& field_content,
                                Transaction& tx) {
  if (TableExisted(kDccAuthorizedTableId, field_content)) {
    return ReadDccSingleRateData(field_content, tx);
  }
  else {
    return false;
  }
}

bool ReadDccSingleRateData(const std::vector<uint8_t>& field_content,
                           Transaction& tx) {

  static const uint32_t kExpectedDataLen = 61;
  static const uint32_t KResponseLen_30 = 30;
  static const uint32_t kMarginLen_7 = 7;
  static const uint32_t kLegLen_1 = 1;
  static const uint32_t kMultiRateFlagLen_1 = 1;
  static const std::string kDeTableIndicator = "2";
  static const uint32_t kLengthLen_2 = 2;
  static const uint32_t kTableIdLen_2 = 2;
  const std::string kDccLegRate = "1";

  auto table_content = ReadFieldFromTable(kDccAuthorizedTableId,
                                          field_content);

  if (table_content.size() < kExpectedDataLen)
    return false;

  auto pos = 0u;

  tx.dcc_response = std::string((const char*) table_content.data(),
                                KResponseLen_30);
  pos += KResponseLen_30;

  std::string dcc_amount_str = std::string(
      (const char*) (table_content.data() + pos), kAmountLen_12);
  pos += kAmountLen_12;

  tx.dcc_conversion_rate = std::string(
      (const char*) (table_content.data() + pos), kConversionLen_8);
  pos += kConversionLen_8;

  std::string dcc_currency = std::string(
      (const char*) (table_content.data() + pos), kCurrencyLen_3);
  pos += kCurrencyLen_3;

  tx.dcc_margin = std::string((const char*) (table_content.data() + pos),
                              kMarginLen_7);
  pos += kMarginLen_7;

  tx.dcc_leg = std::string((const char*) (table_content.data() + pos),
                           kLegLen_1);
  pos += kLegLen_1;

  int currency_code_int = utils::FromString<int>(dcc_currency);
  uint64_t dcc_amount_int = utils::FromString<uint64_t>(dcc_amount_str);

  tx.dcc_amount = types::Amount(currency_code_int, dcc_amount_int);

  if (tx.dcc_provider == DccProvider::PURE_COMMERCE && tx.dcc_leg == kDccLegRate) {
    if (field_content.size() > kExpectedDataLen + 4) {
      pos += kLengthLen_2 + kTableIdLen_2;
      tx.dcc_multi_rate_flag = std::string(
          (const char*) (field_content.data() + pos),
          kMultiRateFlagLen_1);
      pos += kMultiRateFlagLen_1;

      if (tx.dcc_multi_rate_flag == kDeTableIndicator) {
        pos += kTableIdLen_2;
        ReadDccMultiRateData(field_content, pos, tx);
      }
    }
  }

  logger::debug("DC Table From Host");
  logger::debug("Response: %s", tx.dcc_response.c_str());
  logger::debug("Amount: %s", dcc_amount_str.c_str());
  logger::debug("Conversion rate: %s", tx.dcc_conversion_rate.c_str());
  logger::debug("Currency: %s", dcc_currency.c_str());
  logger::debug("Margin: %s", tx.dcc_margin.c_str());
  logger::debug("Leg: %s", tx.dcc_leg.c_str());
  logger::debug("DCC amount: %s", (*tx.dcc_amount).ToString().c_str());

  return true;
}

// PRIVATE FUNCTIONS
static std::string GetFetchTableId(const TransactionType& tx_type) {
  static std::map<TransactionType, std::string> table_id_map =
      {
          { TransactionType::PREAUTH, kDccFetchAuthTableId },
          { TransactionType::SALE, kDccFetchSaleTableId },
          { TransactionType::PREAUTH_COMPLETION_OFFLINE,
              kDccFetchAdviceTableId },
      };

  return utils::GetDefault(table_id_map, tx_type, kDccFetchSaleTableId);
}

static std::string GetDccIndicator(
                                   const DccProvider& provider) {
  static std::map<DccProvider, std::string> indicator_map =
      {
          { DccProvider::FEXCO, kDccIndicatorFexco },
          { DccProvider::PURE_COMMERCE,
              kDccIndicatorPureCommerce },
          { DccProvider::FINTRAX,
              kDccIndicatorFintrax },
      };

  return utils::GetDefault(indicator_map, provider, kDccIndicatorFexco);
}

static bool ReadDccMultiRateData(const std::vector<uint8_t>& table_content,
                                 const uint32_t& start_pos,
                                 Transaction& tx) {
  const uint32_t kEuronetFlagLen_1 = 1;
  const uint32_t kMultiRateRecordCount_6 = 6;
  const uint32_t kRecordLen = kAmountLen_12 + kConversionLen_8 + kCurrencyLen_3;
  const uint32_t kExpectedDataLen = kEuronetFlagLen_1
      + kRecordLen * kMultiRateRecordCount_6;

  if (table_content.size() - start_pos < kExpectedDataLen)
    return false;

  auto pos = start_pos;

  auto euronet_flag = table_content[pos];
  pos += kEuronetFlagLen_1;

  logger::debug("DCC MultiRate Table From Host");
  logger::debug("Euronet Flag: %X", euronet_flag);

  for (auto i = 0u; i < kMultiRateRecordCount_6; i++) {
    auto dcc_amount_str = std::string(
        (const char*) (table_content.data() + pos), kAmountLen_12);
    pos += kAmountLen_12;

    if (dcc_amount_str == "            ") {
      pos += kConversionLen_8 + kCurrencyLen_3;
      continue;
    }

    auto dcc_amount_int = utils::FromString<uint64_t>(dcc_amount_str);

    auto dcc_conversion_rate = std::string(
        (const char*) (table_content.data() + pos), kConversionLen_8);
    pos += kConversionLen_8;

    tx.dcc_conversion_rate_list.push_back(dcc_conversion_rate);

    auto dcc_currency = std::string(
                                    (const char*) (table_content.data() + pos),
                                    kCurrencyLen_3);
    pos += kCurrencyLen_3;

    auto currency_code_int = utils::FromString<int>(dcc_currency);

    types::Amount dcc_amount = types::Amount(currency_code_int,
                                             dcc_amount_int);
    tx.dcc_amount_list.push_back(dcc_amount);

    logger::debug("Amount: %012u", dcc_amount_int);
    logger::debug("Currency: %03d", currency_code_int);
    logger::debug("Conversion rate: %s", dcc_conversion_rate.c_str());
    logger::debug("DCC amount: %s", dcc_amount.ToString().c_str());
  }

  return true;
}

}
