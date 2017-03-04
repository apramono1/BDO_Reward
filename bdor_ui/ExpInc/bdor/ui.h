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
#ifndef BDOR__UI_H_
#define BDOR__UI_H_

#include <cstdio>
#include <vector>
#include <string>
#include <map>
#include <functional>

#include <stdx/optional>

#include <types/amount.h>
#include <types/pan.h>
#include <tpcore/pin_entry_ui.h>
#include <bdor/transaction_type.h>
#include <types/account_type.h>
#include <bdor/batch_data_types.h>
#include <bdor/batch_review_data.h>
#include <bdor/batch_total_data.h>

namespace bdor {

using ItemDesc = std::pair<std::string, std::function<bool()>>;
using MenuDesc = std::vector<ItemDesc>;

class Ui {
 public:

  const size_t kStaffIdLength = 10;  //TODO: confirm staff ID length

  enum RecordSearchOption {
    SEARCH_BY_TRACE_NUMBER,
    SEARCH_BY_ACCOUNT_PAN,
    SEARCH_BY_INVOICE_NUMBER,
    SEARCH_BY_TX_AMOUNT
  };

  virtual ~Ui() {
  }

  enum SettlementOption {
    SETTLE_BY_HOST,
    SETTLE_ALL
  };

  enum InstalmentOption {
      NON_SCB,
      SCB
    };
  void SetMenuTitle(std::string title);
  enum class DccOption {
    OPT_IN,
    OPT_OUT
  };

  bool DispatchMenu(const char* title, const MenuDesc &menu_info);
  std::string GetBatchName(unsigned int host_index);
  std::string GetTxTypeString(TransactionType tx_type);

  virtual unsigned long MenuSelectionIndex(
      const char* title, const std::vector<std::string> &menu) = 0;
  virtual std::string MenuSelection(const char* title,
                                    const std::vector<std::string>& menu) = 0;

  virtual bool SelectAcquirer(unsigned int& host_index, bool show_all_acquirer =
                                  false) = 0;
  virtual bool SelectAccountType(types::AccountType& account_type) = 0;
  virtual bool SelectRecordSearchOption(RecordSearchOption& search_option) = 0;
  virtual bool SelectHost(const std::vector<std::string>& host_name_list,
                          unsigned int& pos) = 0;
  virtual bool SelectTransaction(const std::vector<TxReviewDetail>& tx_list,
                                 int* index) = 0;
  virtual bool SelectCurrency(const types::Amount& base_amount,
                              const types::Amount& dcc_amount,
                              const std::string& dcc_rate,
                              types::Amount& selected_amount) = 0;
  virtual bool RequestInvoice(uint32_t& invoice_no) = 0;
  virtual bool RequestTraceNumber(uint32_t& stan_no) = 0;
  virtual bool RequestTid(std::string& terminal_id) = 0;
  virtual bool RequestAmount(types::Amount& amount, const char* message,
                             bool is_zero_amount_allowed) = 0;
  virtual bool RequestAmount(types::Amount& amount) = 0;
  virtual bool RequestTipAmount(types::Amount& amount) = 0;
  virtual bool RequestPreauthAmount(types::Amount& amount) = 0;
  virtual bool RequestPreAuthCompletionAmount(types::Amount& amount) = 0;
  virtual bool RequestPan(std::vector<uint8_t>& pan_no) = 0;
  virtual bool RequestLast4DigitOfPan(std::string &last_4_digit) = 0;
  virtual bool Request4DbcForAmexCard(std::string* get_4_dbc) = 0;
  virtual bool RequestAtnOrPnrNo(std::string &atn_pnr) = 0;
  virtual bool RequestHostName(std::string &ip) = 0;
  virtual bool RequestPort(std::string &port) = 0;
  virtual bool RequestExpiryDate(std::string& expiry_date) = 0;  // user input MMYY, output YYMM
  virtual bool RequestCvv(std::string& cvv, bool bypass_allowed) = 0;
  virtual bool RequestAuthorizationCode(std::string& auth_code) = 0;
  virtual bool RequestStaffId(std::string& staff_id) = 0;

