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
#ifndef BDOR__GOAL_UI_H_
#define BDOR__GOAL_UI_H_

#include <bdor/ui.h>

#include <types/amount.h>
#include <types/pan.h>
#include <tpcore/pin_entry_ui.h>
#include <bdor/transaction_type.h>
#include <types/account_type.h>

#include <tpcore/goal_handle.h>

namespace bdor {

class GoalUi : public Ui {
 public:
  GoalUi(tpcore::GoalHandle& gl);
  unsigned long MenuSelectionIndex(const char* title,
                                   const std::vector<std::string> &menu)
                                       override;
  std::string MenuSelection(const char* title,
                            const std::vector<std::string> &menu) override;

  bool SelectAcquirer(unsigned int& host_index, bool show_all_acquirer)
      override;
  bool SelectAccountType(types::AccountType& account_type) override;
  bool SelectRecordSearchOption(RecordSearchOption& search_option) override;
  bool SelectHost(const std::vector<std::string>& host_name_list,
                  unsigned int& pos) override;
  bool SelectTransaction(const std::vector<TxReviewDetail>& tx_list,
                         int* index) override;
  bool SelectCurrency(const types::Amount& base_amount,
                      const types::Amount& dcc_amount,
                      const std::string& dcc_rate,
                      types::Amount& selected_amount) override;

  bool RequestInvoice(uint32_t& invoice_no) override;
  bool RequestTraceNumber(uint32_t& stan_no) override;
  bool RequestTid(std::string &terminal_id) override;

  bool RequestAmount(types::Amount& amount) override;
  bool RequestAmount(types::Amount& amount, const char* message,
                     bool is_zero_amount_allowed) override;
  bool RequestTipAmount(types::Amount& amount) override;
  bool RequestPreauthAmount(types::Amount& amount) override;
  bool RequestPreAuthCompletionAmount(types::Amount& amount) override;

  bool RequestPan(std::vector<uint8_t>& pan_no) override;
  bool RequestLast4DigitOfPan(std::string &last_4_digit) override;
  bool Request4DbcForAmexCard(std::string* get_4_dbc) override;
  bool RequestAtnOrPnrNo(std::string &atn_pnr) override;
  bool RequestHostName(std::string &ip) override;
  bool RequestPort(std::string &port) override;
  bool RequestExpiryDate(std::string& expiry_date) override;
  bool RequestCvv(std::string& cvv, bool bypass_allowed) override;
  bool RequestAuthorizationCode(std::string& auth_code) override;
  bool RequestStaffId(std::string& staff_id) override;

  bool DisplayTransactionInfo(const std::string &txn_type,
                              const types::Amount& amount) override;
  bool DisplayTransactionAndTipAmount(types::Amount& sale_amount,
                                      types::Amount& tip_amount) override;
  void MessageTipAdjustNotAllowed() override;
  void MessageProcessing() override;
  bool DisplayPanAndExpiryDate(const std::string& masked_pan,
                               const std::string& masked_expiry_date) override;

  bool AskToProceedWithFallback() override;
  bool AskToPrintCustomerCopy() override;
  bool AskToPrintDetailReport() override;
  bool AskToDeleteOldPreAuth() override;
  bool ConfirmDeleteBatch() override;

  virtual tpcore::PinEntryUI::uptr GetPinEntryUI(TransactionType tx_type,
                                                 types::Amount amount) override;
  bool SelectInstalmentPlan(
      const std::vector<std::string>& instalment_name_list, unsigned int& pos)
          override;

  bool SelectInstalmentOption(InstalmentOption& instalment_option) override;
  void MessageEMVProcessing() override;
  void MessageEMVProcessingError() override;
  void MessageExcessGuest() override;
  void MessageSelectionInProgress() override;
  void MessageInsertPaperRoll() override;
  void MessageUnknownError() override;
  void MessageParameterNotAvailable() override;
  void MessageSaleAlreadyCompleted() override;
  void MessagePleaseTryAgain() override;
  void MessagePrintingMerchantCopy() override;
  void MessagePrintingCustomerCopy() override;
  void MessagePrintingReversalReceipt() override;
  void MessageCommunicatingHost() override;
  void MessageCommunicationError() override;
  void MessageRebootingSystem() override;
  void MessageCountVisit(std::string& str) override;

  //Password
  bool RequestPassword(std::string display_msg, std::string& password) override;
  void MessageInvalidPassword() override;

  //Amount Entry messages
  void MessageAmountOutOfRange() override;
  void MessageTipAmountOutOfRange() override;

  //Settlement
  void MessageSettlementSuccessful() override;
  void MessageSettlementFailed() override;
  void MessageRetrySettlement() override;
  void MessageSettlementInProgress(std::string host_name) override;
  bool SelectSettlementOption(SettlementOption& settlement_option) override;
  bool DisplayTransactionCountAndTotal(const types::Currency& currency,
                                       unsigned int sale_count,
                                       uint64_t sale_total,
                                       unsigned int refund_count,
                                       uint64_t refund_total) override;
  void MessageNoLastSettlementRecord() override;

