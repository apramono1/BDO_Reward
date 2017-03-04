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
#ifndef BDOR__SALE_WORKFLOW_H_
#define BDOR__SALE_WORKFLOW_H_

#include <cstdio>
#include <types/magstripe.h>
#include <emv/emv_config.h>
#include <contactless/cless_config.h>
#include <tpcore/pin_entry.h>
#include <tpcore/telium_event.h>
#include <bdor/application_settings.h>
#include <bdor/transaction.h>
#include <bdor/batch.h>
#include <bdor/ui.h>
#include <bdor/receipts.h>
#include <bdor/host_switch.h>
#include <bdor/card_presenter.h>
#include <bdor/card_presenter_user.h>
#include <types/pos_condition_code.h>
#include <bdor/workflow_status.h>
#include <bdor/emv_workflow.h>

namespace bdor {

class SaleWorkflow : public CardPresenterUser {

 public:
  SaleWorkflow(ApplicationSettings& app_settings,
               Batch& batch,
               HostSwitch& host,
               emv::EmvConfig& emv_config,
               contactless::ClessConfig& cless_config,
               tpcore::PinEntry& pin_entry,
               Ui& ui,
               Receipts& receipts);

  void Start(types::PosConditionCode pos_condition_code =
      types::PosConditionCode::NORMAL);
  Transaction Start(const types::Amount& amount,
             types::PosConditionCode pos_condition_code =
                 types::PosConditionCode::NORMAL);

  WorkflowStatus RunManualFlow(Transaction& tx) final;
  WorkflowStatus RunMagFlow(types::MagStripe mag_stripe, Transaction& tx) final;
  WorkflowStatus RunEmvFlow(Transaction& tx) final;
  WorkflowStatus RunClessFlow(ContactlessWorkflow& cless_workflow,
                              Transaction& tx) final;

 private:
  ApplicationSettings& app_settings_;
  Batch& batch_;
  HostSwitch& host_;
  emv::EmvConfig& emv_config_;
  contactless::ClessConfig& cless_config_;
  tpcore::PinEntry& pin_entry_;
  Ui& ui_;
  Receipts& receipts_;
  CardPresenter card_presenter_;
};

}

#endif
