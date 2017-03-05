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
#include "workflow_activities.h"
#include "tpcore/telium_manager.h"
#include <ctime>
#include "app_counter.h"
#include <types/pos_condition_code.h>
#include <tpcore/goal_handle.h>
#include <stdx/filesystem>
#include "utils/mask.h"
#include <bdor/transaction_type_name.h>

namespace bdor {

void PrintReceipts(ApplicationSettings& app_setting, Ui& ui,
                   TransactionStatus transaction_status,
                   const receipt::Receipt& reversal_receipt,
                   const receipt::Receipt& merchant_receipt,
                   const receipt::Receipt& customer_receipt,
                   receipt::Receipt& save_merchant_receipt,
                   receipt::Receipt& save_customer_receipt) {
  if (transaction_status == TransactionStatus::TO_REVERSE) {
    if (app_setting.GetTerminalConfiguration().FailedReceiptEnabled()) {
      ui.MessagePrintingReversalReceipt();
      PrintLoop(ui, reversal_receipt);
    }

  } else if (transaction_status == TransactionStatus::APPROVED
      || transaction_status == TransactionStatus::TO_ADVISE) {

    SaveLastTransactionReceipt(app_setting, save_merchant_receipt,
                               save_customer_receipt);

    ui.MessagePrintingMerchantCopy();

    PrintLoop(ui, merchant_receipt);

    if (ui.AskToPrintCustomerCopy()) {
      ui.MessagePrintingCustomerCopy();

      PrintLoop(ui, customer_receipt);
    }
  }
}

void PrintLoop(Ui& ui, const receipt::Receipt& receipt) {
  for (;;) {
    auto print_status = receipt.Print();
    if (print_status != receipt::PRINT_NOPAPER)
      break;
    ui.MessageInsertPaperRoll();
  }
}

bool DisplayPanAndExpiryDate(const ApplicationSettings& app_settings,
                             Ui* ui,
                             const std::uint64_t issuer_index,
                             const types::Pan& pan,
                             const std::string& expiry_date) {

  stdx::optional<IssuerDefinition> issuer = app_settings.managed_settings_
      ->GetIssuerDefinition(issuer_index);

  std::string masked_pan;
  if (issuer.has_value()) {
    masked_pan = pan.GetMaskedPan(issuer->pan_mask_display);
  } else {
    masked_pan = pan.ToString();
  }

  std::string expiry_date_masking_pattern = "";
  if (issuer.has_value())
    expiry_date_masking_pattern = issuer.value().expiry_date_mask_display;

  std::string masked_expiry_date = utils::ApplyFixedLengthMask(
      expiry_date, expiry_date_masking_pattern);

  return ui->DisplayPanAndExpiryDate(masked_pan, masked_expiry_date);
}

bool IsMemoryFull() {
  const size_t kMinFlashSize = 200 * 1024;
  size_t flash_available = tpcore::GetFreeDataFlashSize();

  return flash_available < kMinFlashSize;
}

bool IsValidAmount(const ApplicationSettings& app_settings_,
                   const types::Amount& amount) {
  types::Amount min_amount(
      amount.GetCurrencyName().c_str(),
      app_settings_.managed_settings_->terminal_config.MinAmount());

  types::Amount max_amount(
      amount.GetCurrencyName().c_str(),
      app_settings_.managed_settings_->terminal_config.MaxAmount());

  return amount.IsInRange(min_amount, max_amount);
}

bool IsValidRefundAmount(const ApplicationSettings& app_settings,
                         const types::Amount& amount) {
  types::Amount max_amount(
      amount.GetCurrencyName().c_str(),
      app_settings.managed_settings_->terminal_config.RefundMaxAmount());

  return amount <= max_amount;
}

bool IsValidInstalmentAmount(
    stdx::optional<InstalmentDefinition>& instalment_config_,
    const types::Amount& amount) {
  types::Amount min_amount(
                           amount.GetCurrencyName().c_str(),
                           instalment_config_->min_amount);

  types::Amount max_amount(
                           amount.GetCurrencyName().c_str(),
                           instalment_config_->max_amount);

  return amount.IsInRange(min_amount, max_amount);
}
bool IsValidPAN(const CardDefinition& card_definition_config,
                const types::Pan pan) {
  if (card_definition_config.luhn_check_required) {
    if (!pan.IsLuhnVerified()) {
      return false;
    }
  }

  if (!pan.IsLengthCorrect(card_definition_config.pan_min_length,
                           card_definition_config.pan_max_length))
    return false;

  return true;
}

std::string GetOfflineRRN(time_t tx_datetime, uint32_t stan) {
  char rrn[20];
  char time[20];
  strftime(time, sizeof(time), "%H%M%S", localtime(&tx_datetime));

  sprintf(rrn, "%s%06lu", time, stan);

  return std::string(rrn);
}

bool RetrieveConfigsFromPan(
    const ApplicationSettings& app_settings,
    const types::Pan& pan,
    stdx::optional<CardDefinition>& card_definition_config,
    stdx::optional<IssuerDefinition>& issuer_definition_config,
    stdx::optional<HostDefinition>& host_definition_config,
    const Transaction& tx) {

  card_definition_config = app_settings.managed_settings_
      ->GetCardDefinitionFromPan(pan);

  if (!card_definition_config.has_value())
    return false;

  issuer_definition_config =
      app_settings.managed_settings_->GetIssuerDefinition(
          card_definition_config->issuer_index);

  if (!issuer_definition_config.has_value()) {  // Invalid Issuer Index due to wrong settings
    return false;
  }

  if (tx.pos_condition_code == types::PosConditionCode::MOTO)
    host_definition_config = app_settings.managed_settings_->GetHostDefinition(
        card_definition_config->moto_host_index);
  else
    host_definition_config = app_settings.managed_settings_->GetHostDefinition(
        card_definition_config->host_index);

  if (!host_definition_config.has_value()) {  // Invalid Host Index due to wrong settings
    return false;
  }
  return true;
}
bool RetrieveInstalmentConfigsFromPan(
    const ApplicationSettings& app_settings,
    const types::Pan& pan,
    stdx::optional<CardDefinition>& card_definition_config,
    stdx::optional<IssuerDefinition>& issuer_definition_config,
    stdx::optional<HostDefinition>& host_definition_config,
    stdx::optional<InstalmentDefinition>& instalment_definition_config) {

  card_definition_config = app_settings.managed_settings_
      ->GetCardDefinitionFromPan(pan);

  if (!card_definition_config.has_value())
    return false;

  issuer_definition_config =
      app_settings.managed_settings_->GetIssuerDefinition(
          card_definition_config->issuer_index);

  if (!issuer_definition_config.has_value()) {  // Invalid Issuer Index due to wrong settings
    return false;
  }

  host_definition_config = app_settings.managed_settings_->GetHostDefinition(
      instalment_definition_config->host_index);

  if (!host_definition_config.has_value()) {  // Invalid Host Index due to wrong settings
    return false;
  }
  return true;
}
void SetTransactionDataFromConfigs(
    const stdx::optional<CardDefinition>& card_definition_config,
    const stdx::optional<IssuerDefinition>& issuer_definition_config,
    const stdx::optional<HostDefinition>& host_definition_config,
    Transaction* tx) {
  tx->card_index = card_definition_config->index;
  tx->card_name = issuer_definition_config->issuer_name;
  tx->issuer_index = issuer_definition_config->index;
  tx->host_index = host_definition_config->index;
  tx->tid = host_definition_config->terminal_id;
  tx->mid = host_definition_config->merchant_id;
  tx->nii = utils::FromString<uint32_t>(issuer_definition_config->nii);
  tx->tpdu = issuer_definition_config->tpdu;
}
void SetInstalmentTransactionDataFromConfigs(
    const stdx::optional<InstalmentDefinition>& instalment_definition_config,
    Transaction* tx) {
  tx->tenure = instalment_definition_config->instalment_period;
  tx->instalment_description = instalment_definition_config->description;
  tx->program_id = instalment_definition_config->program_id;
  tx->product_id = instalment_definition_config->product_id;
}
bool IsBelowFloorLimit(const types::Amount total_amount,
                       const std::uint64_t floor_limit) {
  return total_amount.GetValue() < floor_limit;
}

uint64_t GetTransactionFloorLimit(const CardDefinition& card_definition_config,
                                  const Transaction& tx) {

  if (tx.pos_condition_code == types::PosConditionCode::MOTO)
    return card_definition_config.moto_floor_limit;
  else
    return card_definition_config.floor_limit;
}

void SetOfflineApproveTransactionData(Transaction* tx) {
  std::string offline_approval_code = "0000Y1";
  tx->auth_id_response = offline_approval_code;
  tx->response_code = "Y1";
  tx->SetTransactionStatus(TransactionStatus::TO_ADVISE);
  tx->stan = GetNextStanNo();
  tx->rrn = GetOfflineRRN(tx->tx_datetime, tx->stan);
}

static CardDefinition::CvvMode GetCvvMode(
    const CardDefinition card_definition_config, const Transaction& tx) {
  if (tx.pos_entry_mode == types::PosEntryMode::MANUAL)
      {
    if (tx.pos_condition_code == types::PosConditionCode::MOTO)
      return card_definition_config.cvv_moto_sale;
    else
      return card_definition_config.cvv_manual_entry;
  }
  else if (tx.pos_entry_mode == types::PosEntryMode::MAGSTRIPE ||
      tx.pos_entry_mode == types::PosEntryMode::FALLBACK_MAGSTRIPE) {
    return card_definition_config.cvv_magstripe_entry;
  }
  else {
    return CardDefinition::CvvMode::CVV_NOPROMPT;
  }
}

bool RequestCvv(const CardDefinition& card_definition_config, Ui& ui,
                Transaction& tx) {
  CardDefinition::CvvMode mode = GetCvvMode(card_definition_config, tx);

  if (mode == CardDefinition::CvvMode::CVV_PROMPT_AND_ALLOW_BYPASS) {
    if (!ui.RequestCvv(tx.cvv, true))
      return false;
  }
  else if (mode == CardDefinition::CvvMode::CVV_PROMPT_AND_NO_BYPASS) {
    if (!ui.RequestCvv(tx.cvv, false))
      return false;
  }

  return true;
}

bool IsCardExpired(const std::string expiry_date_yymm) {
  char date[5];
  time_t current_time;
  current_time = stdx::time(NULL);
  strftime(date, sizeof(date), "%y%m", localtime(&current_time));

  if (expiry_date_yymm.compare(0, 4, date) >= 0)
    return false;

  return true;
}

bool CheckCardExpired(Ui& ui, const std::string expiry_date_yymm) {
  if (IsCardExpired(expiry_date_yymm)) {
    ui.MessageCardExpired();
    return false;
  }

  return true;
}

bool IsFallbackAllowed(const ApplicationSettings& app_settings) {
  return true;  // currently not configurable for FDMS
}

//TODO: call this function during printing receipt to print SIGN line or NO SIGNATURE REQUIRED(QPS)
// if return true, need to put tx.express_payment = true;
bool IsQPSTransaction(const Transaction& tx,
                      const std::uint64_t qps_floor_limit) {

  types::Amount qps_amount((*tx.amount).GetCurrencyName().c_str(),
                           qps_floor_limit);

  if (tx.pos_entry_mode != types::PosEntryMode::MAGSTRIPE
      && tx.pos_entry_mode != types::PosEntryMode::CHIP) {
    return false;
  }

  if (*tx.GetTotalAmount() > qps_amount)
    return false;

  return true;
}

bool CheckAndOpenBatch(const uint64_t host_index, Ui& ui, Batch& batch) {
  if (batch.IsBatchLocked(host_index)) {
    ui.MessagePleaseSettleBatch();
    return false;
  }

  if (batch.IsBatchClosed(host_index))
    return batch.OpenBatch(host_index);

  return true;
}

bool IsTxEnabled(TerminalConfiguration& terminal_config,
                 const Transaction& tx) {
  switch (tx.transaction_type)
  {
    case TransactionType::SALE:
      if (tx.pos_condition_code == types::PosConditionCode::MOTO)
        return terminal_config.TxMotoSaleEnabled();
      else
        return terminal_config.TxSaleEnabled();

    case TransactionType::REFUND:
      if (tx.pos_condition_code == types::PosConditionCode::MOTO)
        return terminal_config.TxMotoRefundEnabled();
      else
        return terminal_config.TxRefundEnabled();

    case TransactionType::PREAUTH:
      if (tx.pos_condition_code == types::PosConditionCode::MOTO_PREAUTH)
        return terminal_config.TxMotoPreauthEnabled();
      else
        return terminal_config.TxPreAuthEnabled();

    case TransactionType::PREAUTH_CANCELLATION:
      return terminal_config.TxVoidPreAuthEnabled();

    case TransactionType::PREAUTH_COMPLETION_ONLINE:
      return terminal_config.TxPreAuthCompletionOnlineEnabled();

    case TransactionType::PREAUTH_COMPLETION_OFFLINE:
      return terminal_config.TxPreAuthCompletionOfflineEnabled();

    case TransactionType::OFFLINE_SALE:
      return terminal_config.TxOfflineSaleEnabled();

    case TransactionType::INSTALMENT_SALE:
      return terminal_config.TxInstalmentSaleEnabled();

    case TransactionType::REWARD:
          return true;

    default:
      return false;
  }
}

bool IsTxEnabledPerCard(const CardDefinition& card_definition_config,
                        const Transaction& tx) {
  switch (tx.transaction_type)
  {
    case TransactionType::SALE:
      if (tx.pos_condition_code == types::PosConditionCode::MOTO)
        return card_definition_config.tx_moto_sale_enabled;
      else
        return card_definition_config.tx_sale_enabled;

    case TransactionType::REFUND:
      if (tx.pos_condition_code == types::PosConditionCode::MOTO)
        return card_definition_config.tx_moto_refund_enabled;
      else
        return card_definition_config.tx_refund_enabled;

    case TransactionType::PREAUTH:
      if (tx.pos_condition_code == types::PosConditionCode::MOTO_PREAUTH)
        return card_definition_config.tx_moto_preauth_enabled;
      else
        return card_definition_config.tx_preauth_enabled;

    case TransactionType::INSTALMENT_SALE:
      return card_definition_config.tx_instalment_sale_enabled;

    case TransactionType::REWARD:
          return true;

    default:
      return false;
  }
}

bool VerifyTipLimit(const types::Amount& sale_amount,
                    const types::Amount& tip_amount,
                    uint64_t tip_limit) {
  auto tip_amount_limit = (sale_amount.GetValue() * tip_limit) / 100;
  if (tip_amount.GetValue() > tip_amount_limit)
    return false;

  return true;
}

bool RequestAmount(const ApplicationSettings& app_settings,
                   Ui& ui,
                   Transaction& tx) {

  TerminalConfiguration terminal_config = app_settings.managed_settings_
      ->terminal_config;

  auto tip_entry_required = tx.TipSupported()
      &&
      (terminal_config.TipProcessing()
          == TerminalConfiguration::TIP_PROMPTED_AND_PRINTED);

  return RequestAmount(app_settings, ui, tip_entry_required, tx);
}

bool RequestAmount(const ApplicationSettings& app_settings,
                   Ui& ui,
                   const bool tip_required, Transaction& tx) {

  std::string currency_name = app_settings.managed_settings_->terminal_config
      .TerminalBaseCurrency().GetName();

  while (true) {
    types::Amount base_amount(currency_name.c_str(), 0);
    if (!ui.RequestAmount(base_amount))
      return false;

    tx.amount = base_amount;

    if (tip_required)
    {
      while (true) {
        types::Amount tip_amount(currency_name.c_str(), 0);

        if (!ui.RequestTipAmount(tip_amount))
          return false;

        if (!VerifyTipLimit(
            base_amount,
            tip_amount,
            app_settings.managed_settings_->terminal_config.TipAdjustMaxVariance())) {
          ui.MessageTipAmountOutOfRange();
          continue;
        }

        if (tip_amount.GetValue() > 0)
          tx.secondary_amount = tip_amount;

        break;
      }
    }

    if (!IsValidAmount(app_settings, *tx.GetTotalAmount())) {
      ui.MessageAmountOutOfRange();
      continue;
    }

    break;
  }
  return true;
}

bool RequestPreauthAmount(const ApplicationSettings& app_settings,
                          Ui& ui,
                          Transaction& tx) {

  std::string currency_name = app_settings.managed_settings_->terminal_config
      .TerminalBaseCurrency().GetName();

  while (true) {
    types::Amount base_amount(currency_name.c_str(), 0);
    if (!ui.RequestPreauthAmount(base_amount))
      return false;

    tx.preauth_amount = base_amount;

    if (!IsValidAmount(app_settings, *tx.GetTotalPreauthAmount())) {
      ui.MessageAmountOutOfRange();
      continue;
    }

    break;
  }
  return true;
}

bool IsDuplicateInvoice(Batch& batch, const uint32_t invoice) {
  std::vector<Transaction> tx_list;
  tx_list = batch.FindTransactionByInvoice(invoice);

  return !tx_list.empty();
}

uint32_t GetNextUniqueInvoice(Batch& batch) {
  uint32_t invoice;

  do {
    invoice = GetNextInvoiceNo();
  } while (IsDuplicateInvoice(batch, invoice));

  return invoice;
}

void SaveLastTransactionReceipt(const ApplicationSettings& app_settings,
                                receipt::Receipt& merchant_receipt,
                                receipt::Receipt& customer_receipt) {

  tpcore::GoalHandle gl;

  customer_receipt.SaveToFile(
      app_settings.GetLastCustomerTxReceiptBmpPath().c_str(),
      receipt::MimeType::MIME_BMP);

  merchant_receipt.SaveToFile(
      app_settings.GetLastMerchantTxReceiptBmpPath().c_str(),
      receipt::MimeType::MIME_BMP);
}

bool CheckFileExist(std::string path) {
  return stdx::is_regular_file(path);
}

TxReviewDetail GetTxDetailToDisplay(const ApplicationSettings& app_settings,
                                    const Transaction& tx) {

  TxReviewDetail output;
  stdx::optional<IssuerDefinition> issuer = app_settings.managed_settings_
      ->GetIssuerDefinition(tx.issuer_index);

  std::string masked_pan;
  if (issuer.has_value()) {
    masked_pan = (*tx.pan).GetMaskedPan(issuer->pan_mask_display);
  } else {
    masked_pan = (*tx.pan).ToString();
  }

  std::string expiry_date_masking_pattern = "";
  if (issuer.has_value())
    expiry_date_masking_pattern = issuer->expiry_date_mask_display;

  std::string masked_expiry_date = utils::ApplyFixedLengthMask(
      tx.expiration_date, expiry_date_masking_pattern);

  output.tx_name = GetTransactionTypeName(tx);
  output.masked_pan = masked_pan;
  output.masked_expiry_date = masked_expiry_date;
  output.card_name = issuer->issuer_name;
  output.amount = *tx.GetTotalAmount();
  output.auth_code = tx.auth_id_response;
  output.stan = tx.stan;
  return output;
}

types::Amount GetTotalAmount(const Transaction& tx) {
  if (tx.transaction_type == TransactionType::PREAUTH
      || tx.transaction_type == TransactionType::PREAUTH_CANCELLATION) {
    return *tx.GetTotalPreauthAmount();
  } else {
    return *tx.GetTotalAmount();
  }

}

bool RunDccFlow(
                const ApplicationSettings& app_settings,
                Ui& ui,
                Batch& batch,
                HostSwitch& host,
                Transaction& tx,
                HostAuthorizedFunc dcc_query_func,
                HostAuthorizedFunc dcc_auth_func,
                HostSwitch::Status& host_status) {
  static std::map<TerminalConfiguration::DccProviderOption, DccProvider> provider_map =
      {
          { TerminalConfiguration::DccProviderOption::FEXCO, DccProvider::FEXCO },
          { TerminalConfiguration::DccProviderOption::PURE_COMMERCE,
              DccProvider::PURE_COMMERCE
          },
          { TerminalConfiguration::DccProviderOption::FINTRAX,
              DccProvider::FINTRAX
          },
      };

  tx.dcc_provider = provider_map[app_settings.GetTerminalConfiguration()
      .DccProvider()];

  if (app_settings.GetTerminalConfiguration().DccStaffIdInputEnabled()) {
    if (!ui.RequestStaffId(tx.dcc_staff_id))
      return false;
  }

  tx.dcc_status = DccStatus::ENQUIRY;
  host_status = (&host->*dcc_query_func)(tx);

  if (tx.dcc_status == DccStatus::DCC_ELIGIBLE) {
    Ui::DccOption dcc_option;

    if (tx.dcc_amount_list.size() > 0) {
      uint32_t index_selected;

      if (!ui.SelectDccMultiOptions(GetTotalAmount(tx), tx.dcc_amount_list,
                                     index_selected)) {
        batch.Delete(tx);
        return false;
      }

      if (index_selected == tx.dcc_amount_list.size()) {
        dcc_option = Ui::DccOption::OPT_OUT;
      }
      else {
        tx.dcc_amount = tx.dcc_amount_list[index_selected];
        tx.dcc_conversion_rate = tx.dcc_conversion_rate_list[index_selected];

        dcc_option = Ui::DccOption::OPT_IN;
      }
    }
    else {
      if (!ui.SelectDccOption(GetTotalAmount(tx), *tx.dcc_amount, dcc_option)) {
        batch.Delete(tx);
        return false;
      }
    }

    tx.dcc_status =
        dcc_option == Ui::DccOption::OPT_IN ?
                                              DccStatus::OPT_IN :
                                              DccStatus::OPT_OUT;

    tx.stan_original = tx.stan = GetNextStanNo();
    tx.tc_upload_stan = GetNextStanNo();

    if (!batch.InsertOrUpdate(tx)) {
      SetInvoiceNo(tx.invoice_num);
      ui.MessageProcessAborted();
      return false;
    }

    host_status = (&host->*dcc_auth_func)(tx);
    return true;
  }

  return true;
}

}
