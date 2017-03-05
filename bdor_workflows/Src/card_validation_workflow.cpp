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
#include <bdor/card_validation_workflow.h>

#include <utils/logger.h>
#include "online_pin_entry.h"
#include "remove_chip_card.h"
#include "workflow_activities.h"
#include <bdor/mag_workflow.h>
#include <bdor/manual_workflow.h>

namespace bdor {

CardValidationWorkflow::CardValidationWorkflow(ApplicationSettings& app_settings,
		                                       HostSwitch& host,
											   tpcore::PinEntry& pin_entry,
											   Ui& ui)
                       : app_settings_(app_settings),
                         host_(host),
                         pin_entry_(pin_entry),
                         ui_(ui) {}

void CardValidationWorkflow::Start() {
  logger::debug("Starting Card Validation");

  Transaction tx;
  tx.transaction_type = TransactionType::REWARD;
  tx.pos_condition_code = types::PosConditionCode::NORMAL;

  TerminalConfiguration terminal_config = app_settings_.GetTerminalConfiguration();
  if (!IsTxEnabled(terminal_config, tx))
	  return ui_.MessageTransactionNotAllowed();

  ui_.MessageEnterPanOrSwipeCard();

  std::vector<tpcore::TeliumEvent::EventType> events;
  events.push_back(tpcore::TeliumEvent::EVENT_KEYBOARD);
  events.push_back(tpcore::TeliumEvent::EVENT_TERMINAL_SWIPE);

  tpcore::TeliumEvent telium_event;
  tpcore::TeliumEvent::EventType event = telium_event.WaitForEvent(
    		                             events, app_settings_.managed_settings_->terminal_config.GetEventTimeout());

  bool flowStat;
  if (event == tpcore::TeliumEvent::EVENT_KEYBOARD)
	  flowStat = RunManualFlow(tx);

  else if (event == tpcore::TeliumEvent::EVENT_TERMINAL_SWIPE)
	  flowStat = RunMagFlow(telium_event.GetMagStripeContent(), tx);

  else if (event == tpcore::TeliumEvent::EVENT_TIMEOUT){
	  ui_.MessageProcessAborted(); flowStat = false;
  }

  if(!flowStat){
	  WaitForCardRemoval(ui_); return;
  }

  if (tx.transaction_status == TransactionStatus::APPROVED) //Is valid card
      ui_.MessageApproved(tx.auth_id_response);
}

bool CardValidationWorkflow::RunManualFlow(Transaction& tx) {

  ManualWorkflow manual_workflow(ui_, app_settings_);

  stdx::optional<CardDefinition> card_definition_config;
  stdx::optional<IssuerDefinition> issuer_definition_config;
  stdx::optional<HostDefinition> host_definition_config;

  while (true) {
	if (!manual_workflow.RequestPan(tx.pan))
		return false;

    if (!RetrieveConfigsFromPan(app_settings_, *tx.pan, card_definition_config, issuer_definition_config, host_definition_config, tx)) {
      ui_.MessageUnsupportedCard();
      continue;
    }

    if (!IsValidPAN(*card_definition_config, *tx.pan)) {
      ui_.MessageInvalidPan();
      continue;
    }
    break;
  }

  manual_workflow.ReadTransactionData(&tx);

  if (!IsTxEnabledPerCard(*card_definition_config, tx)) {
    ui_.MessageUnsupportedCard();
    return false;
  }

  if (!card_definition_config->manual_entry_enabled) {
    ui_.MessageManualNotAllowedForThisCard();
    return false;
  }

  SetTransactionDataFromConfigs(*card_definition_config, *issuer_definition_config, *host_definition_config, &tx);

  tx.online_authorization_required = true;

  host_.Disconnect();
  if (!host_.PreConnect(tx.host_index)){
    ui_.MessageCommunicationError();
    return false;
  }

  if (!DisplayPanAndExpiryDate(app_settings_, &ui_, tx.issuer_index, *tx.pan, tx.expiration_date))
    return false;;

  ui_.MessageCommunicatingHost();
  host_.ValidateCard(tx);
  //ProcessPostOnlineNonFullEmv(app_settings_, ui_, batch_, host_, tx);
  host_.Disconnect();

  return true;
}

bool CardValidationWorkflow::RunMagFlow(types::MagStripe mag_stripe, Transaction& tx) {

  MagWorkflow mag_workflow(ui_, app_settings_);

  ui_.MessageProcessingCard();

  if (!mag_workflow.PerformCardValidation(mag_stripe)) {
    ui_.MessageInvalidTrack2();
    return false;
  }

  mag_workflow.ReadTransactionData(mag_stripe, &tx);

  stdx::optional<CardDefinition> card_definition_config;
  stdx::optional<IssuerDefinition> issuer_definition_config;
  stdx::optional<HostDefinition> host_definition_config;

  if (!RetrieveConfigsFromPan(app_settings_, *tx.pan, card_definition_config, issuer_definition_config, host_definition_config, tx)){
    ui_.MessageUnsupportedCard();
    return false;
  }

  if (!IsValidPAN(*card_definition_config, *tx.pan)){
    ui_.MessageInvalidPan();
    return false;
  }

  if (!CheckCardExpired(ui_, tx.expiration_date))
    return false;

  if (!IsTxEnabledPerCard(*card_definition_config, tx)){
    ui_.MessageUnsupportedCard();
    return false;
  }

  SetTransactionDataFromConfigs(*card_definition_config, *issuer_definition_config, *host_definition_config, &tx);

  tx.online_authorization_required = true; //Need to modify if offline transaction is possible

  host_.Disconnect();
  if (!host_.PreConnect(host_definition_config->index)){
    ui_.MessageCommunicationError();
    return false;
  }

  if (!DisplayPanAndExpiryDate(app_settings_, &ui_, tx.issuer_index, *tx.pan, tx.expiration_date))
    return false;

  ui_.MessageCommunicatingHost();
  host_.ValidateCard(tx);
  //ProcessPostOnlineNonFullEmv(app_settings_, ui_, batch_, host_, tx);
  host_.Disconnect();

  return true;
}

}
