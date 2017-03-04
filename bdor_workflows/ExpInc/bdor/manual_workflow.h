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
#ifndef BDOR__MANUAL_WORKFLOW_H_
#define BDOR__MANUAL_WORKFLOW_H_

#include <memory>

#include <bdor/ui.h>
#include <bdor/transaction.h>
#include <bdor/application_settings.h>

namespace bdor {

class ManualWorkflow {
 public:

  ManualWorkflow(Ui& ui, ApplicationSettings& app_setting);

  ~ManualWorkflow();

  typedef std::unique_ptr<ManualWorkflow> uptr;

  bool RequestPan(stdx::optional<types::Pan>& pan);
  bool AskAndValidateExpiryDate(std::string& expiry_date);
  void ReadTransactionData(Transaction* tx);

 private:
  Ui& ui_;
  ApplicationSettings& app_setting_;
};

}

#endif
