/*
 ------------------------------------------------------------------------------
 INGENICO Technical Software Department
 ------------------------------------------------------------------------------
 Copyright (c) 2016 INGENICO S.A.
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
#include <bdor/void_workflow.h>

#include <utils/logger.h>
#include <bdor/transaction.h>
#include <bdor/receipts.h>
#include <bdor/transaction_type_name.h>
#include "show_transaction_info.h"
#include "workflow_activities.h"
#include "reversal_processing.h"
#include "advice_processing.h"
#include "app_counter.h"
#include <bdor/copy_type.h>
#include <bdor/reprint_type.h>

namespace bdor {

VoidWorkflow::VoidWorkflow(ApplicationSettings& app_setting,
                           Batch& batch,
                           HostSwitch& host,
                           Ui& ui,
                           Receipts& receipts)
    : app_settings_(app_setting),
      batch_(batch),
      host_(host),
      ui_(ui),
      receipts_(receipts) {
}

void VoidWorkflow::Start() {
  logger::debug("Starting Void Transaction");

  //Check whether Void is blocked at terminal level
  if (!app_settings_.managed_settings_->terminal_config.TxVoidEnabled())
    return ui_.MessageTransactionNotAllowed();

  auto tx = RetrieveTransactionInfo(app_settings_, batch_, ui_,
                                    Ui::SEARCH_BY_INVOICE_NUMBER);
  if (tx == nullptr)
    return;

  if (!CheckTransactionTypeAndStatus(*tx))
    return;

  if (!ui_.DisplayTransactionInfo(GetTransactionTypeName(*tx),
                                  *tx->GetTotalAmount()))
    return;

  //Check whether Void is blocked at card/issuer level
  stdx::optional<CardDefinition> card_definition_config =
      app_settings_.managed_settings_->GetCardDefinition(tx->card_index);

  stdx::optional<HostDefinition> host_config;

  if (tx->transaction_type == TransactionType::INSTALMENT_SALE) {

    host_config = app_settings_.managed_settings_->GetHostDefinition(
        tx->host_index);
  }
  else {
    host_config = app_settings_.managed_settings_->GetHostDefinition(
        card_definition_config->host_index);
  }

  if (!card_definition_config->tx_void_enabled) {
    return ui_.MessageTransactionNotAllowed();
  }

  if (IsTxOfflineVoidable(*tx)) {
    tx->SetTransactionAsVoided();
    batch_.InsertOrUpdate(*tx);
    ui_.MessageApproved("OFFLINE VOIDED");
    RunPrintReceiptFlow(*tx);
    return;
  } else {
    ui_.MessageCommunicatingHost();
    HostSwitch::Status status;
    if (host_.PreConnect(host_config->index)) {

      // Reversals
      if (!ProcessPendingReversals(batch_, host_, host_config->index)) {
        host_.Disconnect();
        return ui_.MessagePleaseTryAgain();
      }

      //Perform Void
      tx->SetTransactionStatus(TransactionStatus::IN_PROGRESS);
      tx->SetTransactionInProgressStatus(InProgressStatus::IN_PROGRESS_VOID);
      tx->stan = GetNextStanNo();
      batch_.InsertOrUpdate(*tx);

      status = host_.PerformVoid(*tx);
      if (status != HostSwitch::Status::COMPLETED) {
        host_.Disconnect();
        tx->SetTransactionStatus(TransactionStatus::TO_REVERSE);
        batch_.InsertOrUpdate(*tx);
        ui_.MessagePleaseTryAgain();
        return;
      }

      // comm successful here
      if (tx->response_code != "00") {
        host_.Disconnect();
        //  tx->RevertToPreviousTransactionStatus();
        batch_.InsertOrUpdate(*tx);
        ui_.DisplayResponseMessage(tx->response_code);
        return;
      }

      //Piggyback
      if (app_settings_.managed_settings_->terminal_config.PiggybackEnabled())
        ProcessAllPendingAdvices(batch_, host_, host_config->index);

      host_.Disconnect();

      tx->SetTransactionAsVoided();
      batch_.InsertOrUpdate(*tx);

      ui_.MessageApproved(tx->auth_id_response);
      RunPrintReceiptFlow(*tx);
    }

  }

}

void VoidWorkflow::RunPrintReceiptFlow(const Transaction& tx) {

  PrintReceipts(
                app_settings_,
                ui_,
                tx.transaction_status,
                *receipts_.GetReversalReceipt(app_settings_, tx),
                *receipts_.GetVoidReceipt(app_settings_, tx, CopyType::MERCHANT,
                                          ReprintType::ORIGINAL),
                *receipts_.GetVoidReceipt(app_settings_, tx, CopyType::CUSTOMER,
                                          ReprintType::ORIGINAL),
                *receipts_.GetVoidReceipt(app_settings_, tx, CopyType::MERCHANT,
                                          ReprintType::DUPLICATE),
                *receipts_.GetVoidReceipt(app_settings_, tx, CopyType::CUSTOMER,
                                          ReprintType::DUPLICATE)
                                          );
}

bool VoidWorkflow::IsTxOfflineVoidable(const Transaction& tx) const {
  if (tx.transaction_status == TransactionStatus::TO_ADVISE &&
      !tx.is_adjusted) {
    return true;
  }

  return false;
}

bool VoidWorkflow::CheckTransactionTypeAndStatus(const Transaction& tx) const {

  if (tx.transaction_type == TransactionType::PREAUTH
      || tx.transaction_type == TransactionType::PREAUTH_CANCELLATION) {
    //Need to Add as per specification
    //|| tx.transaction_type== TransactionType::INSTALMENT_SALE) {
    ui_.MessageTransactionNotAllowed();
    return false;
  }

  //original transaction not successful
  if ((tx.transaction_status == TransactionStatus::TO_REVERSE
      || tx.transaction_status == TransactionStatus::IN_PROGRESS)
      && tx.in_progress_status == InProgressStatus::IN_PROGRESS_NONE) {
    ui_.MessageTransactionNotFound();
    return false;
  }

  if (tx.is_voided) {
    ui_.MessageTransactionAlreadyVoided();
    return false;
  }
  return true;
}

}
