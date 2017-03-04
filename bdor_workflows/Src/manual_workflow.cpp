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
#include <bdor/manual_workflow.h>
#include "workflow_activities.h"
namespace bdor {

ManualWorkflow::ManualWorkflow(Ui& ui,
                               ApplicationSettings& app_setting)
    : ui_(ui),
      app_setting_(app_setting) {
}

ManualWorkflow::~ManualWorkflow() {
}

bool ManualWorkflow::RequestPan(stdx::optional<types::Pan>& pan) {

  std::vector<uint8_t> pan_no;
  if (ui_.RequestPan(pan_no) == false)
    return false;

  pan = types::Pan(pan_no, false);
  return true;
}

bool ManualWorkflow::AskAndValidateExpiryDate(std::string& expiry_date_yymm) {
  if (ui_.RequestExpiryDate(expiry_date_yymm) == false)
    return false;

   return CheckCardExpired(ui_, expiry_date_yymm);
}

void ManualWorkflow::ReadTransactionData(Transaction* tx) {
  tx->pos_entry_mode = types::PosEntryMode::MANUAL;
  tx->signature_required = true;
}

}

