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
#include <settlement_single_host_receipt.h>
#include <stdx/memory>
#include <stdx/string>
#include <receipt_header.h>
#include <receipt_settlement_host.h>
#include <receipt_settlement_version.h>
#include <receipt_formatter.h>

namespace bdor {

SettlementSingleHostReceipt::SettlementSingleHostReceipt(
    tpcore::GoalHandle& gl,
    unsigned printer_timeout,
    unsigned int host_index,
    int card_type_number)
    : GoalCompositeReceipt(gl, printer_timeout) {

  Add(stdx::make_unique<ReceiptHeader>(gl_, printer_timeout));
  Add(stdx::make_unique<ReceiptSettlementHost>(gl_, printer_timeout, host_index, card_type_number));
  Add(stdx::make_unique<ReceiptSettlementVersion>(gl_, printer_timeout));

}

}