  //Report
  bool SelectReportOption(SettlementOption& settlement_option) override;

  //Card Processing
  void MessageProcessingCard() override;
  void MessageInsertOrSwipeCard() override;
  void MessageEnterPanOrSwipeCard() override;
  void MessageManualInputCard() override;
  void MessagePleaseUseOtherInterfaces() override;
  void MessageSwipeOrInsertOrTapCard() override;
  void MessagePleaseInsertCard() override;
  void MessagePleaseRemoveCard() override;
  void MessageUnsupportedCard() override;
  void MessageInvalidPan() override;
  void MessageLast4DigitMismatch() override;
  void MessageInvalidTrack2() override;
  void MessageCardRemoved() override;
  void MessageCardBlocked() override;
  void MessageCardExpired() override;
  void MessageCardError() override;
  void MessageHostError() override;
  void MessagePinOk() override;
  void MessagePinBlocked() override;
  void MessageIncorrectPin() override;
  void MessageSwipeNotAllowedForThisCard() override;
  void MessageManualNotAllowedForThisCard() override;
  void MessagePleaseSeePhone() override;

  //Key Exchange
  void MessageMasterKeyNotPresent() override;
  void MessageLogonFailure() override;
  void MessageLogonSuccessful() override;

  //Transaction Status
  void MessageApproved(const std::string& approval_code) override;
  void MessageDeclined() override;
  void MessageClessError() override;
  void MessageDeclinedByCard() override;
  void MessageProcessAborted() override;
  void MessageTransactionNotAllowed() override;
  void MessageTransactionAlreadyVoided() override;
  void MessageTransactionAlreadyAdjusted() override;
  void MessageCashAmountMultipleOf100() override;
  bool MessageTipPercentageExceeded() override;
  void MessageTransactionAlreadyPreAuthCompleted() override;

  //Batch Related
  void MessageSearchingInLogs() override;
  void MessageTransactionNotFound() override;
  void MessageReversalNotFound() override;
  void MessagePleaseSettleBatch() override;
  void MessagePleaseCloseBatch() override;
  void MessageBatchEmpty(const std::string& batch_type) override;
  void MessageBatchFull() override;
  void MessageBatchLocked() override;
  void MessageBatchMaxLimitExceeded() override;
  void MessageBatchTotalZero(const std::string& batch_type) override;
  void MessageDeleteBatch(unsigned int host_index) override;
  void MessageDisplayBatchEmpty(const std::string& host_name) override;
  void MessageBatchUploadInProgress(std::string host_name) override;

  //Terminal Initialization Related
  void MessagePleaseInitializeTerminal() override;
  void MessageTerminalAlreadyInitialized() override;
  void MessageDownloadAndInitializeParameterFile() override;
  void MessageParameterFileNotFoundOnServer() override;
  void MessageAppVersionNotAllowedToDownload() override;
  void MessageParameterVersionNotFoundOnServer() override;
  void MessageInvalidParameterData() override;
  void MessageInitializationSuccessful() override;
  void MessageInitializationFailed() override;

  bool DisplayBalance(const types::Amount& amount) override;

  // Test Host Related
  void DisplayHostTestingProgress(int current_host_num, int total_host_number,
                                  int error_number) override;
  void DisplayHostTestingResult(int total_host_number, int error_number)
      override;

  void DisplayResponseMessage(const std::string& response_code) override;
  void MessageEMVCandidateListEmpty() override;
  void MessageEMVReadCardError() override;

  //Clear Reversal Related
  bool ConfirmClearReversal(uint32_t& invoice_no,
                            const stdx::optional<types::Amount>& amount)
                                override;
  void ReversalCleared() override;
  bool RequestSignature(const types::Amount& amount,
                        std::string& signature_file_path) override;

  void MessageDisplayResourceNotFound(std::string resource_name) override;
  bool DisplayTxReview(TxReviewDetail& tx_detail) override;
  virtual BatchTotalReturn DisplayBatchTotal(BatchTotalToDisplay batch_total)
      override;

  bool SelectDccOption(const types::Amount& local_amount,
                       const types::Amount& dcc_amount,
                       DccOption& option) override;

  bool SelectDccMultiOptions(const types::Amount& local_amount,
                             const std::vector<types::Amount>& dcc_amount_list,
                             uint32_t& index_selected) override;
  void MessageTransactionCancelled() override;
  void DisplayEMVProcessingError(TransactionStatus txn_status) override;

 private:
  tpcore::GoalHandle& gl_;
  unsigned int msg_duration_ = 2000;
  unsigned long screen_timeout = 60 * 1000;

};

}

#endif
