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
#ifndef BDOR__KEY_EXCHANGE_MESSAGE_H_
#define BDOR__KEY_EXCHANGE_MESSAGE_H_

#include <vector>
#include <bdor/transaction.h>
#include <iso8583/apdu.h>
#include <bdor/key_exchange.h>

namespace bdor {

iso8583::Apdu BuildKeyExchangeRequest(const KeyExchange& key_exchange);

bool ReadAndValidateKeyExchangeResponse(const iso8583::Apdu& request_apdu,
                                        const std::vector<uint8_t>& data,
                                        KeyExchange& key_exchange);
}

#endif
