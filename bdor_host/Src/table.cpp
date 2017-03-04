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

#include <string>
#include <vector>
#include <utils/converter.h>
#include <utils/logger.h>
#include <iso8583/encoder.h>

namespace bdor {

const size_t kLenSize = 2;
const size_t kTableIdSize = 2;

std::vector<uint8_t> BuildTable(
                                const std::string& table_id,
                                const std::vector<uint8_t>& table_content) {

  logger::die_unless(table_id.size() == 2, "Invalid table ID: %s",
                     table_id.c_str());

  std::vector<uint8_t> data(table_id.c_str(),
                            table_id.c_str() + table_id.size());

  data.insert(data.end(), table_content.begin(), table_content.end());

  uint32_t data_len = data.size();
  std::string length = utils::NumericPadded(data_len, 4);
  std::vector<uint8_t> output = utils::HexStringToBytes(length, 4);

  data.insert(data.begin(), output.begin(), output.end());

  return data;
}

std::vector<uint8_t> ReadFieldFromTable(
    const std::string& table_id,
    const std::vector<uint8_t>& field_content) {

  std::vector<uint8_t> output;
  unsigned int pos = 0;

  while (pos < field_content.size()) {
    if (field_content.size() - pos < (kLenSize + kTableIdSize)) {
      break;
    }

    int table_size = iso8583::ReadBcd4(field_content.data() + pos);
    if (table_size < 2) {
      break;
    }

    std::string current_table_id(
        (const char*) (field_content.data() + pos + kLenSize),
        kTableIdSize);
    if (current_table_id == table_id) {
      output = std::vector<uint8_t>(
          field_content.data() + pos + kLenSize + kTableIdSize,
          field_content.data() + pos + kLenSize + table_size);
      break;
    }

    pos += kLenSize + table_size;
  }

  return output;
}


bool TableExisted(
    const std::string& table_id,
    const std::vector<uint8_t>& field_content) {

  std::vector<uint8_t> output;
    unsigned int pos = 0;

    while (pos < field_content.size()) {
      if (field_content.size() - pos < (kLenSize + kTableIdSize)) {
        break;
      }

      int table_size = iso8583::ReadBcd4(field_content.data() + pos);
      if (table_size < 2) {
        break;
      }

      std::string current_table_id(
          (const char*) (field_content.data() + pos + kLenSize),
          kTableIdSize);
      if (current_table_id == table_id) {
        return true;
      }

      pos += kLenSize + table_size;
    }

  return false;
}
}
