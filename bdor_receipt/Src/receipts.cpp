/*
 ------------------------------------------------------------------------------
 INGENICO Technical Software Department
 ------------------------------------------------------------------------------
 Copyright (c) 2014 INGENICO S.A.
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
#include <bdor/receipts.h>

#include <ctime>
#include <stdx/string>
#include <utils/strings.h>
#include <types/pos_entry_mode.h>
#include <types/pos_condition_code.h>
#include <utils/logger.h>
#include <utils/mask.h>
#include <bdor/application_settings.h>
#include <utils/get_default.h>

namespace bdor {

namespace {

std::string GetTransactionTypeName(const Transaction& tx) {

  std::string txn_type;
  if (tx.is_voided == true)
    txn_type = "VOID ";

  if (tx.pos_condition_code == types::PosConditionCode::MOTO ||
      tx.pos_condition_code == types::PosConditionCode::MOTO_PREAUTH)
    txn_type += "MOTO ";

  switch (tx.transaction_type) {
    case TransactionType::SALE:
      if (tx.is_adjusted)
        txn_type += "TIP ADJUSTMENT";
      else
        txn_type += "SALE";
      break;

    case TransactionType::OFFLINE_SALE:
      txn_type += "OFFLINE SALE";
      break;
    case TransactionType::CASH_ADVANCE:
      txn_type += "CASH ADVANCE";
      break;
    case TransactionType::REFUND:
      txn_type += "REFUND";
      break;
    case TransactionType::PREAUTH:
      txn_type += "PREAUTH";
      break;
    case TransactionType::INSTALMENT_SALE:
      txn_type += "INSTALMENT";
      break;
    case TransactionType::PREAUTH_CANCELLATION:
      txn_type += "PREAUTH";
      break;
    case TransactionType::PREAUTH_COMPLETION_OFFLINE:
      txn_type += "PREAUTH COMP";
      break;
    case TransactionType::PREAUTH_COMPLETION_ONLINE:
      txn_type += "PREAUTH COMP";
      break;
    default:
      break;
  }

  return txn_type;
}

std::string GetEntryModeString(types::PosEntryMode mode) {
  switch (mode) {
    case types::PosEntryMode::MANUAL:
      return "MANUAL";

    case types::PosEntryMode::MAGSTRIPE:
      return "SWIPE";

    case types::PosEntryMode::CHIP:
      return "CHIP";

    case types::PosEntryMode::CONTACTLESS:
      case types::PosEntryMode::CONTACTLESS_MAGSTRIPE:
      return "CLESS";

    case types::PosEntryMode::FALLBACK_MAGSTRIPE:
      return "FALLBACK";

    default:
      logger::die_unless(false, "Unknown Pos Entry Mode");
      return "UNKNOWN";
  }
}

std::string GetFormattedVersion(const std::string& version,
                                const std::string& build_date) {
  return "FDMS v" + version + " (" + build_date + ")";
}

types::Amount GetBaseAmount(const Transaction& tx) {
  if (tx.transaction_type == TransactionType::PREAUTH
      || tx.transaction_type == TransactionType::PREAUTH_CANCELLATION) {
    return *tx.preauth_amount;
  } else {
    return *tx.amount;
  }

}

types::Amount GetTotalAmount(const Transaction& tx) {
  if (tx.transaction_type == TransactionType::PREAUTH
      || tx.transaction_type == TransactionType::PREAUTH_CANCELLATION) {
    return *tx.GetTotalPreauthAmount();
  } else {
    return *tx.GetTotalAmount();
  }

}

std::string GetPanForPrinting(const stdx::optional<IssuerDefinition> issuer,
                              const CopyType copy,
                              const types::Pan pan) {
  std::string pan_masking_pattern = "";
  if (issuer.has_value())
  {
    if (copy == CopyType::CUSTOMER)
      pan_masking_pattern = issuer.value().pan_mask_customer_receipt;
    else
      pan_masking_pattern = issuer.value().pan_mask_merchant_receipt;
  }

  return pan.GetMaskedPan(pan_masking_pattern);
}

std::string GetExpiryDateForPrinting(
    const stdx::optional<IssuerDefinition> issuer,
    const CopyType copy,
    const std::string expiry_date) {

  std::string expiry_date_masking_pattern = "";
  if (issuer.has_value())
  {
    if (copy == CopyType::CUSTOMER)
      expiry_date_masking_pattern = issuer.value()
          .expiry_date_mask_customer_receipt;
    else
      expiry_date_masking_pattern = issuer.value()
          .expiry_date_mask_merchant_receipt;
  }

  std::string masked_expiry_date = utils::ApplyFixedLengthMask(
      expiry_date, expiry_date_masking_pattern);

  std::string month = masked_expiry_date.substr(2, 2);
  std::string year = masked_expiry_date.substr(0, 2);

  return month + "/" + year;
}

std::string GetDccProviderName(DccProvider provider) {
  switch (provider) {
    case DccProvider::FEXCO:
      return "Fexco";
    case DccProvider::PURE_COMMERCE:
      return "PureCommerce";
    case DccProvider::FINTRAX:
      return "Fintrax";
    default:
      return "Unknown DCC";
  }
}

std::string NumberToText(uint32_t number) {
  static std::map<uint32_t, std::string> number_map =
      {
          { '0', "ZERO" },
          { '1', "ONE" },
          { '2', "TWO" },
          { '3', "THREE" },
          { '4', "FOUR" },
          { '5', "FIVE" },
          { '6', "SIX" },
          { '7', "SEVEN" },
          { '8', "EIGHT" },
          { '9', "NINE" }
      };

  return utils::GetDefault(number_map, number, std::string(""));
}

// For example: 04.5000 => FOUR PT. FIVE ZERO
// To take one number in front the dot, and two numbers behind the dot
// TODO: put this function into unit tests
std::string GetPercentageText(std::string margin) {

  const uint32_t kMarginSize = 7;

  if (margin.size() != kMarginSize)
    return "FORMAT ERR";

  auto dot_found = margin.find(".");
  if (dot_found == std::string::npos || dot_found == 0
      || dot_found == kMarginSize - 1) {
    return "FORMAT ERR";
  }

  std::string margin_text = NumberToText(margin[dot_found -1]);
  margin_text += " PT. ";
  margin_text += NumberToText(margin[dot_found + 1]) + " " + NumberToText(margin[dot_found + 2]);

  return margin_text;
}

void SetFooterData(const Transaction& tx, receipt::Receipt::Map& result) {
  if ((tx.transaction_type == bdor::TransactionType::INSTALMENT_SALE)
      && (!tx.is_voided)) {
    result["FOOTER_1"] = "I AGREE TO PAY THE ABOVE TOTAL AMOUNT";
    result["FOOTER_2"] = "AND TO BE BOUND BY THE INSTALMENT TERMS";
    result["FOOTER_3"] = "AND CONDITIONS, A COPY OF WHICH IS";
    result["FOOTER_4"] = "AVAILABLE FROM THE MERCHANT";
  }
  else if (tx.dcc_status == DccStatus::OPT_IN) {
    result["FOOTER_1"] = "Cardholder acknowledges that the currency";
    result["FOOTER_2"] = "conversion is conducted by the Merchant";
    result["FOOTER_3"] = "and is not associated with or endorsed by";
    result["FOOTER_4"] = "Visa/Mastercard.";
    result["FOOTER_5"] = "Cardholder has been offerred a choice of";
    result["FOOTER_6"] = "currencies for payment including the";
    result["FOOTER_7"] = "Merchant's local currency.";
    result["FOOTER_8"] = "Cardholder agrees to pay the above total";
    result["FOOTER_9"] = "amount and accept that the selected";
    result["FOOTER_10"] = "Transaction Currency is final.";
    result["FOOTER_11"] = "This transaction is based on "
        + GetDccProviderName(tx.dcc_provider);
    result["FOOTER_12"] = "wholesale Rate plus "
        + GetPercentageText(tx.dcc_margin);
    result["FOOTER_13"] = "pct international Conversion Margin.";
  }
  else {
    result["FOOTER_1"] = "I AGREE TO PAY THE ABOVE TOTAL AMOUNT";
    result["FOOTER_2"] = "ACCORDING TO CARD ISSUER AGREEMENT";
  }
}

void SetDccData(const Transaction& tx, receipt::Receipt::Map& result) {
  result["DCC_CURRENCY"] = (*tx.dcc_amount).GetCurrencyName();

  double conversion_rate = utils::FromString<double>(tx.dcc_conversion_rate);
  double rate = 1.0 /  conversion_rate;

  result["CONVERSION_RATE"] = utils::ToString<double>(rate);
  result["DCC_AMOUNT"] = (*tx.dcc_amount).ToString();
  result["DCC_MARGIN"] = tx.dcc_margin;
}

receipt::Receipt::Map GetTxMap(const CopyType copy, const Transaction& tx,
                               const ApplicationSettings& app_settings) {

  receipt::Receipt::Map result;

  char date[20], time[20];
  strftime(date, sizeof(date), "%d/%m/%Y", localtime(&tx.tx_datetime));
  result["DATE"] = date;
  strftime(time, sizeof(time), "%T", localtime(&tx.tx_datetime));
  result["TIME"] = time;

  TerminalConfiguration terminal_config =
      app_settings.GetTerminalConfiguration();
  result["MERCHANT_NAME"] = terminal_config.ReceiptHeaderLine1();
  result["MERCHANT_ADDRESS_1"] = terminal_config.ReceiptHeaderLine2();
  result["MERCHANT_ADDRESS_2"] = terminal_config.ReceiptHeaderLine3();
  result["MERCHANT_ADDRESS_3"] = terminal_config.ReceiptHeaderLine4();

  result["MID"] = utils::ApplyFixedLengthMask(
      tx.mid, terminal_config.MerchantIdMasking());
  result["TID"] = utils::ApplyFixedLengthMask(
      tx.tid, terminal_config.TerminalIdMasking());

  result["AMOUNT"] = GetBaseAmount(tx).ToString();

  result["BATCH_NO"] = utils::NumericPadded(tx.batch_num, 6);
  result["INVOICE_NO"] = utils::NumericPadded(tx.invoice_num, 6);
  result["TRACE_NO"] = utils::NumericPadded(tx.stan, 6);

  result["TXN_TYPE"] = GetTransactionTypeName(tx);

  result["SE_NUMBER"] = tx.amex_se_number;

  result["APP_NAME"] = tx.application_prefered_name;
  result["AID"] = tx.aid.has_value() ? tx.aid->ToString() : "";

  stdx::optional<IssuerDefinition> issuer = app_settings.managed_settings_
      ->GetIssuerDefinition(tx.issuer_index);

  if (tx.pan.has_value()) {
    result["PAN"] = GetPanForPrinting(issuer, copy, *tx.pan);
  }

  result["EXP_DATE"] = GetExpiryDateForPrinting(issuer, copy,
                                                tx.expiration_date);

  result["CARD_TYPE"] = tx.card_name;

  result["ENTRY_MODE"] = GetEntryModeString(tx.pos_entry_mode);

  result["TC"] = tx.transaction_certificate;

  result["AUTH_CODE"] = tx.auth_id_response;
  result["RRN"] = tx.rrn;

  result["CURRENCY"] = GetBaseAmount(tx).GetCurrencyName();
  result["AMOUNT"] = GetBaseAmount(tx).ToString();

  result["SIGNATURE"] = tx.signature_file_path;

  types::Amount zero_amount(GetBaseAmount(tx).GetCurrencyName().c_str(), 0);
  result["ZERO_AMOUNT"] = zero_amount.ToString();

  if (tx.secondary_amount.has_value()) {
    result["TIP_AMOUNT"] = tx.secondary_amount->ToString();
    result["TOTAL_AMOUNT"] = GetTotalAmount(tx).ToString();
  }

  std::string name_trimmed = utils::LeftTrimmed(tx.card_holder_name,
                                                tx.card_holder_name.size(),
                                                ' ');
  name_trimmed = utils::RightTrimmed(name_trimmed, name_trimmed.size(), ' ');
  result["NAME"] = name_trimmed;

  if ((tx.transaction_type == bdor::TransactionType::INSTALMENT_SALE)
      && (!tx.is_voided)) {
    uint64_t tenure = tx.tenure;
    std::string stenure = utils::ToString(tenure);
    result["TENURE"] = utils::LeftTrimmed(stenure,
                                          stenure.size(),
                                          '0');
    if (tx.emi_per_month.has_value())
      result["EMI"] = tx.emi_per_month->ToString();
  }

  if (tx.dcc_status == DccStatus::OPT_IN) {
    SetDccData(tx, result);
  }

  SetFooterData(tx, result);
  result["VERSION"] = GetFormattedVersion(app_settings.app_version,
                                          app_settings.build_date);
  return result;
}

receipt::Receipt::Map GetSettlementMap(
    ApplicationSettings& app_settings,
    const SingleHostSettlementData& settlement_data) {
  receipt::Receipt::Map result;

  std::map<std::string, std::string> receipt_data;

  receipt_data["CURRENCY"] = app_settings.managed_settings_->terminal_config
      .TerminalBaseCurrency().GetName();

  if (!app_settings.managed_settings_
      ->terminal_config.ReceiptHeaderLine1().empty())
    receipt_data["MERCHANT_NAME"] = app_settings.managed_settings_
        ->terminal_config.ReceiptHeaderLine1().c_str();

  if (!app_settings.managed_settings_
      ->terminal_config.ReceiptHeaderLine2().empty())
    receipt_data["MERCHANT_ADDRESS_1"] = app_settings.managed_settings_
        ->terminal_config.ReceiptHeaderLine2().c_str();

  if (!app_settings.managed_settings_
      ->terminal_config.ReceiptHeaderLine3().empty())
    receipt_data["MERCHANT_ADDRESS_2"] = app_settings.managed_settings_
        ->terminal_config.ReceiptHeaderLine3().c_str();

  if (!app_settings.managed_settings_
      ->terminal_config.ReceiptHeaderLine4().empty())
    receipt_data["MERCHANT_ADDRESS_3"] = app_settings.managed_settings_
        ->terminal_config.ReceiptHeaderLine4().c_str();

  if (!app_settings.managed_settings_
      ->terminal_config.ReceiptHeaderLine5().empty())
    receipt_data["MERCHANT_ADDRESS_4"] = app_settings.managed_settings_
        ->terminal_config.ReceiptHeaderLine5().c_str();

  std::string host_index_postfix = "_"
      + stdx::to_string(settlement_data.report.host_index);

  char date[20], time[20];
  strftime(date, sizeof(date), "%d/%m/%Y",
           localtime(&settlement_data.settlement_datetime));
  receipt_data["DATE" + host_index_postfix] = date;
  strftime(time, sizeof(time), "%T",
           localtime(&settlement_data.settlement_datetime));
  receipt_data["TIME" + host_index_postfix] = time;

  auto host_definition = app_settings.managed_settings_->GetHostDefinition(
      settlement_data.report.host_index);
  receipt_data["MID" + host_index_postfix] = host_definition->merchant_id;
  receipt_data["TID" + host_index_postfix] = host_definition->terminal_id;
  receipt_data["BATCH_NO" + host_index_postfix] = utils::NumericPadded(
      settlement_data.report.batch_number, 6);
  receipt_data["HOST" + host_index_postfix] = host_definition->host_name;

  receipt_data["NO_CARD_NAME" + host_index_postfix] = stdx::to_string(
      settlement_data.report.batch_totals_list.size()).c_str();

  if (settlement_data.report_type == HostReportType::SETTLEMENT_REPORT) {
    std::string settlement_status = "SUCCESSFUL";
    if (settlement_data.settlement_status != SettlementStatus::SUCCESS) {
      settlement_status = "FAILED";
    }
    receipt_data["SETTLEMENT_STATUS" + host_index_postfix] = settlement_status;

    receipt_data["REPORT_TYPE" + host_index_postfix] = std::string(
        "SETTLEMENT REPORT");
  } else {
    receipt_data["REPORT_TYPE" + host_index_postfix] = std::string(
        "SUMMARY REPORT");
  }

  std::string batch_upload_line;
  if (settlement_data.batch_upload_performed) {
    batch_upload_line = "BATCH UPLOAD PERFORMED";
  }
  receipt_data["BATCH_UPLOAD" + host_index_postfix] = batch_upload_line;

  for (unsigned int i = 0; i < settlement_data.report.batch_totals_list.size();
      i++) {
    std::string card_name_postfix = host_index_postfix + "_"
        + stdx::to_string(i);
    auto& card_totals = settlement_data.report.batch_totals_list[i];

    receipt_data["CARD_NAME" + card_name_postfix] = card_totals.card_type;

    receipt_data["SALE_COUNT" + card_name_postfix] = stdx::to_string(
        card_totals.sales_total.count);
    receipt_data["SALE_AMOUNT" + card_name_postfix] = stdx::to_string(
        card_totals.sales_total.total);

    receipt_data["TIPS_COUNT" + card_name_postfix] = stdx::to_string(
        card_totals.tips_total.count);
    receipt_data["TIPS_AMOUNT" + card_name_postfix] = stdx::to_string(
        card_totals.tips_total.total);

    receipt_data["REFUND_COUNT" + card_name_postfix] = stdx::to_string(
        card_totals.refunds_total.count);
    receipt_data["REFUND_AMOUNT" + card_name_postfix] = stdx::to_string(
        card_totals.refunds_total.total);

    receipt_data["OFFLINE_SALE_COUNT" + card_name_postfix] = stdx::to_string(
        card_totals.offline_sales_total.count);
    receipt_data["OFFLINE_SALE_AMOUNT" + card_name_postfix] = stdx::to_string(
        card_totals.offline_sales_total.total);

    receipt_data["VOID_COUNT" + card_name_postfix] = stdx::to_string(
        card_totals.void_sales_total.count);
    receipt_data["VOID_AMOUNT" + card_name_postfix] = stdx::to_string(
        card_totals.void_sales_total.total);

    receipt_data["VOID_REFUND_COUNT" + card_name_postfix] = stdx::to_string(
        card_totals.void_refunds_total.count);
    receipt_data["VOID_REFUND_AMOUNT" + card_name_postfix] = stdx::to_string(
        card_totals.void_refunds_total.total);

    receipt_data["TOTAL_COUNT" + card_name_postfix] = stdx::to_string(
        card_totals.card_total.count);
    receipt_data["TOTAL_AMOUNT" + card_name_postfix] = stdx::to_string(
        card_totals.card_total.total);
  }

// grand total
// TODO: try to put that code in a function
  std::string postfix = host_index_postfix + "_GRAND_TOTAL";
  receipt_data["SALE_COUNT" + postfix] = stdx::to_string(
      settlement_data.report.global_batch_totals.sales_total.count);
  receipt_data["SALE_AMOUNT" + postfix] = stdx::to_string(
      settlement_data.report.global_batch_totals.sales_total.total);

  receipt_data["TIPS_COUNT" + postfix] = stdx::to_string(
      settlement_data.report.global_batch_totals.tips_total.count);
  receipt_data["TIPS_AMOUNT" + postfix] = stdx::to_string(
      settlement_data.report.global_batch_totals.tips_total.total);

  receipt_data["REFUND_COUNT" + postfix] = stdx::to_string(
      settlement_data.report.global_batch_totals.refunds_total.count);
  receipt_data["REFUND_AMOUNT" + postfix] = stdx::to_string(
      settlement_data.report.global_batch_totals.refunds_total.total);

  receipt_data["OFFLINE_SALE_COUNT" + postfix] = stdx::to_string(
      settlement_data.report.global_batch_totals.offline_sales_total.count);
  receipt_data["OFFLINE_SALE_AMOUNT" + postfix] = stdx::to_string(
      settlement_data.report.global_batch_totals.offline_sales_total.total);

  receipt_data["VOID_COUNT" + postfix] = stdx::to_string(
      settlement_data.report.global_batch_totals.void_sales_total.count);
  receipt_data["VOID_AMOUNT" + postfix] = stdx::to_string(
      settlement_data.report.global_batch_totals.void_sales_total.total);

  receipt_data["VOID_REFUND_COUNT" + postfix] = stdx::to_string(
      settlement_data.report.global_batch_totals.void_refunds_total.count);
  receipt_data["VOID_REFUND_AMOUNT" + postfix] = stdx::to_string(
      settlement_data.report.global_batch_totals.void_refunds_total.total);

  receipt_data["TOTAL_COUNT" + postfix] = stdx::to_string(
      settlement_data.report.global_batch_totals.total.count);
  receipt_data["TOTAL_AMOUNT" + postfix] = stdx::to_string(
      settlement_data.report.global_batch_totals.total.total);

  receipt_data["VERSION"] = GetFormattedVersion(app_settings.app_version,
                                                app_settings.build_date);

  return receipt_data;
}

receipt::Receipt::Map GetGrandTotalSettlementMap(
    ApplicationSettings& app_settings,
    const GrandBatchTotalsForReport& grand_totals) {
  receipt::Receipt::Map result;

  std::map<std::string, std::string> receipt_data;

  receipt_data["CURRENCY"] = app_settings.managed_settings_->terminal_config
      .TerminalBaseCurrency().GetName();

// grand total
// TODO: try to put that code in a function
  std::string postfix = "_GRAND_GRAND_TOTAL";
  receipt_data["SALE_COUNT" + postfix] = stdx::to_string(
      grand_totals.sales_total.count);
  receipt_data["SALE_AMOUNT" + postfix] = stdx::to_string(
      grand_totals.sales_total.total);

  receipt_data["TIPS_COUNT" + postfix] = stdx::to_string(
      grand_totals.tips_total.count);
  receipt_data["TIPS_AMOUNT" + postfix] = stdx::to_string(
      grand_totals.tips_total.total);

  receipt_data["REFUND_COUNT" + postfix] = stdx::to_string(
      grand_totals.refunds_total.count);
  receipt_data["REFUND_AMOUNT" + postfix] = stdx::to_string(
      grand_totals.refunds_total.total);

  receipt_data["OFFLINE_SALE_COUNT" + postfix] = stdx::to_string(
      grand_totals.offline_sales_total.count);
  receipt_data["OFFLINE_SALE_AMOUNT" + postfix] = stdx::to_string(
      grand_totals.offline_sales_total.total);

  receipt_data["VOID_COUNT" + postfix] = stdx::to_string(
      grand_totals.void_sales_total.count);
  receipt_data["VOID_AMOUNT" + postfix] = stdx::to_string(
      grand_totals.void_sales_total.total);

  receipt_data["VOID_REFUND_COUNT" + postfix] = stdx::to_string(
      grand_totals.void_refunds_total.count);
  receipt_data["VOID_REFUND_AMOUNT" + postfix] = stdx::to_string(
      grand_totals.void_refunds_total.total);

  receipt_data["TOTAL_COUNT" + postfix] = stdx::to_string(
      grand_totals.total.count);
  receipt_data["TOTAL_AMOUNT" + postfix] = stdx::to_string(
      grand_totals.total.total);

  return receipt_data;
}

receipt::Receipt::Map GetDetailReportMap(ApplicationSettings& app_settings,
                                         const std::vector<Transaction>& txns,
                                         int host_index, int batch_num) {
  receipt::Receipt::Map result;

  int i = 0;
  time_t current_time;
  char date[20], time[20];
  current_time = stdx::time(NULL);
  strftime(date, sizeof(date), "%d/%m/%Y", localtime(&current_time));
  result["DATE"] = date;
  strftime(time, sizeof(time), "%T", localtime(&current_time));
  result["TIME"] = time;

  result["CURRENCY"] = app_settings.managed_settings_->terminal_config
      .TerminalBaseCurrency().GetName();

  auto host_definition = app_settings.managed_settings_->GetHostDefinition(
      host_index);
  result["MID"] = host_definition->merchant_id;
  result["TID"] = host_definition->terminal_id;
  result["BATCH_NO"] = utils::NumericPadded(batch_num, 6);
  result["HOST"] = host_definition->host_name;

  if (!app_settings.managed_settings_
      ->terminal_config.ReceiptHeaderLine1().empty())
    result["MERCHANT_NAME"] = app_settings.managed_settings_
        ->terminal_config.ReceiptHeaderLine1().c_str();

  if (!app_settings.managed_settings_
      ->terminal_config.ReceiptHeaderLine2().empty())
    result["MERCHANT_ADDRESS_1"] = app_settings.managed_settings_
        ->terminal_config.ReceiptHeaderLine2().c_str();

  if (!app_settings.managed_settings_
      ->terminal_config.ReceiptHeaderLine3().empty())
    result["MERCHANT_ADDRESS_2"] = app_settings.managed_settings_
        ->terminal_config.ReceiptHeaderLine3().c_str();

  if (!app_settings.managed_settings_
      ->terminal_config.ReceiptHeaderLine4().empty())
    result["MERCHANT_ADDRESS_3"] = app_settings.managed_settings_
        ->terminal_config.ReceiptHeaderLine4().c_str();

  if (!app_settings.managed_settings_
      ->terminal_config.ReceiptHeaderLine5().empty())
    result["MERCHANT_ADDRESS_4"] = app_settings.managed_settings_
        ->terminal_config.ReceiptHeaderLine5().c_str();

  for (const auto& txn : txns) {
    std::string postfix = "_" + utils::ToString(i);
    stdx::optional<IssuerDefinition> issuer = app_settings.managed_settings_
        ->GetIssuerDefinition(txn.issuer_index);
    std::string field_name;

    std::string masked_pan;
    if (issuer.has_value()) {
      masked_pan = txn.pan->GetMaskedPan(
                                         issuer->pan_mask_merchant_receipt);
    } else {
      masked_pan = txn.pan->ToString();
    }

    std::string masked_expiry_date = txn.expiration_date;  //   expiry_date;
    std::string expiry_date_mask;

    if (issuer.has_value())
      expiry_date_mask = issuer.value().expiry_date_mask_merchant_receipt;
    else
      expiry_date_mask = txn.expiration_date;

    const size_t kExpiryDateMaskLength = 4;
    if (expiry_date_mask.size() == kExpiryDateMaskLength) {
      for (unsigned int i = 0; i < kExpiryDateMaskLength; i++) {
        if (expiry_date_mask[i] != 'N')
          masked_expiry_date[i] = expiry_date_mask[i];
      }
    }

    field_name = "CARD_NAME" + postfix;
    result[field_name.c_str()] = txn.card_name;

    field_name = "PAN" + postfix;
    result[field_name.c_str()] = masked_pan;

    std::string month = masked_expiry_date.substr(0, 2);
    std::string year = masked_expiry_date.substr(2, 2);
    field_name = "EXP_DATE" + postfix;
    result[field_name.c_str()] = month + "/" + year;

    field_name = "TRACE_NO" + postfix;
    result[field_name.c_str()] = utils::NumericPadded(txn.stan, 6);

    field_name = "TXN_TYPE" + postfix;
    result[field_name.c_str()] = GetTransactionTypeName(txn);

    field_name = "AMOUNT" + postfix;
    result[field_name.c_str()] = txn.GetTotalAmount()->ToString();

    field_name = "AUTH_CODE" + postfix;
    result[field_name.c_str()] = txn.auth_id_response;

    field_name = "INVOICE" + postfix;
    result[field_name.c_str()] = utils::NumericPadded(txn.invoice_num, 6);

    char tx_date[20], tx_time[20];
    strftime(tx_date, sizeof(tx_date), "%d/%m/%Y", localtime(&txn.tx_datetime));
    field_name = "DATE" + postfix;
    result[field_name] = tx_date;

    strftime(tx_time, sizeof(tx_time), "%T", localtime(&txn.tx_datetime));
    field_name = "TIME" + postfix;
    result[field_name] = tx_time;

    i++;
  }

  return result;
}

receipt::Receipt::Map GetDetailReportSummaryMap(
    ApplicationSettings& app_settings,
    std::vector<BatchTotalsForReport> host_total) {

  receipt::Receipt::Map result;
  int i = 0;
  BatchTotal grand_total;

  for (const auto& total : host_total) {
    std::string postfix = "_" + utils::ToString(i);
    std::string field_name;

    field_name = "CARD_NAME" + postfix;
    result[field_name.c_str()] = total.card_type;

    field_name = "TOTAL_COUNT" + postfix;
    result[field_name.c_str()] = stdx::to_string(total.card_total.count);

    field_name = "TOTAL_AMOUNT" + postfix;
    result[field_name.c_str()] = stdx::to_string(total.card_total.total);

    i++;
    grand_total += total.card_total;
  }

  result["GRAND_TOTAL_CARD_NAME"] = "TOTALS";
  result["GRAND_TOTAL_COUNT"] = stdx::to_string(grand_total.count);
  result["GRAND_TOTAL_AMOUNT"] = stdx::to_string(grand_total.total);

  result["VERSION"] = GetFormattedVersion(app_settings.app_version,
                                          app_settings.build_date);
  return result;
}

}

Receipts::receipt_uptr Receipts::GetReversalReceipt(
    const ApplicationSettings& app_settings,
    const Transaction& tx) {

  auto receipt = PrepareReversalReceipt();
  receipt->Expand(GetTxMap(CopyType::MERCHANT, tx, app_settings));
  return receipt;
}

CvmType GetCvmType(const Transaction& tx) {
  if (tx.pin_entered && tx.signature_required)
    return CvmType::PIN_AND_SIGNATURE;

  auto cvm = tx.pin_entered ? CvmType::PIN :
             tx.signature_required ? CvmType::SIGNATURE :
             tx.express_payment ? CvmType::EXPRESS_PAYMENT :
                                  CvmType::NO_CVM;

  return cvm;
}

Receipts::receipt_uptr Receipts::GetSaleReceipt(
    const ApplicationSettings& app_settings,
    const Transaction& tx,
    CopyType copy,
    ReprintType reprint) {

  TerminalConfiguration::TipProcessingMode tip_mode = app_settings
      .GetTerminalConfiguration().TipProcessing();
  if (!tx.TipSupported())
    tip_mode = TerminalConfiguration::TipProcessingMode::TIP_DISABLED;

  auto receipt = PrepareSaleReceipt(copy, reprint, tip_mode, GetCvmType(tx));

  receipt->Expand(GetTxMap(copy, tx, app_settings));
  return receipt;
}

Receipts::receipt_uptr Receipts::GetOfflineSaleReceipt(
    const ApplicationSettings& app_settings,
    const Transaction& tx,
    CopyType copy,
    ReprintType reprint) {

  TerminalConfiguration::TipProcessingMode tip_mode = app_settings
      .GetTerminalConfiguration().TipProcessing();
  auto receipt = PrepareOfflineSaleReceipt(copy, reprint, tip_mode,
                                           GetCvmType(tx));
  receipt->Expand(GetTxMap(copy, tx, app_settings));
  return receipt;
}

Receipts::receipt_uptr Receipts::GetRefundReceipt(
    const ApplicationSettings& app_settings,
    const Transaction& tx,
    CopyType copy,
    ReprintType reprint) {

  auto receipt = PrepareRefundReceipt(copy, reprint, GetCvmType(tx));

  receipt->Expand(GetTxMap(copy, tx, app_settings));
  return receipt;
}

Receipts::receipt_uptr Receipts::GetPreauthReceipt(
    const ApplicationSettings& app_settings,
    const Transaction& tx,
    CopyType copy,
    ReprintType reprint) {

  auto receipt = PreparePreauthReceipt(copy, reprint, GetCvmType(tx));
  receipt->Expand(GetTxMap(copy, tx, app_settings));
  return receipt;
}

Receipts::receipt_uptr Receipts::GetPreauthCancellationReceipt(
    const ApplicationSettings& app_settings,
    const Transaction& tx,
    CopyType copy,
    ReprintType reprint) {

  auto receipt = PreparePreauthCancellationReceipt(copy, reprint, GetCvmType(tx));
  receipt->Expand(GetTxMap(copy, tx, app_settings));
  return receipt;
}

Receipts::receipt_uptr Receipts::GetCompletionReceipt(
    const ApplicationSettings& app_settings,
    const Transaction& tx,
    CopyType copy,
    ReprintType reprint) {

  auto receipt = PrepareCompletionReceipt(copy, reprint, GetCvmType(tx));
  receipt->Expand(GetTxMap(copy, tx, app_settings));
  return receipt;
}

Receipts::receipt_uptr Receipts::GetTipAdjustReceipt(
    const ApplicationSettings& app_settings,
    const Transaction& tx,
    CopyType copy,
    ReprintType reprint) {

  auto receipt = PrepareTipAdjustReceipt(copy, reprint, GetCvmType(tx));
  receipt->Expand(GetTxMap(copy, tx, app_settings));
  return receipt;
}

Receipts::receipt_uptr Receipts::GetVoidReceipt(
    const ApplicationSettings& app_settings,
    const Transaction& tx,
    CopyType copy,
    ReprintType reprint) {

  TerminalConfiguration::TipProcessingMode tip_mode = app_settings
      .GetTerminalConfiguration().TipProcessing();
  auto receipt = PrepareVoidReceipt(copy, reprint, tip_mode, GetCvmType(tx));
  receipt->Expand(GetTxMap(copy, tx, app_settings));
  return receipt;
}

Receipts::receipt_uptr Receipts::GetSingleHostSettlementReceipt(
    ApplicationSettings& app_settings,
    const SingleHostSettlementData& settlement_data) {

  auto receipt = PrepareSingleHostSettlementReceipt(
      settlement_data.report.host_index,
      settlement_data.report.batch_totals_list.size());
  receipt->Expand(GetSettlementMap(app_settings, settlement_data));
  return receipt;
}

Receipts::receipt_uptr Receipts::GetAllHostsSettlementReceipt(
    ApplicationSettings& app_settings,
    const AllHostsSettlementData& settlement_data) {

  auto receipt = PrepareAllHostsSettlementReceipt(
      settlement_data.host_report_list);

  receipt::Receipt::Map data_dict;
  for (const auto& host_settlement_data : settlement_data.host_report_list) {
    auto map = GetSettlementMap(app_settings, host_settlement_data);
    data_dict.insert(map.begin(), map.end());
  }

  auto map = GetGrandTotalSettlementMap(app_settings,
                                        settlement_data.global_totals);
  data_dict.insert(map.begin(), map.end());

  receipt->Expand(data_dict);
  return receipt;
}

Receipts::receipt_uptr Receipts::GetDetailReportReceipt(
    ApplicationSettings& app_settings,
    const DetailReportSingleHost& detail_report) {

  receipt::Receipt::Map data_dict;

  auto receipt = PrepareDetailReportReceipt(
      detail_report.transaction_count, detail_report.issuer_totals_list.size());

  auto map_detail = GetDetailReportMap(app_settings,
                                       detail_report.transaction_list,
                                       detail_report.host_index,
                                       detail_report.batch_number);
  data_dict.insert(map_detail.begin(), map_detail.end());

  auto map_total = GetDetailReportSummaryMap(app_settings,
                                             detail_report.issuer_totals_list);
  data_dict.insert(map_total.begin(), map_total.end());

  receipt->Expand(data_dict);

  return receipt;
}

Receipts::receipt_uptr Receipts::GetInstalmentSaleReceipt(
    const ApplicationSettings& app_settings,
    const Transaction& tx,
    CopyType copy,
    ReprintType reprint) {

  auto receipt = PrepareInstalmentSaleReceipt(copy, reprint, GetCvmType(tx));
  receipt->Expand(GetTxMap(copy, tx, app_settings));
  return receipt;
}
}