  virtual bool DisplayTransactionInfo(const std::string &txn_type,
                                      const types::Amount& amount) = 0;
  virtual bool DisplayTransactionAndTipAmount(types::Amount& sale_amount,
                                              types::Amount& tip_amount) = 0;
  virtual void MessageTipAdjustNotAllowed() = 0;
  virtual void MessageProcessing() = 0;
  virtual bool DisplayPanAndExpiryDate(
      const std::string& masked_pan, const std::string& masked_expiry_date) = 0;

  virtual bool AskToProceedWithFallback() = 0;
  virtual bool AskToPrintCustomerCopy() = 0;
  virtual bool AskToPrintDetailReport() = 0;
  virtual bool AskToDeleteOldPreAuth() = 0;
  virtual bool ConfirmDeleteBatch() = 0;

  virtual tpcore::PinEntryUI::uptr GetPinEntryUI(TransactionType tx_type,
                                                 types::Amount amount) = 0;

  virtual void MessageEMVProcessing() = 0;
  virtual void MessageEMVProcessingError() = 0;
  virtual void MessageExcessGuest() = 0;
  virtual void MessageSelectionInProgress() = 0;
  virtual void MessageInsertPaperRoll() = 0;
  virtual void MessageUnknownError() = 0;
  virtual void MessageParameterNotAvailable() = 0;
  virtual void MessageSaleAlreadyCompleted() = 0;
  virtual void MessagePleaseTryAgain() = 0;
  virtual void MessagePrintingMerchantCopy() = 0;
  virtual void MessagePrintingCustomerCopy() = 0;
  virtual void MessagePrintingReversalReceipt() = 0;
  virtual void MessageCommunicatingHost() = 0;
  virtual void MessageCommunicationError() = 0;
  virtual void MessageRebootingSystem() = 0;
  virtual void MessageCountVisit(std::string& str) = 0;

  // Password
  virtual bool RequestPassword(std::string display_msg,
                               std::string& password) = 0;
  virtual void MessageInvalidPassword() = 0;

  // Amount entry messages
  virtual void MessageAmountOutOfRange() = 0;
  virtual void MessageTipAmountOutOfRange() = 0;

  //Settlement
  virtual void MessageSettlementSuccessful() = 0;
  virtual void MessageSettlementFailed() = 0;
  virtual void MessageRetrySettlement() = 0;
  virtual void MessageSettlementInProgress(std::string host_name) = 0;
  virtual bool SelectSettlementOption(SettlementOption& settlement_option) = 0;
  virtual bool DisplayTransactionCountAndTotal(const types::Currency& currency,
                                               unsigned int sale_count,
                                               uint64_t sale_total,
                                               unsigned int refund_count,
                                               uint64_t refund_total) = 0;
  virtual void MessageNoLastSettlementRecord() = 0;

  //Report
  virtual bool SelectReportOption(SettlementOption& settlement_option) = 0;

  //Card Processing
  virtual void MessageProcessingCard() = 0;
  virtual void MessageInsertOrSwipeCard() = 0;
  virtual void MessageEnterPanOrSwipeCard() = 0;
  virtual void MessageManualInputCard() = 0;
  virtual void MessagePleaseInsertCard() = 0;
  virtual void MessagePleaseUseOtherInterfaces() = 0;
  virtual void MessageSwipeOrInsertOrTapCard() = 0;
  virtual void MessagePleaseRemoveCard() = 0;
  virtual void MessageUnsupportedCard() = 0;
  virtual void MessageInvalidPan() = 0;
  virtual void MessageLast4DigitMismatch() = 0;
  virtual void MessageInvalidTrack2() = 0;
  virtual void MessageCardRemoved() = 0;
  virtual void MessageCardBlocked() = 0;
  virtual void MessageCardExpired() = 0;
  virtual void MessageCardError() = 0;
  virtual void MessageHostError() = 0;
  virtual void MessagePinOk() = 0;
  virtual void MessagePinBlocked() = 0;
  virtual void MessageIncorrectPin() = 0;
  virtual void MessageSwipeNotAllowedForThisCard() = 0;
  virtual void MessageManualNotAllowedForThisCard() = 0;
  virtual void MessagePleaseSeePhone() = 0;

