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
#ifndef BDOR__EMV_WORKFLOW_H_
#define BDOR__EMV_WORKFLOW_H_

#include <memory>
#include <tpcore/pin_entry.h>
#include <emv/emv_config.h>
#include <emv/emv_card_application.h>
#include <emvapi/emv_api.h>
#include <tpcore/cam_device_guard.h>
#include <bdor/transaction.h>
#include <bdor/ui.h>
#include <bdor/application_settings.h>
#include <bdor/workflow_status.h>

namespace bdor {

enum class EmvForceAction {
  DECLINE,
  ONLINE,
  NONE
};

class EmvWorkflow {
 public:
  EmvWorkflow(Ui& ui,
              ApplicationSettings& app_setting,
              emv::EmvConfig& emv_config);

  ~EmvWorkflow();

  WorkflowStatus PerformApplicationSelection(Transaction& tx);
  WorkflowStatus ReadApplicationData(Transaction& tx);
  bool PerformOfflineDataAuthenticationAndProcessingRestriction(
      Transaction& tx);
  bool ReadTransactionData(Transaction& tx);
  WorkflowStatus ReadICCData(Transaction& tx);
  WorkflowStatus FullEMVProcessingBeforeOnline(
      tpcore::PinEntry& pin_entry, Transaction& tx,
      const types::Amount* last_transaction_amount =
          nullptr,
      EmvForceAction force_action = EmvForceAction::NONE);
  bool PerformCardholderVerification(Transaction& tx,
                                     tpcore::PinEntry* pin_entry);
  bool PerformActionAnalysis(
      Transaction& tx,
      const types::Amount* last_transaction_amount =
          nullptr,
      EmvForceAction force_action = EmvForceAction::NONE);
  bool PerformPostOnlineProcessing(Transaction& tx);

 private:
  emvapi::EmvApi::TransactionType GetEmvTransactionType(
      const TransactionType transaction_type) const;
  stdx::optional<types::Amount> GetEmvTransactionAmount(const Transaction& tx) const;
  Ui& ui_;
  ApplicationSettings& app_setting_;
  emvapi::EmvApi::uptr emv_api_;

  emv::EmvCardApplication* AskForApplicationSelection(
      std::vector<emv::EmvCardApplication>& app_list);

  TransactionStatus ProcessEmvApiOutcome(
                                         bool emv_api_transaction_status,
                                         emvapi::EmvApi::Status emv_api_status);
};

}

#endif
