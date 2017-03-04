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

#include <iso8583/encoder.h>
#include <utils/converter.h>
#include "apdu_utils.h"
#include "table_dcc.h"
#include <bdor/transaction.h>
#include <iso8583/apdu.h>
#include "protocol.h"
#include <utils/logger.h>
#include "table.h"
#include "table_dcc.h"
#include <apdu_utils.h>
#include "dcc_message.h"

using namespace types;

namespace bdor {

// PRIVATE DECLARATIONS

static void SetDccStaffId(const Transaction& tx, iso8583::Apdu& apdu);

static void SetDccFetchData(const Transaction& tx, iso8583::Apdu& apdu);

static void SetDccAuthorizedData(const std::string& dcc_conversion_rate,
                                 const std::string& dcc_margin,
                                 iso8583::Apdu& apdu);

static std::string TransformConversionRate(const std::string& conversion_rate);

static const std::string kDefaultRate = "00001.00";
static const std::string kDefaultMargin = "0000.00";

// PUBLIC FUNCTIONS
void AddDccDataField(const Transaction& tx, iso8583::Apdu& request_apdu) {
  if (!tx.dcc_staff_id.empty()) {
    SetDccStaffId(tx, request_apdu);
  }

  if (tx.dcc_status == DccStatus::ENQUIRY) {
    SetDccFetchData(tx, request_apdu);
  }
  else if (tx.dcc_status == DccStatus::OPT_OUT) {
    SetDccAmount(*tx.amount, request_apdu);
    SetDccAuthorizedData(kDefaultRate, kDefaultMargin, request_apdu);
  }
  else if (tx.dcc_status == DccStatus::OPT_IN) {
    SetDccAmount(*tx.dcc_amount, request_apdu);
    SetDccConversionRate(tx.dcc_conversion_rate, request_apdu);
    SetDccCurrencyCode((*tx.dcc_amount).GetCurrencyCode(), request_apdu);
    SetDccAuthorizedData(tx.dcc_conversion_rate, tx.dcc_margin, request_apdu);
  }
}

void SetDccAmount(const types::Amount& dcc_amount, iso8583::Apdu& apdu) {
  apdu.SetField(kFieldDccAmount, dcc_amount.GetValue());
}

void SetDccConversionRate(const std::string& dcc_conversion_rate,
                                 iso8583::Apdu& apdu) {
  apdu.SetField(kFieldDccConversionRate,
                TransformConversionRate(dcc_conversion_rate));
}

void SetDccCurrencyCode(const uint32_t& currency_code,
                               iso8583::Apdu& apdu) {
  auto currency_code_str = utils::NumericPadded(currency_code, 3);
  apdu.SetField(kFieldCurrencyCode, currency_code_str);
}

std::vector<uint8_t> GetDccResponseField(const iso8583::Apdu& apdu) {
  return apdu.GetFieldAsBytes(kField63);
}

bool HasDccResponseField(const iso8583::Apdu& apdu) {
  return apdu.HasField(kField63);
}

// PRIVATE FUNCTIONS
// Example: "000.0155" => "40000155"
// TODO: put this function into unit tests
static std::string TransformConversionRate(const std::string& conversion_rate) {
  const uint32_t kRateSize = 8;

  if (conversion_rate.size() != kRateSize)
    logger::die_unless(false, "Wrong conversion format");

  auto dot_found = conversion_rate.find(".");
  if (dot_found == std::string::npos)
    logger::die_unless(false, "Wrong conversion format");

  auto offset_from_the_right = kRateSize - dot_found - 1;

  auto formatted_rate = utils::ToString(offset_from_the_right)
      + conversion_rate;

  return formatted_rate.erase(dot_found + 1, 1);
}

static void SetDccStaffId(const Transaction& tx, iso8583::Apdu& apdu) {
  auto dcc_table_data = BuildDccStaffIdTable(tx.dcc_staff_id);
  AddTableDataToField63(dcc_table_data, apdu);
}

static void SetDccFetchData(const Transaction& tx, iso8583::Apdu& apdu) {
  auto dcc_table_data = BuildDccFetchTable(tx.transaction_type,
                                           tx.dcc_provider);
  AddTableDataToField63(dcc_table_data, apdu);
}

static void SetDccAuthorizedData(const std::string& dcc_conversion_rate,
                                 const std::string& dcc_margin,
                                 iso8583::Apdu& apdu) {
  auto dcc_table_data = BuildDccAuthorizedTable(dcc_conversion_rate,
                                                dcc_margin);
  AddTableDataToField63(dcc_table_data, apdu);
}

}

