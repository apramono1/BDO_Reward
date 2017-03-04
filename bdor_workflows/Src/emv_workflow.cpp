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
#include <bdor/emv_workflow.h>

#include <utils/logger.h>
#include <utils/converter.h>
#include <utils/strings.h>
#include "workflow_activities.h"
#include "EmvLib_Tags.h"

namespace bdor {

using emvapi::EmvApi;

namespace {

EmvApi::AuthorizationResult GetAuthorizationResult(
    const std::string& response_code) {

  if (response_code.empty())
    return EmvApi::AuthorizationResult::UNABLE_TO_GO_ONLINE;

  if (response_code != "00")
    return EmvApi::AuthorizationResult::DECLINED;

  return EmvApi::AuthorizationResult::APPROVED;
}

}

EmvWorkflow::EmvWorkflow(Ui& ui,
                         ApplicationSettings& app_setting,
                         emv::EmvConfig& emv_config)
    : ui_(ui),
      app_setting_(app_setting),
      emv_api_(stdx::make_unique<EmvApi>(emv_config,
                                         EmvApi::CardReader::READER_TERMINAL)) {
}

EmvWorkflow::~EmvWorkflow() {
}

WorkflowStatus EmvWorkflow::PerformApplicationSelection(Transaction& tx) {
  EmvApi::Status status;

  ui_.MessageSelectionInProgress();
  emvapi::EmvCandidateList emv_candidate_list;
  status = emv_api_->PerformApplicationSelection(emv_candidate_list);

  if (status == EmvApi::EMV_CARD_BLOCKED) {
      logger::error("PerformApplicationSelection Card Blocked");
      ui_.MessageCardBlocked();
      return WorkflowStatus::EMV_CARD_BLOCKED;
    }

  if (status != EmvApi::EMV_SUCCESS) {
    logger::error("PerformApplicationSelection FAILED");
    ui_.MessageEMVReadCardError();
    return WorkflowStatus::EMV_FALLBACK_NEEDED;
  }

  std::vector<emv::EmvCardApplication> app_list =
      emv_candidate_list.GetAllApplicationsOrderedByPriority();
  logger::debug("Number of application in Candidate List: %d",
                app_list.size());
  if (app_list.size() == 0) {
    ui_.MessageEMVCandidateListEmpty();
    return WorkflowStatus::EMV_FALLBACK_NEEDED;
  }

  // select application
  emv::EmvCardApplication* app = nullptr;
  if (app_list.size() == 1) {
    if (app_list[0].IsManualConfirmationRequired()) {
      logger::debug("Asking for App Selection confirmation");
      app = AskForApplicationSelection(app_list);
    } else {
      app = app_list.data();
    }

  } else if (app_list.size() > 1) {
    logger::debug("Asking for App Selection confirmation");
    app = AskForApplicationSelection(app_list);
  }

  if (app == nullptr) {
    ui_.MessageProcessAborted();
    logger::debug("No application selected");
    return WorkflowStatus::EMV_CANCELLED;
  }

  status = emv_api_->PerformInit();
  if (status != EmvApi::EMV_SUCCESS) {
    logger::error("PerformInit FAILED");
    ui_.MessageEMVReadCardError();
    return WorkflowStatus::EMV_FALLBACK_NEEDED;
  }

  std::string pse_language_preference;
  if (emv_candidate_list.pse_language_preference)
    pse_language_preference = *(emv_candidate_list.pse_language_preference);

  status = emv_api_->PerformFinalSelect(*app, pse_language_preference);

  if (status != EmvApi::EMV_SUCCESS) {
    logger::error("PerformFinalSelect FAILED");
    ui_.MessageEMVReadCardError();
    return WorkflowStatus::EMV_FALLBACK_NEEDED;
  }

  tx.application_prefered_name = app->GetLabelToDisplayDuringSelection();

  return WorkflowStatus::EMV_SUCCESS;
}

WorkflowStatus EmvWorkflow::ReadApplicationData(Transaction& tx) {

  EmvApi::Status status;
  types::Amount additional_amount(
      (*GetEmvTransactionAmount(tx)).GetCurrencyName(), 0);

  status = emv_api_->PerformInitiateApplicationProcessing(
      *GetEmvTransactionAmount(tx),
      &additional_amount);
  if (status != EmvApi::EMV_SUCCESS) {
    logger::error("PerformInitiateApplicationProcessing FAILED");

    TransactionStatus txn_status = ProcessEmvApiOutcome(
        emv_api_->GetTransactionStatus(), status);

    tx.SetTransactionStatus(txn_status);

    if (status == EmvApi::EMV_CARD_ERROR) {
      ui_.MessageEMVReadCardError();
      return WorkflowStatus::EMV_FALLBACK_NEEDED;
    }
    else {
      ui_.MessageEMVReadCardError();
      return WorkflowStatus::EMV_ERROR;
    }
  }

  status = emv_api_->PerformReadApplicationData();
  if (status != EmvApi::EMV_SUCCESS) {
    logger::error("PerformInitiateApplicationProcessing FAILED");

    TransactionStatus txn_status = ProcessEmvApiOutcome(
        emv_api_->GetTransactionStatus(), status);

    tx.SetTransactionStatus(txn_status);

    if (status == EmvApi::EMV_CARD_ERROR) {
      ui_.MessageEMVReadCardError();
      return WorkflowStatus::EMV_FALLBACK_NEEDED;
    }
    else {
      ui_.MessageEMVReadCardError();
      return WorkflowStatus::EMV_ERROR;
    }
  }

  return WorkflowStatus::EMV_SUCCESS;
}

bool EmvWorkflow::ReadTransactionData(Transaction& tx) {

  tx.track2_equivalent_data = *emv_api_->GetTrack2EquivalentData();
  tx.pan = *emv_api_->GetPan();

  tx.expiration_date = tx.track2_equivalent_data->GetExpirationDate();

  auto pan_sequence_number = emv_api_->GetPanSequenceNumber();
  if (pan_sequence_number)
    tx.pan_sequence_number = utils::BcdToInt(*pan_sequence_number);
  tx.aid = *emv_api_->GetSelectedAid();
  tx.tx_datetime = *emv_api_->GetTransactionDateTime();

  tx.card_holder_name = emv_api_->GetCardholderName();

  return true;
}

WorkflowStatus EmvWorkflow::ReadICCData(Transaction& tx) {
  auto emv_status = PerformApplicationSelection(tx);
  if (emv_status != WorkflowStatus::EMV_SUCCESS) {
    return emv_status;
  }

  emv_status = ReadApplicationData(tx);
  if (emv_status != WorkflowStatus::EMV_SUCCESS) {
    return emv_status;
  }

  if (!ReadTransactionData(tx))
    return WorkflowStatus::EMV_ERROR;

  return WorkflowStatus::EMV_SUCCESS;
}

WorkflowStatus EmvWorkflow::FullEMVProcessingBeforeOnline(
    tpcore::PinEntry& pin_entry, Transaction& tx,
    const types::Amount* last_transaction_amount,
    EmvForceAction force_action) {

  ui_.MessageEMVProcessing();
  if (!PerformOfflineDataAuthenticationAndProcessingRestriction(
                                                                tx))
    return WorkflowStatus::EMV_ERROR;

  if (!PerformCardholderVerification(tx, &pin_entry))
    return WorkflowStatus::EMV_ERROR;

  if (!PerformActionAnalysis(tx, last_transaction_amount, force_action))
    return WorkflowStatus::EMV_ERROR;

  return WorkflowStatus::EMV_SUCCESS;
}

bool EmvWorkflow::PerformOfflineDataAuthenticationAndProcessingRestriction(
    Transaction& tx) {
  EmvApi::Status status;
  auto transaction_type = GetEmvTransactionType(tx.transaction_type);

  status = emv_api_->PerformOfflineDataAuthentication(tx.tid);
  if (status != EmvApi::EMV_SUCCESS) {
    logger::error("PerformOfflineDataAuthentication FAILED");

    TransactionStatus txn_status = ProcessEmvApiOutcome(
        emv_api_->GetTransactionStatus(), status);

    tx.SetTransactionStatus(txn_status);

    return false;
  }

  status = emv_api_->PerformProcessingRestriction(transaction_type);
  if (status != EmvApi::EMV_SUCCESS) {
    logger::error("PerformProcessingRestriction FAILED");

    TransactionStatus txn_status = ProcessEmvApiOutcome(
        emv_api_->GetTransactionStatus(), status);

    tx.SetTransactionStatus(txn_status);
    return false;
  }
  tx.tvr = emv_api_->GetTvr();
  tx.tsi = emv_api_->GetTsi();
  return true;
}

bool EmvWorkflow::PerformCardholderVerification(Transaction& tx,
                                                tpcore::PinEntry* pin_entry) {
  EmvApi::CardholderVerificationStatus cv_status;
  EmvApi::Status status;

  status = emv_api_->PerformCardholderVerificationFirst(cv_status);
  if (status != EmvApi::EMV_SUCCESS) {
    logger::error("PerformCardholderVerificationFirst FAILED");
    TransactionStatus txn_status = ProcessEmvApiOutcome(
        emv_api_->GetTransactionStatus(), status);

    tx.SetTransactionStatus(txn_status);
    return false;
  }

  while (cv_status.next_action
      != EmvApi::CardholderVerificationStatus::END) {

    types::PinEntryStatus pin_entry_status;
    int pin_length = 0;

    tpcore::PinEntryUI::uptr pin_entry_ui = ui_.GetPinEntryUI(
        tx.transaction_type, *GetEmvTransactionAmount(tx));

    if (cv_status.next_action
        == EmvApi::CardholderVerificationStatus::ENTER_OFFLINE_PIN) {

      int pin_remaining_attempts = emv_api_->GetPinTryCounter().value();

      pin_entry_status = pin_entry->AskForPinOffline(*pin_entry_ui,
                                                     pin_remaining_attempts,
                                                     &pin_length);
    } else {  // PIN ONLINE
      pin_entry_status = pin_entry->AskForPinOnline(*pin_entry_ui, 2,
                                                    &pin_length);
      if (pin_entry_status == types::PinEntryStatus::ENTRY_SUCCESS)
        tx.pin_entered = true;
      if (pin_entry_status != types::PinEntryStatus::ENTRY_SUCCESS)
        return false;
    }

    status = emv_api_->PerformCardholderVerificationNext(cv_status,
                                                         pin_entry_status,
                                                         pin_length);

    if (status != EmvApi::EMV_SUCCESS) {
      logger::error("PerformCardholderVerificationNext FAILED");
      TransactionStatus txn_status = ProcessEmvApiOutcome(
          emv_api_->GetTransactionStatus(), status);

      tx.SetTransactionStatus(txn_status);
      return false;
    }

    // display pin correct or wrong
    if (cv_status.result
        == EmvApi::CardholderVerificationStatus::PIN_OFFLINE_WRONG) {
      ui_.MessageIncorrectPin();
    }
    if (cv_status.result
        == EmvApi::CardholderVerificationStatus::PIN_OFFLINE_CORRECT) {
      tx.pin_entered = true;
      ui_.MessagePinOk();
    }
  }

  tx.signature_required = cv_status.signature_required;
  tx.tvr = emv_api_->GetTvr();
  tx.tsi = emv_api_->GetTsi();
  return true;
}

bool EmvWorkflow::PerformActionAnalysis(
    Transaction& tx,
    const types::Amount* last_transaction_amount,
    EmvForceAction force_action) {
  EmvApi::Status status;

  status = emv_api_->PerformTerminalRiskManagement(last_transaction_amount);
  if (status != EmvApi::EMV_SUCCESS) {
    logger::error("PerformTerminalRiskManagement FAILED");
    TransactionStatus txn_status = ProcessEmvApiOutcome(
        emv_api_->GetTransactionStatus(), status);

    tx.SetTransactionStatus(txn_status);
    return false;
  }

  EmvApi::ActionAnalysisResult terminal_action_analysis_result;
  status = emv_api_->PerformTerminalActionAnalysis(
      terminal_action_analysis_result);

  if (status != EmvApi::EMV_SUCCESS) {
    logger::error("PerformTerminalActionAnalysis FAILED");
    TransactionStatus txn_status = ProcessEmvApiOutcome(
        emv_api_->GetTransactionStatus(), status);

    tx.SetTransactionStatus(txn_status);
    return false;
  }

  if (force_action == EmvForceAction::DECLINE) {
    terminal_action_analysis_result =
        EmvApi::ActionAnalysisResult::AAC;  // useful for Refund
  } else if (force_action == EmvForceAction::ONLINE) {
    terminal_action_analysis_result =
        EmvApi::ActionAnalysisResult::ARQC;
  }

  EmvApi::ActionAnalysisResult card_action_analysis_result;
  status = emv_api_->PerformCardActionAnalysis(
      card_action_analysis_result,
      &terminal_action_analysis_result);
  if (status != EmvApi::EMV_SUCCESS) {
    logger::error("PerformCardActionAnalysis FAILED");
    TransactionStatus txn_status = ProcessEmvApiOutcome(
        emv_api_->GetTransactionStatus(), status);

    tx.SetTransactionStatus(txn_status);
    return false;
  }

  if (card_action_analysis_result
      == EmvApi::ActionAnalysisResult::ARQC
      || force_action == EmvForceAction::DECLINE) {
    tx.online_authorization_required = true;
  }
  else if (card_action_analysis_result
      == EmvApi::ActionAnalysisResult::TC) {
    if (tx.transaction_type != TransactionType::INSTALMENT_SALE) {
      tx.online_authorization_required = false;
      tx.SetTransactionStatus(TransactionStatus::TO_ADVISE);
    }
    else {
      //Installment Transaction Force Online
      tx.online_authorization_required = true;
    }
  }
  else if (card_action_analysis_result
      == EmvApi::ActionAnalysisResult::AAC) {
    tx.SetTransactionStatus(TransactionStatus::DECLINED);
    return false;
  }

  tx.icc_data = emv_api_->GetIccDataForHost();
  tx.transaction_certificate = utils::BytesToHexString(
      emv_api_->GetApplicationCryptogram());
  tx.tvr = emv_api_->GetTvr();
  tx.tsi = emv_api_->GetTsi();
  return true;
}

bool EmvWorkflow::PerformPostOnlineProcessing(Transaction& tx) {
  EmvApi::Status status;

  status = emv_api_->PerformOnlineProcessing(tx.issuer_emv_response);
  if (status != EmvApi::EMV_SUCCESS) {
    logger::error("PerformOnlineProcessing FAILED");

    TransactionStatus txn_status = ProcessEmvApiOutcome(
        emv_api_->GetTransactionStatus(), status);

    tx.SetTransactionStatus(txn_status);

    tx.icc_data = emv_api_->GetIccDataForHost();
    return false;
  }

  status = emv_api_->PerformIssuerToCardScriptsProcessing1( { });
  if (status != EmvApi::EMV_SUCCESS) {
    logger::error("PerformIssuerToCardScriptsProcessing1 FAILED");

    TransactionStatus txn_status = ProcessEmvApiOutcome(
        emv_api_->GetTransactionStatus(), status);

    tx.SetTransactionStatus(txn_status);

    tx.icc_data = emv_api_->GetIccDataForHost();
    return false;
  }

  EmvApi::AuthorizationResult authorization_result =
      GetAuthorizationResult(tx.response_code);
  std::string* response_code_ptr = nullptr;
  if (!tx.response_code.empty()) {
    response_code_ptr = &tx.response_code;
  }
  std::vector<uint8_t> auth_id_response_ptr;
  if (!tx.auth_id_response.empty()) {
    auth_id_response_ptr = utils::TextStringToBytes(
        tx.auth_id_response.c_str());
  }
  status = emv_api_->PerformCompletion(authorization_result,
                                       response_code_ptr,
                                       &auth_id_response_ptr);
  if (status != EmvApi::EMV_SUCCESS) {
    logger::error("PerformCompletion FAILED");
    TransactionStatus txn_status = ProcessEmvApiOutcome(
        emv_api_->GetTransactionStatus(), status);

    tx.SetTransactionStatus(txn_status);
    return false;
  }

  status = emv_api_->PerformIssuerToCardScriptsProcessing2( { });
  if (status != EmvApi::EMV_SUCCESS) {
    logger::error("PerformIssuerToCardScriptsProcessing2 FAILED");
    TransactionStatus txn_status = ProcessEmvApiOutcome(
        emv_api_->GetTransactionStatus(), status);

    tx.SetTransactionStatus(txn_status);
    tx.icc_data = emv_api_->GetIccDataForHost();
    return false;
  }

  TransactionStatus txn_status = ProcessEmvApiOutcome(
      emv_api_->GetTransactionStatus(), status);

  tx.SetTransactionStatus(txn_status);

  tx.icc_data = emv_api_->GetIccDataForHost();
  tx.tvr = emv_api_->GetTvr();
  tx.tsi = emv_api_->GetTsi();
  return true;
}

emv::EmvCardApplication* EmvWorkflow::AskForApplicationSelection(
    std::vector<emv::EmvCardApplication>& app_list) {

  std::map<std::string, emv::EmvCardApplication*> mapper;
  std::vector<std::string> menu_content;
  for (unsigned int i = 0; i < app_list.size(); i++) {
    mapper[app_list[i].GetLabelToDisplayDuringSelection()] = &app_list[i];
    menu_content.push_back(app_list[i].GetLabelToDisplayDuringSelection());
  }

  std::string item_selected = ui_.MenuSelection("Application Selection",
                                                menu_content);
  if (item_selected.empty()) {
    return nullptr;
  }

  return mapper[item_selected];
}

TransactionStatus EmvWorkflow::ProcessEmvApiOutcome(
    bool emv_api_transaction_status,
    EmvApi::Status emv_api_status) {

  TransactionStatus tx_status = TransactionStatus::TERMINAL_ERROR;
  if (emv_api_transaction_status) {
    logger::debug("Transaction APPROVED");
    tx_status = TransactionStatus::APPROVED;
  } else {
    switch (emv_api_status) {
      case EmvApi::Status::EMV_SUCCESS:
        logger::debug("Transaction DECLINED");
        tx_status = TransactionStatus::DECLINED;
        ui_.MessageDeclined();
        break;
      case EmvApi::Status::EMV_CARD_BLOCKED:
        logger::debug("CARD BLOCKED");
        tx_status = TransactionStatus::CARD_BLOCKED;
        ui_.MessageCardBlocked();
        break;
      case EmvApi::Status::EMV_CARD_ERROR:
        logger::debug("CARD ERROR");
        tx_status = TransactionStatus::CARD_ERROR;
        ui_.MessageCardError();
        break;
      case EmvApi::Status::EMV_CARD_REMOVED:
        logger::debug("CARD REMOVED");
        tx_status = TransactionStatus::CARD_REMOVED_FROM_READER;
        ui_.MessageCardRemoved();
        break;
      case EmvApi::Status::EMV_TERMINAL_ERROR:
        default:
        logger::debug("TERMINAL ERROR");
        tx_status = TransactionStatus::TERMINAL_ERROR;
        ui_.MessageProcessAborted();
        break;
    }
  }

  return tx_status;
}

EmvApi::TransactionType EmvWorkflow::GetEmvTransactionType(
    const TransactionType transaction_type) const {
  switch (transaction_type)
  {
    case TransactionType::SALE:
      return EmvApi::TransactionType::SALE;
    default:
      return EmvApi::TransactionType::SALE;
  }
}

stdx::optional<types::Amount> EmvWorkflow::GetEmvTransactionAmount(
    const Transaction& tx) const {
  if (tx.transaction_type == TransactionType::PREAUTH
      || tx.transaction_type == TransactionType::PREAUTH_CANCELLATION) {
    return *tx.GetTotalPreauthAmount();
  }
  return *tx.GetTotalAmount();
}

}

