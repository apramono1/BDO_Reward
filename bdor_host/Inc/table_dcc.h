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
#ifndef BDOR__TABLE_DCC_H_
#define BDOR__TABLE_DCC_H_

#include <string>
#include <types/amount.h>

#include <bdor/transaction_type.h>
#include <bdor/transaction.h>
#include <bdor/transaction_status.h>

namespace bdor {

std::vector<uint8_t> BuildDccStaffIdTable(const std::string& staff_id);

std::vector<uint8_t> BuildDccFetchTable(const TransactionType& tx_type,
                                        const DccProvider& provider);

std::vector<uint8_t> BuildDccAuthorizedTable(std::string dcc_rate,
                                             std::string margin);
bool ReadDccSingleRateData(const std::vector<uint8_t>& field_content,
                         Transaction& tx);

bool ReadDccEnquiryResponseData(const std::vector<uint8_t>& field_content,
                                Transaction& tx);

}
#endif
