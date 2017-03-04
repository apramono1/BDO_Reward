/*
 ------------------------------------------------------------------------------
 INGENICO Technical Software Department
 ------------------------------------------------------------------------------
 Copyright (c) 2014-2016 INGENICO S.A.
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
#include "receipt_settlement_host.h"
#include <stdx/memory>
#include <stdx/string>

#include <receipt_settlement_header.h>
#include "receipt_settlement_totals.h"
#include "receipt_settlement_footer.h"
#include "receipt_formatter.h"

namespace bdor {

ReceiptSettlementHost::ReceiptSettlementHost(tpcore::GoalHandle& gl,
                                             unsigned printer_timeout,
                                             unsigned int host_index,
                                             int card_type_number)
    : GoalCompositeReceipt(gl, printer_timeout) {
  Add(stdx::make_unique<ReceiptSettlementHeader>(gl_, printer_timeout, host_index));
  for (int i = 0; i < card_type_number; i++) {
    Add(stdx::make_unique<ReceiptSettlementTotals>(gl_, printer_timeout, stdx::to_string(host_index), false, i));
  }
  Add(stdx::make_unique<ReceiptSettlementTotals>(gl_, printer_timeout, stdx::to_string(host_index), true));
  Add(stdx::make_unique<ReceiptSettlementFooter>(gl_, printer_timeout, host_index));

}

}
