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
#ifndef BDOR__DCC_MESSAGE_H_
#define BDOR__DCC_MESSAGE_H_

#include <bdor/transaction.h>
#include <iso8583/apdu.h>

namespace bdor {

void AddDccDataField(const Transaction& tx, iso8583::Apdu& request_apdu);

void SetDccAmount(const types::Amount& dcc_amount, iso8583::Apdu& apdu);

void SetDccConversionRate(const std::string& dcc_conversion_rate,
                          iso8583::Apdu& apdu);

std::vector<uint8_t> GetDccResponseField(const iso8583::Apdu& apdu);

void SetDccCurrencyCode(const uint32_t& currency_code,
                               iso8583::Apdu& apdu);

bool HasDccResponseField(const iso8583::Apdu& apdu);

}

#endif
