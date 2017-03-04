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
#include <settlement_all_hosts_receipt.h>
#include <stdx/memory>
#include <stdx/string>
#include <receipt_header.h>
#include <receipt_settlement_host.h>
#include <receipt_settlement_totals.h>
#include <receipt_settlement_version.h>
#include <receipt_formatter.h>

namespace bdor {

SettlementAllHostsReceipt::SettlementAllHostsReceipt(
    tpcore::GoalHandle& gl,
    unsigned printer_timeout,
    const std::vector<SingleHostSettlementData>& host_report_list)
    : GoalCompositeReceipt(gl, printer_timeout) {

  Add(stdx::make_unique<ReceiptHeader>(gl_, printer_timeout));
  for (const auto& report : host_report_list) {
    Add(stdx::make_unique<ReceiptSettlementHost>(gl_,
                                                 printer_timeout,
                                                 report.report.host_index,
                                                 report.report.batch_totals_list.size()));
  }
  Add(stdx::make_unique<ReceiptSettlementTotals>(gl_, printer_timeout, "GRAND", true));
  Add(stdx::make_unique<ReceiptSettlementVersion>(gl_, printer_timeout));
}

}
