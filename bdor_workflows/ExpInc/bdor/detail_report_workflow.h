/*
 ------------------------------------------------------------------------------
 INGENICO Technical Software Department
 ------------------------------------------------------------------------------
 Copyright (c) 2015-2016 INGENICO S.A.
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
#ifndef BDOR__DETAIL_REPORT_WORKFLOW_H_
#define BDOR__DETAIL_REPORT_WORKFLOW_H_

#include <bdor/application_settings.h>
#include <bdor/batch.h>
#include <bdor/host.h>
#include <bdor/receipts.h>
#include <bdor/settlement_data.h>
#include <bdor/ui.h>

namespace bdor {

class DetailReportWorkflow {
 public:
	DetailReportWorkflow(ApplicationSettings& app_setting,
                     Batch& batch,
                     Ui& ui,
                     Receipts& receipts);

  void Start();

 private:
  ApplicationSettings& app_setting_;
  Batch& batch_;
  Ui& ui_;
  Receipts& receipts_;

};

}

#endif
