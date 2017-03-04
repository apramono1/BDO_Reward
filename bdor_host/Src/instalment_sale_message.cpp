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

#include "instalment_sale_message.h"
#include "sale_message.h"
#include "protocol.h"
#include "field_utils.h"
#include "apdu_utils.h"
#include <iso8583/field_types.h>
#include <iso8583/encoder.h>
#include <utils/converter.h>
#include <utils/logger.h>
#include <tpcore/telium_manager.h>
#include <iso8583/printer.h>
#include <tpcore/calendar.h>
#include "table.h"

static const std::string kEmiTableId = "EM";

using namespace types;

namespace bdor {

// PRIVATE DECLARATIONS
static void GetInstalmentData(const iso8583::Apdu& apdu, Transaction &tx);

// PUBLIC FUNCTIONS
iso8583::Apdu BuildInstalmentSaleRequest(const Transaction& tx) {
  iso8583::Apdu request_apdu = BuildSaleRequest(tx);

  SetInstalmentData(tx, request_apdu);

  return request_apdu;
}

bool ReadAndValidateInstalmentSaleResponse(const iso8583::Apdu& request_apdu,
                                           const std::vector<uint8_t>& data,
                                           Transaction& tx) {

  if (!ReadAndValidateSaleResponse(request_apdu, data, tx))
    return false;

  iso8583::Apdu response_apdu(bdor_spec(), data.data(),
                              data.size());

  if (!response_apdu.HasField(kField63))
    return false;

  GetInstalmentData(response_apdu, tx);

  return true;
}

// PRIVATE FUNCTIONS

static void GetInstalmentData(const iso8583::Apdu& apdu, Transaction &tx) {
  std::vector<uint8_t> output;
  int64_t value = 0;

  unsigned int pos = 0;

  if (apdu.HasField(kField63)) {
    auto field_content = apdu.GetFieldAsBytes(kField63);

    auto table_content = ReadFieldFromTable(kEmiTableId, field_content);
    if (table_content.size() > 1) {
      output = table_content;

      logger::xdebug(output.data(), output.size());

      std::string amount((const char*) (output.data() + pos), 12);
      logger::error(amount.c_str());
      value = utils::FromString<uint64_t>(amount);
      if (value)
        tx.amount->SetValue(value);
      pos += 12;

      std::string discount_amount((const char*) (output.data() + pos), 12);
      logger::error(discount_amount.c_str());
      value = 0;
      value = utils::FromString<uint64_t>(discount_amount);
      if (value)
        tx.secondary_amount->SetValue(value);
      pos += 12;

      std::string tenure((const char*) (output.data() + pos), 2);
      logger::error(tenure.c_str());
      tx.tenure = utils::FromString<uint64_t>(tenure);
      pos += 2;

      std::string interest_rate((const char*) (output.data() + pos), 5);
      logger::error(interest_rate.c_str());
      tx.interest_rate = utils::FromString<uint32_t>(interest_rate);
      pos += 5;

      std::string roi_tenure((const char*) (output.data() + pos), 5);
      logger::error(roi_tenure.c_str());
      tx.roi_tenure = utils::FromString<uint32_t>(roi_tenure);
      pos += 5;

      std::string currency_name = tx.amount->GetCurrencyName();

      std::string sinterest_amount((const char*) (output.data() + pos), 12);
      logger::error(sinterest_amount.c_str());
      types::Amount interest_amount(currency_name.c_str(), 0);
      value = 0;
      value = utils::FromString<uint64_t>(sinterest_amount);
      if (value)
      {
        interest_amount.SetValue(value);
        tx.interest_amount = interest_amount;
      }
      pos += 12;

      std::string stotal_prod_amount((const char*) (output.data() + pos), 12);
      logger::error(stotal_prod_amount.c_str());
      types::Amount total_prod_amount(currency_name.c_str(), 0);

      value = 0;
      value = utils::FromString<uint64_t>(stotal_prod_amount);
      if (value)
      {
        total_prod_amount.SetValue(value);
        tx.total_prod_amount = total_prod_amount;
      }
      pos += 12;

      std::string emi_per_month((const char*) (output.data() + pos), 12);
      logger::error(emi_per_month.c_str());
      types::Amount emi_amount(currency_name.c_str(), 0);
      value = 0;
      value = utils::FromString<uint64_t>(emi_per_month);
      if (value)
      {
        emi_amount.SetValue(value);
        tx.emi_per_month = emi_amount;
      }
      pos += 12;

      std::string processing_fee((const char*) (output.data() + pos), 12);
      logger::error(processing_fee.c_str());
      tx.processing_fee = utils::FromString<uint32_t>(roi_tenure);
      pos += 12;

    }
  }

  return;
}

}
