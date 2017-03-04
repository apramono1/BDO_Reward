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
#ifndef BDOR__REWARD_WORKFLOW_H_
#define BDOR__REWARD_WORKFLOW_H_

#include <cstdio>
#include <stdx/memory>
#include <types/magstripe.h>
#include <emv/emv_config.h>
#include <contactless/cless_config.h>
#include <tpcore/pin_entry.h>
#include <tpcore/telium_event.h>
#include <bdor/lounge_usage.h>
#include <bdor/application_settings.h>
#include <bdor/transaction.h>
#include <bdor/batch.h>
#include <bdor/ui.h>
#include <bdor/goal_ui.h>
#include <bdor/receipts.h>
#include <bdor/host_switch.h>
#include <bdor/emv_workflow.h>
#include <bdor/mag_workflow.h>
#include <bdor/manual_workflow.h>
#include <bdor/card_presenter.h>
#include <bdor/card_presenter_user.h>
#include <bdor/workflow_status.h>
#include <bdor/acquirer.h>

namespace bdor {

class RewardWorkflow : public CardPresenterUser {

 public:
  RewardWorkflow(ApplicationSettings& app_settings,
               Batch& batch,
               HostSwitch& host,
			   Acquirer& acquirer,
               emv::EmvConfig& emv_config,
               contactless::ClessConfig& cless_config,
               tpcore::PinEntry& pin_entry,
               Ui& ui,
               Receipts& receipts,
			   Password& password);

  void Start(types::PosConditionCode pos_condition_mode = types::PosConditionCode::NORMAL);

  WorkflowStatus RunManualFlow(Transaction& tx) final;
  WorkflowStatus RunMagFlow(types::MagStripe mag_stripe, Transaction& tx) final;
  WorkflowStatus RunEmvFlow(Transaction& tx) final;
  WorkflowStatus RunClessFlow(ContactlessWorkflow& cless_workflow,
                               Transaction& tx) final;

 private:
  ApplicationSettings& app_settings_;
  Batch& batch_;
  HostSwitch& host_;
  Acquirer& acquirer_;
  emv::EmvConfig& emv_config_;
  contactless::ClessConfig& cless_config_;
  tpcore::PinEntry& pin_entry_;
  Ui& ui_;
  Receipts& receipts_;
  Password& password_;
  CardPresenter card_presenter_;
};

}

#endif
