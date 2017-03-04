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
#include <utils/logger.h>
#include "table.h"
#include <bdor/transaction.h>

namespace bdor {

static const std::string kEmiTableId = "EM";

std::vector<uint8_t> BuildInstalmentTable(const Transaction& tx) {

  // if (!tx.product_id.empty())

  std::stringstream emi_content_str;
  std::string rfu;

  std::stringstream product_amount;
  if (tx.amount.has_value())
    product_amount << tx.amount->GetValue();

  std::stringstream discount_amount;
  if (tx.secondary_amount.has_value())
    discount_amount << tx.secondary_amount->GetValue();

  std::stringstream tenure_str;
  tenure_str << tx.tenure;

  std::stringstream interest_rate_str;
  interest_rate_str << tx.interest_rate;

  std::stringstream roi_tenure_str;
  roi_tenure_str << tx.roi_tenure;

  std::stringstream interest_amount_str;
  if (tx.interest_amount.has_value())
    interest_amount_str << tx.interest_amount->GetValue();

  std::stringstream total_amount_str;
  if (tx.total_prod_amount.has_value())
    total_amount_str << tx.total_prod_amount->GetValue();

  std::stringstream emi_str;
  if (tx.emi_per_month.has_value())
    emi_str << tx.emi_per_month->GetValue();

  std::stringstream fee_str;
  fee_str << tx.processing_fee;

  emi_content_str << iso8583::RightAligned(product_amount.str(), 12, '0')
                  << iso8583::RightAligned(discount_amount.str(), 12, '0')
                  << iso8583::RightAligned(tenure_str.str(), 2, '0')
                  << iso8583::RightAligned(interest_rate_str.str(), 5, '0')
                  << iso8583::RightAligned(roi_tenure_str.str(), 5, '0')
                  << iso8583::RightAligned(interest_amount_str.str(), 12, '0')
                  << iso8583::RightAligned(total_amount_str.str(), 12, '0')
                  << iso8583::RightAligned(emi_str.str(), 12, '0')
                  << iso8583::RightAligned(fee_str.str(), 12, '0')
                  << iso8583::RightAligned(tx.program_id, 6, '0')
                  << iso8583::RightAligned(tx.product_id, 4, '0')
                  << iso8583::RightAligned(rfu, 10, ' ');

  std::string emi_content;
  emi_content = emi_content_str.str();

  std::vector<uint8_t> table_content(
      emi_content.c_str(), emi_content.c_str() + emi_content.length());

  logger::xdebug(table_content.data(), table_content.size());

  return BuildTable(kEmiTableId, table_content);
}

}
