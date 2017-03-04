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
#ifndef BDOR__FIELD_UTILS_H_
#define BDOR__FIELD_UTILS_H_

#include <string>
#include <vector>
#include <types/pos_entry_mode.h>
#include <types/pos_condition_code.h>
#include <bdor/transaction_type.h>

namespace bdor {

std::string GetProcessingCodeValue(TransactionType tx_type, bool is_voided_or_adjusted);
std::string GetProcessingCodeTransactionType(TransactionType tx_type, bool is_voided_or_adjusted);

std::string GetPosEntryModeValue(types::PosEntryMode pos_entry_mode);

uint32_t GetPosConditionCodeValue(types::PosConditionCode pos_condition_code);

void AddTableToField(std::vector<uint8_t>& field_content,
                     const std::string& table_id,
                     const std::vector<uint8_t>& table_content);

std::vector<uint8_t> ReadFieldFromTable(const std::string& table_id,
                                        const std::vector<uint8_t>& field_content);

bool IsTrack2ToBeSent(types::PosEntryMode pos_entry_mode);

}

#endif
