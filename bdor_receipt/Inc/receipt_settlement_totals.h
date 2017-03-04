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
#ifndef BDOR__RECEIPT_SETTLEMENT_TOTALS_H_
#define BDOR__RECEIPT_SETTLEMENT_TOTALS_H_

#include <receipt/goal_receipt.h>
#include <stdx/string>

namespace bdor {

class ReceiptSettlementTotals : public receipt::GoalReceipt {
 public:
  ReceiptSettlementTotals(tpcore::GoalHandle& gl,
                          unsigned printer_timeout,
                          const std::string& host_index,
                          bool is_grand_total,
                          int index)
      : GoalReceipt(gl, printer_timeout),
        host_index_(host_index),
        is_grand_total_(is_grand_total),
        index_(stdx::to_string(index)) {
  }

  ReceiptSettlementTotals(tpcore::GoalHandle& gl,
                          unsigned printer_timeout,
                          const std::string& host_index,
                          bool is_grand_total)
      : GoalReceipt(gl, printer_timeout),
        host_index_(host_index),
        is_grand_total_(is_grand_total),
        index_("GRAND_TOTAL") {
  }

  void Expand(const Receipt::Map& dict) override;

 private:
  std::string host_index_;
  bool is_grand_total_;
  std::string index_;

};

}

#endif
