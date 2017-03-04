/*
 ------------------------------------------------------------------------------
 INGENICO Technical Software Department
 ------------------------------------------------------------------------------
 Copyright (c) 2016 INGENICO S.A.
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
#ifndef BDOR__REPRINT_RECEIPT_H_
#define BDOR__REPRINT_RECEIPT_H_

#include <bdor/receipts.h>
#include <bdor/ui.h>
#include <bdor/managed_settings.h>

namespace bdor {

class ReprintLastSettlementWorkflow {
 public:

  ReprintLastSettlementWorkflow(ApplicationSettings& app_setting, Ui& ui,
                                Receipts& receipts);

  bool Start();

 private:
  void PrintDuplicateHeader();

  ApplicationSettings& app_setting_;
  Ui& ui_;
  Receipts& receipts_;
};
}
#endif
