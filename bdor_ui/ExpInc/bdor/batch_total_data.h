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
#ifndef BDOR__BATCH_TOTAL_DATA_H_
#define BDOR__BATCH_TOTAL_DATA_H_

#include <bdor/ui.h>
#include <types/amount.h>
#include <stdx/string>
#include <bdor/batch_data_types.h>

namespace bdor {

enum BatchTotalReturn{
  PRINT_REPORT,
  OTHER_KEY_PRESSED,
  ENTER_KEY_PRESSED,
  RESOURCE_NOT_FOUND
};

struct BatchTotalToDisplay {
  std::string host_name;
  std::string currency;
  std::string masked_expiry_date;
  BatchTotal sales_total;
  BatchTotal tips_total;
  BatchTotal refunds_total;
  BatchTotal offline_sales_total;
  BatchTotal void_sales_total;
  BatchTotal void_refunds_total;
  BatchTotal total;
};
}
#endif
