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
#ifndef BDOR__RECEIPT_TRANSACTION_HEADER_H_
#define BDOR__RECEIPT_TRANSACTION_HEADER_H_

#include <string>
#include <receipt/goal_receipt.h>

namespace bdor {

class ReceiptTransactionHeader : public receipt::GoalReceipt {

 public:
  ReceiptTransactionHeader(tpcore::GoalHandle& gl,
                           unsigned printer_timeout,
                           bool is_reversal = false);

  void Expand(const Receipt::Map& dict) override;

 private:
  bool is_reversal_;
};

}

#endif
