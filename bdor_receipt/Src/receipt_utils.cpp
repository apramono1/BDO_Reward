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

#include <receipt/goal_receipt.h>
#include "receipt_formatter.h"
#include "receipt/receipt.h"
#include <bdor/application_settings.h>

namespace bdor {

using namespace receipt;

// CONST
const T_GL_DIM tx_type_font_size = 25;
const T_GL_DIM tx_pan_font_size = 20;
const T_GL_DIM tx_content_font_size = 15;

const uint32_t kRightPadding = 15;
const char padding_char = ' ';

const std::string font_name = "Roboto Mono";

const uint32_t separater_size = 2;
const char separater_char = ' ';
const char amount_padding_char = '_';
const uint32_t tx_amount_font_size = 25;
const uint32_t amount_padding_length = 15;

// PRIVATE DECLARATIONS

static void SetPrintDataPanAndEntryMode(const Receipt::Map& dict,
                                        T_GL_HWIDGET layout,
                                        T_GL_DIM& line);

static void SetPrintDataRrnAndCardType(const Receipt::Map& dict,
                                       T_GL_HWIDGET layout,
                                       T_GL_DIM& line);

static void SetPrintDataAuthCodeAndTraceNo(const Receipt::Map& dict,
                                           T_GL_HWIDGET layout,
                                           T_GL_DIM& line);

static void SetPrintDataAuthCodeAndTraceNo(const Receipt::Map& dict,
                                           T_GL_HWIDGET layout,
                                           T_GL_DIM& line);

static void SetPrintDataExpiryDate(const Receipt::Map& dict,
                                   T_GL_HWIDGET layout,
                                   T_GL_DIM& line);

static void SetPrintDataEmvInformation(const Receipt::Map& dict,
                                       T_GL_HWIDGET layout,
                                       T_GL_DIM& line);

static void SetPrintDataConfirmedAmount(const Receipt::Map& dict,
                                        const std::string tittle,
                                        const std::string amount,
                                        const std::string currency,
                                        T_GL_HWIDGET layout,
                                        T_GL_DIM& line);

static void SetPrintDataBlankAmount(const Receipt::Map& dict,
                                    const std::string tittle,
                                    const std::string currency,
                                    T_GL_HWIDGET layout,
                                    T_GL_DIM& line);

// PUBLIC FUNCTIONS

void SetPrintDataLineFeed(T_GL_HWIDGET layout,
                          T_GL_DIM& line) {
  SetPrintData(layout, "", line++, GL_ALIGN_CENTER, tx_content_font_size,
               tx_content_font_size);
}

void SetPrintDataTxnTypeDebit(const Receipt::Map& dict,
                              T_GL_HWIDGET layout,
                              T_GL_DIM& line) {
  std::string text = Receipt::GetValue(dict, "TXN_TYPE");

  SetPrintData(layout, text.c_str(), line++, GL_ALIGN_CENTER, tx_type_font_size,
               tx_type_font_size,
               GL_FONT_STYLE_BOLD, false, GL_DIRECTION_WIDTH,
               font_name);
}

void SetPrintDataTxnTypeCredit(const Receipt::Map& dict,
                               T_GL_HWIDGET layout,
                               T_GL_DIM& line) {
  std::string text = Receipt::GetValue(dict, "TXN_TYPE");

  SetPrintData(layout, text.c_str(), line++, GL_ALIGN_CENTER, tx_type_font_size,
               tx_type_font_size,
               GL_FONT_STYLE_BOLD, true, GL_DIRECTION_WIDTH,
               font_name);
}

void SetPrintDataTransactionDetails(const Receipt::Map& dict,
                                    T_GL_HWIDGET layout,
                                    T_GL_DIM& line) {
  SetPrintDataLineFeed(layout, line);
  SetPrintDataPanAndEntryMode(dict, layout, line);
  SetPrintDataLineFeed(layout, line);
  SetPrintDataRrnAndCardType(dict, layout, line);
  SetPrintDataAuthCodeAndTraceNo(dict, layout, line);
  SetPrintDataExpiryDate(dict, layout, line);
  SetPrintDataEmvInformation(dict, layout, line);
  SetPrintDataLineFeed(layout, line);
}

void SetPrintDataAmountsWithTip(
    const Receipt::Map& dict,
    TerminalConfiguration::TipProcessingMode tip_mode,
    T_GL_HWIDGET layout,
    T_GL_DIM& line) {
  std::string currency = Receipt::GetValue(dict, "CURRENCY");
  std::string base_amount = Receipt::GetValue(dict, "AMOUNT");
  std::string tip_amount = Receipt::GetValue(dict, "TIP_AMOUNT");
  std::string total_amount = Receipt::GetValue(dict, "TOTAL_AMOUNT");
  std::string zero_amount = Receipt::GetValue(dict, "ZERO_AMOUNT");

  if (tip_amount.empty()) {
    tip_amount = zero_amount;
    total_amount = base_amount;
  }

  if (tip_mode == TerminalConfiguration::TIP_PROMPTED_AND_PRINTED) {
    SetPrintDataConfirmedAmount(dict, "BASE:", base_amount, currency, layout,
                                line);
    SetPrintDataConfirmedAmount(dict, "TIPS:", tip_amount, currency, layout,
                                line);
    SetPrintDataConfirmedAmount(dict, "TOTAL:", total_amount, currency, layout,
                                line);
  }
  else if (tip_mode == TerminalConfiguration::TIP_PRINTED_ONLY) {
    SetPrintDataConfirmedAmount(dict, "BASE:", base_amount, currency, layout,
                                line);

    SetPrintDataBlankAmount(dict, "TIPS:", currency, layout, line);
    SetPrintDataBlankAmount(dict, "TOTAL:", currency, layout, line);
  }
  else {
    SetPrintDataConfirmedAmount(dict, "TOTAL:", base_amount, currency, layout,
                                line);
  }
}

void SetPrintContentTextLeftAligned(std::string text,
                                    T_GL_HWIDGET layout,
                                    T_GL_DIM& line) {
  SetPrintData(layout, text.c_str(), line, GL_ALIGN_LEFT, tx_content_font_size,
               tx_content_font_size,
               GL_FONT_STYLE_BOLD, false,
               GL_DIRECTION_WIDTH,
               font_name);
}

void SetPrintContentTextCentertAligned(std::string text,
                                    T_GL_HWIDGET layout,
                                    T_GL_DIM& line) {
  SetPrintData(layout, text.c_str(), line, GL_ALIGN_CENTER, tx_content_font_size,
               tx_content_font_size,
               GL_FONT_STYLE_BOLD, false,
               GL_DIRECTION_WIDTH,
               font_name);
}

void SetPrintContentTextRightAligned(std::string text,
                                     T_GL_HWIDGET layout,
                                     T_GL_DIM& line) {
  SetPrintData(layout, text.c_str(), line, GL_ALIGN_RIGHT, tx_content_font_size,
               tx_content_font_size,
               GL_FONT_STYLE_BOLD, false,
               GL_DIRECTION_WIDTH,
               font_name);
}

void SetPrintAmountTextLeftAligned(std::string text, T_GL_HWIDGET layout,
                                   T_GL_DIM& line) {
  SetPrintData(layout, text.c_str(), line, GL_ALIGN_LEFT, tx_amount_font_size,
               tx_amount_font_size,
               GL_FONT_STYLE_BOLD, false,
               GL_DIRECTION_WIDTH,
               font_name);
}

void SetPrintAmountTextRightAligned(std::string text, T_GL_HWIDGET layout,
                                    T_GL_DIM& line) {
  SetPrintData(layout, text.c_str(), line, GL_ALIGN_RIGHT, tx_amount_font_size,
               tx_amount_font_size,
               GL_FONT_STYLE_BOLD, false,
               GL_DIRECTION_WIDTH,
               font_name);
}

std::string FormatMargin(std::string margin) {
  int pos = 0;
  do {
    if (margin[pos] != '0')
      break;

    if (margin[pos + 1] == '.')
      break;

    pos++;
  }
  while (true);

  return margin.substr(pos, margin.size() - pos);
}

void SetPrintDataDccInformation(const Receipt::Map& dict,
                                T_GL_HWIDGET layout,
                                T_GL_DIM& line) {
  std::string dcc_currency = Receipt::GetValue(dict, "DCC_CURRENCY");

  if (dcc_currency.empty())
    return;

  std::string local_currency = Receipt::GetValue(dict, "CURRENCY");
  std::string conversion_rate = Receipt::GetValue(dict, "CONVERSION_RATE");
  std::string local_amount = Receipt::GetValue(dict, "AMOUNT");
  std::string dcc_amount = Receipt::GetValue(dict, "DCC_AMOUNT");
  std::string dcc_margin = Receipt::GetValue(dict, "DCC_MARGIN");

  SetPrintDataLineFeed(layout, line);

  std::string fx_rate_text = "EXCHANGE RATE       : 1 " + dcc_currency + " = " + conversion_rate + " " + local_currency;
  SetPrintContentTextLeftAligned(fx_rate_text, layout, line);
  line++;

  std::string margin_fee_text = "MARGIN FEE          : " + FormatMargin(dcc_margin) + " %%";
  SetPrintContentTextLeftAligned(margin_fee_text, layout, line);
  line++;

  std::string txn_currency_text = "TRANSACTION CURRENCY: " + dcc_currency;
  SetPrintContentTextLeftAligned(txn_currency_text, layout, line);
  line++;

  SetPrintDataLineFeed(layout, line);

  SetPrintDataConfirmedAmount(dict, "TOTAL:", dcc_amount, dcc_currency, layout,
                               line);

  SetPrintDataLineFeed(layout, line);

  SetPrintContentTextCentertAligned(" [ Y ] I have been offered choices of", layout, line);
  line++;

  SetPrintContentTextCentertAligned("currencies and agree to pay in " + dcc_currency, layout, line);
  line++;

  SetPrintDataLineFeed(layout, line);
}

void SetPrintDataTotalAmountOnly(const Receipt::Map& dict,
                                 T_GL_HWIDGET layout,
                                 T_GL_DIM& line) {
  std::string currency = Receipt::GetValue(dict, "CURRENCY");
  std::string base_amount = Receipt::GetValue(dict, "AMOUNT");

  SetPrintDataConfirmedAmount(dict, "TOTAL:", base_amount, currency, layout,
                              line);
}

void SetPrintDataAmountsForVoid(
    const Receipt::Map& dict,
    TerminalConfiguration::TipProcessingMode tip_mode,
    T_GL_HWIDGET layout,
    T_GL_DIM& line) {

  std::string currency = Receipt::GetValue(dict, "CURRENCY");
  std::string base_amount = Receipt::GetValue(dict, "AMOUNT");
  std::string tip_amount = Receipt::GetValue(dict, "TIP_AMOUNT");
  std::string total_amount = Receipt::GetValue(dict, "TOTAL_AMOUNT");
  std::string zero_amount = Receipt::GetValue(dict, "ZERO_AMOUNT");
  std::string negative_sign = "-";

  if (tip_amount.empty()) {
    tip_amount = zero_amount;
    total_amount = base_amount;
  }

  base_amount = negative_sign + base_amount;
  total_amount = negative_sign + total_amount;
  if (tip_amount != zero_amount)
    tip_amount = negative_sign + tip_amount;

  if (tip_mode == TerminalConfiguration::TIP_DISABLED
      || tip_amount == zero_amount) {
    SetPrintDataConfirmedAmount(dict, "TOTAL:", base_amount, currency, layout,
                                line);
  }
  else
  {
    SetPrintDataConfirmedAmount(dict, "BASE:", base_amount, currency, layout,
                                line);
    SetPrintDataConfirmedAmount(dict, "TIPS:", tip_amount, currency, layout,
                                line);
    SetPrintDataConfirmedAmount(dict, "TOTAL:", total_amount, currency, layout,
                                line);
  }
}

void SetPrintDataAmountsForTipAdjust(
                                     const Receipt::Map& dict,
                                     T_GL_HWIDGET layout,
                                     T_GL_DIM& line) {
  std::string currency = Receipt::GetValue(dict, "CURRENCY");
  std::string base_amount = Receipt::GetValue(dict, "AMOUNT");
  std::string tip_amount = Receipt::GetValue(dict, "TIP_AMOUNT");
  std::string total_amount = Receipt::GetValue(dict, "TOTAL_AMOUNT");

  SetPrintDataConfirmedAmount(dict, "BASE:", base_amount, currency, layout,
                              line);
  SetPrintDataConfirmedAmount(dict, "TIPS:", tip_amount, currency, layout,
                              line);
  SetPrintDataConfirmedAmount(dict, "TOTAL:", total_amount, currency, layout,
                              line);
}

void SetPrintDataTenureInformation(const Receipt::Map& dict,
                                   T_GL_HWIDGET layout,
                                   T_GL_DIM& line) {
  std::string text = "Tenure : " + Receipt::GetValue(dict, "TENURE")
      + " Months";
  SetPrintData(layout, text.c_str(), line++, GL_ALIGN_LEFT,
               tx_content_font_size,
               tx_content_font_size, GL_FONT_STYLE_BOLD,
               false,
               GL_DIRECTION_WIDTH, font_name);

  SetPrintData(layout, "Estimate Monthly Repayment : ", line++, GL_ALIGN_LEFT,
               tx_content_font_size,
               tx_content_font_size, GL_FONT_STYLE_BOLD,
               false,
               GL_DIRECTION_WIDTH, font_name);

  text = Receipt::GetValue(dict, "CURRENCY") + "  "
      + Receipt::GetValue(dict, "EMI");
  SetPrintData(layout, text.c_str(), line++, GL_ALIGN_RIGHT,
               tx_content_font_size,
               tx_content_font_size, GL_FONT_STYLE_BOLD,
               false,
               GL_DIRECTION_WIDTH, font_name);
}

void SetPrintDataHostResponseText(const Receipt::Map& dict,
                                  T_GL_HWIDGET layout,
                                  T_GL_DIM& line) {
  std::string hostResponse1 = Receipt::GetValue(dict, "HOST_RESP_1").c_str();
  std::string hostResponse2 = Receipt::GetValue(dict, "HOST_RESP_2").c_str();

  SetPrintData(layout, "", line++, GL_ALIGN_CENTER, tx_content_font_size,
               tx_content_font_size);

  if (!hostResponse1.empty())
    SetPrintData(layout, hostResponse1.c_str(), line++, GL_ALIGN_CENTER,
                 tx_content_font_size,
                 tx_content_font_size);

  if (!hostResponse2.empty())
    SetPrintData(layout, hostResponse2.c_str(), line++, GL_ALIGN_CENTER,
                 tx_content_font_size,
                 tx_content_font_size);
}

// PRIVATE FUNCTIONS

static void SetPrintDataPanAndEntryMode(const Receipt::Map& dict,
                                        T_GL_HWIDGET layout,
                                        T_GL_DIM& line) {
  std::string text = "CARD NUM: " + Receipt::GetValue(dict, "PAN") + " "
      + Receipt::GetValue(dict, "ENTRY_MODE");

  SetPrintData(layout, text.c_str(), line++, GL_ALIGN_LEFT,
               tx_pan_font_size,
               tx_pan_font_size, GL_FONT_STYLE_BOLD,
               false,
               GL_DIRECTION_WIDTH, std::string(), GL_UNIT_PIXEL
               );
}

static void SetPrintDataRrnAndCardType(const Receipt::Map& dict,
                                       T_GL_HWIDGET layout,
                                       T_GL_DIM& line) {
  std::string text = "RREF NUM: " + Receipt::GetValue(dict, "RRN");

  SetPrintData(layout, text.c_str(), line, GL_ALIGN_LEFT, tx_content_font_size,
               tx_content_font_size,
               GL_FONT_STYLE_BOLD, false,
               GL_DIRECTION_WIDTH,
               font_name);

  text = "CARD TYPE: " + Receipt::GetValue(dict, "CARD_TYPE");

  if (kRightPadding > text.size())
    text.append(kRightPadding - text.size(), padding_char);

  SetPrintData(layout, text.c_str(), line++, GL_ALIGN_RIGHT,
               tx_content_font_size,
               tx_content_font_size, GL_FONT_STYLE_BOLD,
               false,
               GL_DIRECTION_WIDTH, font_name);

}

static void SetPrintDataAuthCodeAndTraceNo(const Receipt::Map& dict,
                                           T_GL_HWIDGET layout,
                                           T_GL_DIM& line) {
  std::string text = "APPROVAL CODE: " + Receipt::GetValue(dict, "AUTH_CODE");

  SetPrintData(layout, text.c_str(), line, GL_ALIGN_LEFT, tx_content_font_size,
               tx_content_font_size,
               GL_FONT_STYLE_BOLD, false,
               GL_DIRECTION_WIDTH,
               font_name);

  text = "TRC NUM: " + Receipt::GetValue(dict, "TRACE_NO");
  if (kRightPadding > text.size())
    text.append(kRightPadding - text.size(), padding_char);
  SetPrintData(layout, text.c_str(), line++, GL_ALIGN_RIGHT,
               tx_content_font_size,
               tx_content_font_size, GL_FONT_STYLE_BOLD,
               false,
               GL_DIRECTION_WIDTH, font_name);
}

static void SetPrintDataExpiryDate(const Receipt::Map& dict,
                                   T_GL_HWIDGET layout,
                                   T_GL_DIM& line) {
  std::string text = "EXP DATE: " + Receipt::GetValue(dict, "EXP_DATE");

  SetPrintData(layout, text.c_str(), line++, GL_ALIGN_LEFT,
               tx_content_font_size,
               tx_content_font_size, GL_FONT_STYLE_BOLD,
               false,
               GL_DIRECTION_WIDTH, font_name);

}

static void SetPrintDataEmvInformation(const Receipt::Map& dict,
                                       T_GL_HWIDGET layout,
                                       T_GL_DIM& line) {
  std::string AID = Receipt::GetValue(dict, "AID").c_str();

  if (!AID.empty())
  {
    std::string text = "TC: " + Receipt::GetValue(dict, "TC");
    SetPrintData(layout, text.c_str(), line, GL_ALIGN_LEFT,
                 tx_content_font_size,
                 tx_content_font_size, GL_FONT_STYLE_BOLD,
                 false,
                 GL_DIRECTION_WIDTH, font_name);

    text = "AID: " + AID;
    if (AID.size() <= 16) {
      SetPrintData(layout, text.c_str(), line++, GL_ALIGN_RIGHT,
                 tx_content_font_size,
                 tx_content_font_size, GL_FONT_STYLE_BOLD,
                 false,
                 GL_DIRECTION_WIDTH, font_name);
    }
    else {
      line++;
      SetPrintData(layout, text.c_str(), line++, GL_ALIGN_LEFT,
                       tx_content_font_size,
                       tx_content_font_size, GL_FONT_STYLE_BOLD,
                       false,
                       GL_DIRECTION_WIDTH, font_name);
    }

    text = "Appl Label: " + Receipt::GetValue(dict, "APP_NAME");
    SetPrintData(layout, text.c_str(), line++, GL_ALIGN_LEFT,
                 tx_content_font_size,
                 tx_content_font_size, GL_FONT_STYLE_BOLD,
                 false,
                 GL_DIRECTION_WIDTH, font_name);
  }
}

static void SetPrintDataConfirmedAmount(const Receipt::Map& dict,
                                        const std::string tittle,
                                        const std::string amount,
                                        const std::string currency,
                                        T_GL_HWIDGET layout,
                                        T_GL_DIM& line) {
  SetPrintData(layout, tittle.c_str(), line, GL_ALIGN_LEFT, tx_amount_font_size,
               tx_amount_font_size,
               GL_FONT_STYLE_BOLD, false,
               GL_DIRECTION_WIDTH,
               font_name);
  SetPrintData(
               layout,
               GetAmountPrintingFormat(currency, amount, amount_padding_length,
                                       padding_char).c_str(),
               line++, GL_ALIGN_RIGHT, tx_amount_font_size, tx_amount_font_size,
               GL_FONT_STYLE_BOLD,
               false, GL_DIRECTION_WIDTH, font_name);
}

static void SetPrintDataBlankAmount(const Receipt::Map& dict,
                                    const std::string tittle,
                                    const std::string currency,
                                    T_GL_HWIDGET layout,
                                    T_GL_DIM& line) {
  SetPrintData(layout, tittle.c_str(), line, GL_ALIGN_LEFT, tx_amount_font_size,
               tx_amount_font_size,
               GL_FONT_STYLE_BOLD, false,
               GL_DIRECTION_WIDTH,
               font_name);

  SetPrintData(
      layout,
      GetAmountInputPrintingFormat(currency, separater_size, separater_char,
                                   amount_padding_length,
                                   amount_padding_char)
          .c_str(),
      line++, GL_ALIGN_RIGHT, tx_amount_font_size, tx_amount_font_size,
      GL_FONT_STYLE_BOLD,
      false, GL_DIRECTION_WIDTH, font_name);

}

}
