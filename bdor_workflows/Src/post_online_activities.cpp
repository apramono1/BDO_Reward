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
#include "post_online_activities.h"
#include "app_counter.h"
#include "tc_processing.h"
#include "advice_processing.h"

namespace bdor {

bool IsResponseCodeApproved(const std::string& response_code) {
  std::vector<std::string> approve_codes = { "00", "10", "11",};
  return std::find(approve_codes.begin(), approve_codes.end(), response_code)
      != approve_codes.end();
}

bool IsHostResponseReceived(const Transaction& tx) {
  return !tx.response_code.empty();
}

void UploadPendingItemsAfterTxnApproved(const ApplicationSettings& app_settings,
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

void ProcessPostOnlineNonFullEmv(const ApplicationSettings& app_settings,
                                 Ui& ui,
                                 Batch& batch,
                                 HostSwitch& host,
                                 Transaction& tx) {
  tx.SetTransactionStatus(TransactionStatus::TO_REVERSE);
  batch.InsertOrUpdate(tx);

  if (!IsHostResponseReceived(tx)) {
    ui.MessagePleaseTryAgain();
  }
  else if (IsResponseCodeApproved(tx.response_code)) {
    tx.SetTransactionStatus(TransactionStatus::APPROVED);
    batch.InsertOrUpdate(tx);

    UploadPendingItemsAfterTxnApproved(app_settings, batch,
                                       host,
                                       tx.host_index);
  }
  else {
    batch.Delete(tx);
    SetInvoiceNo(tx.invoice_num);
    ui.DisplayResponseMessage(tx.response_code);
  }
}

void ProcessPostOnlineFullEmv(const ApplicationSettings& app_settings,
                              Ui& ui,
                              Batch& batch,
                              HostSwitch& host,
                              Transaction& tx,
                              EmvWorkflow& emv_workflow) {
  tx.SetTransactionStatus(TransactionStatus::TO_REVERSE);
  batch.InsertOrUpdate(tx);

  ui.MessageEMVProcessing();
  emv_workflow.PerformPostOnlineProcessing(tx);

  if (!tx.response_code.empty()
      && tx.transaction_status == TransactionStatus::APPROVED) {  // Second gen AC return TC

    tx.SetTcUploadStatus(TcUploadStatus::TC_UPLOAD_NEEDED);

    batch.InsertOrUpdate(tx);

    UploadPendingItemsAfterTxnApproved(app_settings, batch,
                                       host,
                                       tx.host_index);
    host.Disconnect();
    return;
  }

  if (IsHostResponseReceived(tx) && !IsResponseCodeApproved(tx.response_code)
      && tx.transaction_status != TransactionStatus::APPROVED) {  // Declined BY HOST
    batch.Delete(tx);
    SetInvoiceNo(tx.invoice_num);
    ui.DisplayResponseMessage(tx.response_code);
    return;
  }

  // Card Declined in PostOnlineProcessing
  if (tx.transaction_status != TransactionStatus::APPROVED) {
    tx.SetTransactionStatus(TransactionStatus::TO_REVERSE);
    SetInvoiceNo(tx.invoice_num);
    ui.MessagePleaseTryAgain();
  }

  batch.InsertOrUpdate(tx);
}
}
