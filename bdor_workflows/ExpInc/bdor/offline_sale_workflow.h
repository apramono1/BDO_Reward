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
#ifndef BDOR__OFFLINE_SALE_WORKFLOW_H_
#define BDOR__OFFLINE_SALE_WORKFLOW_H_

#include <bdor/application_settings.h>
#include <bdor/batch.h>
#include <bdor/ui.h>
#include <bdor/receipts.h>
#include <emv/emv_config.h>
#include <contactless/cless_config.h>
#include <bdor/mag_workflow.h>
#include <bdor/manual_workflow.h>
#include <bdor/emv_workflow.h>
#include <bdor/workflow_status.h>
#include <bdor/card_presenter.h>
#include <bdor/card_presenter_user.h>
#include <types/pos_condition_code.h>

namespace bdor {

class OfflineSaleWorkflow : public CardPresenterUser {

 public:
  OfflineSaleWorkflow(ApplicationSettings& app_setting,
                      Batch& batch,
                      emv::EmvConfig& emv_config,
                      contactless::ClessConfig& cless_config,
                      Ui& ui,
                      Receipts& receipts);

  void Start();
  WorkflowStatus RunManualFlow(Transaction& tx) final;
  WorkflowStatus RunMagFlow(types::MagStripe mag_stripe, Transaction& tx) final;
  WorkflowStatus RunEmvFlow(Transaction& tx) final;
  WorkflowStatus RunClessFlow(ContactlessWorkflow& cless_workflow,
                              Transaction& tx) final;

 private:
  ApplicationSettings& app_settings_;
  Batch& batch_;
  emv::EmvConfig& emv_config_;
  contactless::ClessConfig& cless_config_;
  Ui& ui_;
  Receipts& receipts_;
  ManualWorkflow manual_workflow_;
  MagWorkflow mag_workflow_;
  CardPresenter card_presenter_;

  TransactionStatus ProcessOfflineSale(Transaction& tx);
  bool IsTxnEnabledPerCard(const Transaction& tx) const;
  bool IsValidOfflineSaleAmount(const types::Amount& amount) const;
  void RunPrintReceiptFlow(const Transaction& tx) const;
  bool GetAuthCode(std::string& auth_code);
  bool RequestTxAmount(Transaction& tx);
};
}

#endif
