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
#ifndef BDOR__RECEIPT_DETAIL_TOTAL_H_
#define BDOR__RECEIPT_DETAIL_TOTAL_H_

#include <receipt/goal_receipt.h>
#include <stdx/string>
#include <bdor/Transaction.h>

namespace bdor {

class ReceiptDetailTotal : public receipt::GoalReceipt {
 public:
	ReceiptDetailTotal(tpcore::GoalHandle& gl,
						unsigned printer_timeout, int card_type_number)
      : GoalReceipt(gl, printer_timeout),
		card_type_number_(card_type_number)
 	 	{
		}

  void Expand(const Receipt::Map& dict) override;

private:
  int card_type_number_;

};

}

#endif
