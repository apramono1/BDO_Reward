/*
 ------------------------------------------------------------------------------
 INGENICO Technical Software Department
 ------------------------------------------------------------------------------
 Copyright (c) 2014 INGENICO S.A.
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
#ifndef BDOR__DUPLICATE_RECEIPT_HEADER_H_
#define BDOR__DUPLICATE_RECEIPT_HEADER_H_

#include <receipt/goal_receipt.h>

namespace bdor {

class DuplicateReceiptHeader : public receipt::GoalReceipt {

 public:
  DuplicateReceiptHeader(tpcore::GoalHandle& gl, unsigned printer_timeout);
  void Expand(const Receipt::Map& dict) override;
};

}

#endif
