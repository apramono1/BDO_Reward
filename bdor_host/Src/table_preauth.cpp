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
#include "table.h"

namespace bdor {

static const std::string kPreAuthTableId = "PA";

std::vector<uint8_t> BuildPreAuthTable(const std::string& invoice_num,
                                       const std::string& last_four_digits,
                                       const std::string& auth_code) {
  std::string table_content_str = invoice_num + last_four_digits + auth_code;

  std::vector<uint8_t> table_content(
      table_content_str.c_str(),
      table_content_str.c_str() + table_content_str.length());

  return BuildTable(kPreAuthTableId, table_content);
}

}
