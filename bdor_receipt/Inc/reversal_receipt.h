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
#ifndef BDOR__REVERSAL_RECEIPT_H_
#define BDOR__REVERSAL_RECEIPT_H_

#include <receipt/goal_composite_receipt.h>

namespace bdor {

class ReversalReceipt : public receipt::GoalCompositeReceipt {

  struct Body : receipt::GoalReceipt {
    Body(tpcore::GoalHandle& gl, unsigned  printer_timeout);
    void Expand(const Receipt::Map& dict) override;
  };

 public:
  ReversalReceipt(tpcore::GoalHandle& gl,
                  unsigned printer_timeout);
};

} 

#endif