  //Key Exchange
  virtual void MessageMasterKeyNotPresent() = 0;
  virtual void MessageLogonFailure() = 0;
  virtual void MessageLogonSuccessful() = 0;

  //Transaction Status
  virtual void MessageApproved(const std::string& approval_code) = 0;
  virtual void MessageDeclined() = 0;
  virtual void MessageClessError() = 0;
  virtual void MessageDeclinedByCard() = 0;
  virtual void MessageProcessAborted() = 0;
  virtual void MessageTransactionNotAllowed() = 0;
  virtual void MessageTransactionAlreadyVoided() = 0;
  virtual void MessageTransactionAlreadyAdjusted() = 0;
  virtual void MessageCashAmountMultipleOf100() = 0;
  virtual bool MessageTipPercentageExceeded() = 0;
  virtual void MessageTransactionAlreadyPreAuthCompleted() = 0;

  //Batch Related
  virtual void MessageSearchingInLogs() = 0;
  virtual void MessageTransactionNotFound() = 0;
  virtual void MessageReversalNotFound() = 0;
  virtual void MessagePleaseSettleBatch() = 0;
  virtual void MessagePleaseCloseBatch() = 0;
  virtual void MessageBatchEmpty(const std::string& batch_type) = 0;
  virtual void MessageBatchFull() = 0;
  virtual void MessageBatchLocked() = 0;
  virtual void MessageBatchMaxLimitExceeded() = 0;
  virtual void MessageBatchTotalZero(const std::string& batch_type) = 0;
  virtual void MessageDeleteBatch(unsigned int host_index) = 0;
  virtual void MessageDisplayBatchEmpty(const std::string& host_name = "")= 0;
  virtual void MessageBatchUploadInProgress(std::string host_name) = 0;

  //Terminal Initialization Related
  virtual void MessagePleaseInitializeTerminal() = 0;
  virtual void MessageTerminalAlreadyInitialized() = 0;
  virtual void MessageDownloadAndInitializeParameterFile() = 0;
  virtual void MessageParameterFileNotFoundOnServer() = 0;
  virtual void MessageAppVersionNotAllowedToDownload() = 0;
  virtual void MessageParameterVersionNotFoundOnServer() = 0;
  virtual void MessageInvalidParameterData() = 0;
  virtual void MessageInitializationSuccessful() = 0;
  virtual void MessageInitializationFailed() = 0;

  virtual bool DisplayBalance(const types::Amount& amount) = 0;

  // Test Host Related
  virtual void DisplayHostTestingProgress(int current_host_num,
                                          int total_host_number,
                                          int error_number) = 0;
  virtual void DisplayHostTestingResult(int total_host_number,
                                        int error_number) = 0;
  virtual void DisplayResponseMessage(const std::string& response_code) = 0;
  virtual void MessageEMVCandidateListEmpty() = 0;
  virtual void MessageEMVReadCardError() = 0;

  //Clear Reversal Related
  virtual bool ConfirmClearReversal(
      uint32_t& invoice_no, const stdx::optional<types::Amount>& amount) = 0;
  virtual void ReversalCleared() = 0;
  virtual bool RequestSignature(const types::Amount& amount,
                                std::string& signature_file_path) = 0;
  virtual bool SelectInstalmentPlan(
      const std::vector<std::string>& instalment_name_list,
      unsigned int& pos)=0;

  virtual bool SelectInstalmentOption(InstalmentOption& instalment_option)=0;
  virtual void MessageDisplayResourceNotFound(std::string resource_name) = 0;
  virtual bool DisplayTxReview(TxReviewDetail& tx_detail) = 0;
  virtual BatchTotalReturn DisplayBatchTotal(BatchTotalToDisplay batch_total) = 0;

  virtual bool SelectDccOption(const types::Amount& local_amount,
                               const types::Amount& dcc_amount,
                               DccOption& option) = 0;

  virtual bool SelectDccMultiOptions(const types::Amount& local_amount,
                        const std::vector<types::Amount>& dcc_amount_list,
                        uint32_t& index_selected) = 0;
  virtual void MessageTransactionCancelled() = 0;
  virtual void DisplayEMVProcessingError(TransactionStatus txn_status) = 0;


   protected:
  const char *txn_type_title_;
};

}

#endif
