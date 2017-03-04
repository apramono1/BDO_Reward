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
#ifndef BDOR__REDEMPTION_WORKFLOW_H_
#define BDOR__REDEMPTION_WORKFLOW_H_

#include <types/magstripe.h>
#include <types/pos_condition_code.h>
#include <tpcore/pin_entry.h>
#include <emv/emv_config.h>
#include <bdor/application_settings.h>
#include <bdor/batch.h>
#include <bdor/host_switch.h>
#include <bdor/ui.h>
#include <bdor/receipts.h>
#include <bdor/emv_workflow.h>
#include <bdor/mag_workflow.h>
#include <bdor/manual_workflow.h>
#include <bdor/card_presenter.h>
#include <bdor/card_presenter_user.h>
#include <bdor/workflow_status.h>

namespace bdor {

class RedemptionWorkflow {

 public:
  RedemptionWorkflow(ApplicationSettings& app_settings,
                 Batch& batch,
                 HostSwitch& host,
                 emv::EmvConfig& emv_config,
                 contactless::ClessConfig& cless_config,
                 tpcore::PinEntry& pin_entry,
                 Ui& ui,
                 Receipts& receipts);

  bool Start(types::PosConditionCode pos_condition_code = types::PosConditionCode::NORMAL);

 private:
  ApplicationSettings& app_settings_;
  Batch& batch_;
  HostSwitch& host_;
  emv::EmvConfig& emv_config_;
  contactless::ClessConfig& cless_config_;
  tpcore::PinEntry& pin_entry_;
  Ui& ui_;
  Receipts& receipts_;

};

}

#endif
