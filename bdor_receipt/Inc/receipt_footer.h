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
#ifndef BDOR__RECEIPT_FOOTER_H_
#define BDOR__RECEIPT_FOOTER_H_

#include <receipt/goal_receipt.h>
#include <bdor/copy_type.h>
#include <bdor/reprint_type.h>

namespace bdor {

class ReceiptFooter : public receipt::GoalReceipt {

 public:
  ReceiptFooter(tpcore::GoalHandle& gl,
                        unsigned printer_timeout = 1000,
                        CopyType copy = CopyType::MERCHANT,
                        ReprintType reprint = ReprintType::ORIGINAL);
  void Expand(const Receipt::Map& dict) override;

 private:
   CopyType copy_;
   ReprintType reprint_;
};
}

#endif
