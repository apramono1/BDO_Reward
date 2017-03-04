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

#include <bdor/card_presenter.h>
#include "remove_chip_card.h"
#include "workflow_activities.h"
#include "contactless_utils.h"
#include <bdor/workflow_status.h>

namespace bdor {

CardPresenter::CardPresenter(ApplicationSettings& app_settings,
                             contactless::ClessConfig& cless_config,
                             Ui& ui)
    : app_settings_(app_settings),
      cless_config_(cless_config),
      ui_(ui) {
}

bool CardPresenter::PromptManualEntryForMoto(CardPresenterUser* wf,
                                             Transaction& tx) {
  if (wf->RunManualFlow(tx) != WorkflowStatus::MANUAL_SUCCESS)
    return false;
  return true;
}

bool CardPresenter::PromptCardToStart(
                                      CardPresenterUser* wf,
                                      Transaction& tx) {
  if (IsContactlessEnabled(tx.transaction_type,
                           app_settings_.GetTerminalConfiguration())) {
    cless_workflow_ = stdx::make_unique<ContactlessWorkflow>(
        cless_config_,
        app_settings_,
        ui_,
        GetClessTransactionType(tx.transaction_type),
        GetTotalAuthoriseAmount(tx));

    cless_workflow_->Initialize();
    cless_workflow_->StartDetection();
  }

  FallBackType fall_back_type = FallBackType::NONE;

  do {
    MessagePresentCard(tx, fall_back_type);

    tpcore::TeliumEvent telium_event;
    tpcore::TeliumEvent::EventType event = telium_event.WaitForEvent(
        GetEvents(tx, fall_back_type),
        app_settings_.managed_settings_->terminal_config.GetEventTimeout());

    if (event == tpcore::TeliumEvent::EVENT_KEYBOARD) {
      CancelOtherDetection(tx);
      if (wf->RunManualFlow(tx) != WorkflowStatus::MANUAL_SUCCESS)
        return false;

    } else if (event == tpcore::TeliumEvent::EVENT_TERMINAL_SWIPE) {
      CancelOtherDetection(tx);
      auto mag_status = wf->RunMagFlow(telium_event.GetMagStripeContent(), tx);
      if (mag_status == WorkflowStatus::CHIP_CARD_SWIPED)
        continue;
      else if (mag_status != WorkflowStatus::MAG_SUCCESS)
        return false;

    } else if (event == tpcore::TeliumEvent::EVENT_TERMINAL_CHIP) {
      CancelOtherDetection(tx);
      tx.is_fallback = false;
      auto emv_flow_status = wf->RunEmvFlow(tx);

      if (emv_flow_status == WorkflowStatus::EMV_FALLBACK_NEEDED) {
        if (!IsFallbackAllowed(app_settings_)) {
          ui_.MessagePleaseTryAgain();
          continue;
        }

        tx.is_fallback = true;
        fall_back_type = FallBackType::EMV_FALLBACK;

        if (ui_.AskToProceedWithFallback() == false) {
          WaitForCardRemoval(ui_);
          return false;
        }

        WaitForCardRemoval(ui_);
        continue;
      } else if (emv_flow_status != WorkflowStatus::EMV_SUCCESS) {
        WaitForCardRemoval(ui_);
        return false;
      }

    } else if (event == tpcore::TeliumEvent::EVENT_CONTACTLESS) {
      WorkflowStatus cless_flow_status = wf->RunClessFlow(*cless_workflow_,
                                                          tx);
      CancelOtherDetection(tx);

      if (cless_flow_status == WorkflowStatus::CLESS_FALLBACK_NEEDED) {
        fall_back_type = FallBackType::CLESS_FALLBACK;
        continue;

      } else if (cless_flow_status != WorkflowStatus::CLESS_SUCCESS) {
        return false;
      }

    } else if (event == tpcore::TeliumEvent::EVENT_TIMEOUT) {
      ui_.MessageProcessAborted();
      return false;
    }
    else {
      ui_.MessageEMVReadCardError();
      return false;
    }
    return true;
  } while (true);

  return true;
}

void CardPresenter::MessagePresentCard(
    const Transaction& tx,
    const FallBackType fall_back_type) const {

  if (fall_back_type == FallBackType::EMV_FALLBACK) {
      ui_.MessageInsertOrSwipeCard();

  } else if (!IsContactlessEnabled(tx.transaction_type,
                            app_settings_.GetTerminalConfiguration())) {
    ui_.MessageEnterPanOrSwipeCard();

  } else if (IsContactlessEnabled(tx.transaction_type,
                                  app_settings_.GetTerminalConfiguration())
      && fall_back_type == FallBackType::CLESS_FALLBACK) {
    ui_.MessageSwipeOrInsertOrTapCard();

  } else {
    ui_.MessageSwipeOrInsertOrTapCard();
  }
}

std::vector<tpcore::TeliumEvent::EventType> CardPresenter::GetEvents(
    const Transaction& tx, const FallBackType fall_back_type) const {

  std::vector<tpcore::TeliumEvent::EventType> events;

  events.push_back(tpcore::TeliumEvent::EVENT_KEYBOARD);

  events.push_back(tpcore::TeliumEvent::EVENT_TERMINAL_SWIPE);

  if (fall_back_type != FallBackType::EMV_FALLBACK)
    events.push_back(tpcore::TeliumEvent::EVENT_TERMINAL_CHIP);

  if (IsContactlessEnabled(tx.transaction_type,
                           app_settings_.GetTerminalConfiguration())
      && fall_back_type == FallBackType::NONE)
    events.push_back(tpcore::TeliumEvent::EVENT_CONTACTLESS);

  return events;
}

types::Amount CardPresenter::GetTotalAuthoriseAmount(
    const Transaction& tx) const {
  if (tx.transaction_type == TransactionType::PREAUTH
      || tx.transaction_type == TransactionType::PREAUTH_CANCELLATION) {
    return *tx.GetTotalPreauthAmount();
  }
  else {
    return *tx.GetTotalAmount();
  }
}

void CardPresenter::CancelOtherDetection(const Transaction& tx) const {
  if (IsContactlessEnabled(tx.transaction_type,
                           app_settings_.GetTerminalConfiguration()))
    cless_workflow_->CancelDetection();
}

}
