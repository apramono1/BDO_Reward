/*
 ------------------------------------------------------------------------------
 INGENICO Technical Software Department
 ------------------------------------------------------------------------------
 Copyright (c) 2014-2016 INGENICO S.A.
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
#include "post_card_validation.h"

#include "app_counter.h"
#include "tc_processing.h"
#include "advice_processing.h"
#include <lounge_usage.h>
#include <post_online_activities.h>
#include <bdor/acquirer.h>

namespace bdor {

bool IsResponseApproved(const std::string& response_code) {
  std::vector<std::string> approve_codes = { "22", "23", };
  return std::find(approve_codes.begin(), approve_codes.end(), response_code)
      != approve_codes.end();
}

bool IsResponseReceived(const Transaction& tx) {
  return !tx.response_code.empty();
}

void UploadPendingItemsAfterApproval(const ApplicationSettings& app_settings,
                                        Batch& batch,
                                        HostSwitch& host,
                                        unsigned int host_index) {
  // Only upload the current TC
  if (app_settings.managed_settings_->terminal_config.TcUploadEnabled()) {
    ProcessCurrentTc(batch, host, host_index);
  }

  if (app_settings.managed_settings_->terminal_config.PiggybackEnabled()) {
    ProcessAllPendingAdvices(batch, host, host_index);
  }
}

void ProcessPostCardNonEMVValidation(Transaction& tx, Batch& batch_, Acquirer& acquirer_, Ui& ui_) {
  tx.SetTransactionStatus(TransactionStatus::TO_REVERSE);
  batch_.InsertOrUpdate(tx);

  if (!IsResponseReceived(tx))
    ui_.MessagePleaseTryAgain();

  else if (IsResponseApproved(tx.response_code)){
    if(tx.response_code == "22") {//Invalid card
    	batch_.Delete(tx);
    	SetInvoiceNo(tx.invoice_num);
    	tx.SetTransactionStatus(TransactionStatus::DECLINED);
    	ui_.DisplayResponseMessage(tx.response_code);
    }

    LoungeUsageMenuManager(acquirer_, ui_).ProcessValidCard(tx);

    if(tx.transaction_status == TransactionStatus::APPROVED){
    	batch_.InsertOrUpdate(tx);
    	//UploadPendingItemsAfterApproval(app_setting_,batch_,host_,tx.host_index);
    }
  }
}

void ProcessPostCardEMVValidation(Transaction& tx, EmvWorkflow& emv_workflow, Batch& batch_, Ui& ui_, Acquirer& acquirer_, HostSwitch& host_) {
  tx.SetTransactionStatus(TransactionStatus::TO_REVERSE);
  batch_.InsertOrUpdate(tx);

  ui_.MessageEMVProcessing();
  emv_workflow.PerformPostOnlineProcessing(tx);

  if (!tx.response_code.empty()
      && tx.transaction_status == TransactionStatus::APPROVED) {  // Second gen AC return TC

    tx.SetTcUploadStatus(TcUploadStatus::TC_UPLOAD_NEEDED);

    batch_.InsertOrUpdate(tx);

    /* UploadPendingItemsAfterApproval(app_setting_, batch_,
                                       host_,
                                       tx.host_index); */
    host_.Disconnect();
    return;
  }

  if (IsResponseReceived(tx) && !IsResponseApproved(tx.response_code)
      && tx.transaction_status != TransactionStatus::APPROVED) {  // Declined BY HOST
    batch_.Delete(tx);
    SetInvoiceNo(tx.invoice_num);
    ui_.DisplayResponseMessage(tx.response_code);
    return;
  }

  // Card Declined in PostOnlineProcessing
  if (tx.transaction_status != TransactionStatus::APPROVED) {
    tx.SetTransactionStatus(TransactionStatus::TO_REVERSE);
    SetInvoiceNo(tx.invoice_num);
    ui_.MessagePleaseTryAgain();
  }

  batch_.InsertOrUpdate(tx);
}
}
