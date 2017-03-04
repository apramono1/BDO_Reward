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
#include <bdor/redemption_workflow.h>

#include <utils/logger.h>
#include "online_pin_entry.h"
#include "remove_chip_card.h"
#include "floor_limit.h"
#include "reversal_processing.h"
#include "workflow_activities.h"
#include "app_counter.h"
#include <bdor/copy_type.h>
#include <bdor/reprint_type.h>
#include "post_card_validation.h"

namespace bdor {

RedemptionWorkflow::RedemptionWorkflow(ApplicationSettings& app_settings,
        Batch& batch,
        HostSwitch& host,
        emv::EmvConfig& emv_config,
        contactless::ClessConfig& cless_config,
        tpcore::PinEntry& pin_entry,
        Ui& ui,
        Receipts& receipts)

    : app_settings_(app_settings),
      batch_(batch),
      host_(host),
      emv_config_(emv_config),
      cless_config_(cless_config),
      pin_entry_(pin_entry),
      ui_(ui),
      receipts_(receipts){
}


bool RedemptionWorkflow::Start(types::PosConditionCode pos_condition_mode) {

  logger::debug("Starting Redemption Transaction");

  if (IsMemoryFull()) {
    ui_.MessageBatchFull();
  }

  Transaction tx;
  tx.transaction_type = TransactionType::REWARD;
  tx.pos_condition_code = pos_condition_mode;

  TerminalConfiguration terminal_config =
      app_settings_.GetTerminalConfiguration();

  stdx::optional<CardDefinition> card_definition_config;
  stdx::optional<IssuerDefinition> issuer_definition_config;
  stdx::optional<HostDefinition> host_definition_config;

  if (!CheckAndOpenBatch(tx.host_index, ui_, batch_))
      return false;

    tx.online_authorization_required = true;

    host_.Disconnect();
    if (!host_.PreConnect(host_definition_config->index)) {  // Wrong comm config
      ui_.MessageCommunicationError();
      return false;
    }

    tx.batch_num = batch_.GetCurrentBatchNumber(host_definition_config->index);
    tx.invoice_num = GetNextUniqueInvoice(batch_);

    ui_.MessageCommunicatingHost();

    if (!ProcessPendingReversals(batch_, host_, host_definition_config->index)) {
      SetInvoiceNo(tx.invoice_num);
      ui_.MessagePleaseTryAgain();
      return false;
    }

    tx.SetTransactionStatus(TransactionStatus::IN_PROGRESS);

    tx.stan = GetNextStanNo();  // get STAN after processing reversal, in case if acq host requires increase STAN for every reversal attempt.

    if (!batch_.Insert(tx)) {
      SetInvoiceNo(tx.invoice_num);
      ui_.MessageProcessAborted();  // Batch Error
      return false;
    }

    host_.AuthorizeRedemption(tx);
     //ProcessPostCardNonEMVValidation(tx, batch_, acquirer_, ui_);

    if (tx.transaction_status == TransactionStatus::APPROVED){

    PrintReceipts(
      app_settings_, ui_,
      tx.transaction_status,
      *receipts_.GetReversalReceipt(app_settings_, tx),
      *receipts_.GetPreauthReceipt(app_settings_, tx, CopyType::MERCHANT,
                                   ReprintType::ORIGINAL),
      *receipts_.GetPreauthReceipt(app_settings_, tx, CopyType::CUSTOMER,
                                   ReprintType::ORIGINAL),
      *receipts_.GetPreauthReceipt(app_settings_, tx, CopyType::MERCHANT,
                                   ReprintType::DUPLICATE),
      *receipts_.GetPreauthReceipt(app_settings_, tx, CopyType::CUSTOMER,
                                   ReprintType::DUPLICATE)
                                   );
  }

  WaitForCardRemoval(ui_);
}}
