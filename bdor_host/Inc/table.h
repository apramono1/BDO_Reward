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
#ifndef BDOR__TABLE_H_
#define BDOR__TABLE_H_

#include <string>
#include <vector>

namespace bdor {

std::vector<uint8_t> BuildTable(
                     const std::string& table_id,
                     const std::vector<uint8_t>& table_content);

std::vector<uint8_t> ReadFieldFromTable(
    const std::string& table_id,
    const std::vector<uint8_t>& field_content);

bool TableExisted(
    const std::string& table_id,
    const std::vector<uint8_t>& field_content);

}

#endif
