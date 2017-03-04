/*
 ------------------------------------------------------------------------------
 INGENICO Technical Software Department
 ------------------------------------------------------------------------------
 Copyright (c) 2015-2016 INGENICO S.A.
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
#ifndef BDOR__CONTACTLESS_UTILS_H_
#define BDOR__CONTACTLESS_UTILS_H_

#include <contactless/cless.h>
#include <bdor/transaction_type.h>
#include <bdor/terminal_configuration.h>

namespace bdor {
bool IsContactlessEnabled(TransactionType transaction_type,
                          TerminalConfiguration& terminal_config);

contactless::Cless::TransactionType GetClessTransactionType(
    TransactionType transaction_type);
}

#endif
