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
#include "table.h"

namespace bdor {

static const std::string kCvv2TableId = "16";

std::vector<uint8_t> BuildCvvTable(const std::string& cvv) {
  std::string table_content_str = iso8583::RightAligned(cvv, 6, '0');
  std::vector<uint8_t> table_content(
      table_content_str.c_str(),
      table_content_str.c_str() + table_content_str.length());

  return BuildTable(kCvv2TableId, table_content);
}

}
