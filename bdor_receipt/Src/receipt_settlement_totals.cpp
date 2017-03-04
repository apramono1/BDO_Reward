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
#include <receipt_settlement_totals.h>
#include <stdx/string>
#include <types/amount.h>
#include <utils/strings.h>
#include <receipt_formatter.h>

namespace bdor {

namespace {

void AddItem(T_GL_HWIDGET* layout,
             T_GL_DIM* line,
             const receipt::Receipt::Map& dict,
             const std::string& currency,
             const std::string& title,
             const std::string& count_key,
             const std::string& amount_key) {

  using receipt::Receipt;

  const int kFontSize = 16;
  SetPrintData(*layout, title.c_str(), *line, GL_ALIGN_LEFT, kFontSize, kFontSize);

  std::string count_str = Receipt::GetValue(dict, count_key.c_str());
  std::string center_str = count_str + "        " + currency;
  SetPrintData(*layout,
               center_str.c_str(),
               *line,
               GL_ALIGN_CENTER,
               kFontSize,
               kFontSize);

  std::string amount_value_str = Receipt::GetValue(dict, amount_key.c_str());
  types::Amount amount(currency, utils::FromString<std::uint64_t>(amount_value_str));
  std::string total_str = amount.ToString();

  SetPrintData(*layout,
               total_str.c_str(),
               *line,
               GL_ALIGN_RIGHT,
               kFontSize,
               kFontSize);

}

}

void ReceiptSettlementTotals::Expand(const Receipt::Map& dict) {
  T_GL_HWIDGET layout = GetGoalRootLayout();

  T_GL_DIM line = 0;
  std::string kSpaceLine = " ";
  int kBodyFontSize = 16;

  std::string postfix = "_" + host_index_ + "_" + index_;
  std::string title;
  if (is_grand_total_) {
    title = "GRAND TOTALS:";
  } else {
    title = "CARD NAME: ";
    title += GetValue(dict, ("CARD_NAME" + postfix).c_str());
  }

  SetPrintData(layout, title.c_str(), line++, GL_ALIGN_LEFT, kBodyFontSize,
               kBodyFontSize);
  SetPrintData(layout, "TRANSACTION", line, GL_ALIGN_LEFT, kBodyFontSize,
               kBodyFontSize);
  SetPrintData(layout, "COUNT", line, GL_ALIGN_CENTER, kBodyFontSize,
               kBodyFontSize);
  SetPrintData(layout, "AMOUNT", line++, GL_ALIGN_RIGHT, kBodyFontSize,
               kBodyFontSize);

  SetPrintData(layout, kSpaceLine.c_str(), line++, GL_ALIGN_CENTER,
               kBodyFontSize,
               kBodyFontSize);

  std::string currency = GetValue(dict, "CURRENCY");

  line++;
  AddItem(&layout,
          &line,
          dict,
          currency,
          "SALES",
          "SALE_COUNT" + postfix,
          "SALE_AMOUNT" + postfix);

  line++;
  AddItem(&layout,
          &line,
          dict,
          currency,
          "TIPS",
          "TIPS_COUNT" + postfix,
          "TIPS_AMOUNT" + postfix);

  line++;
  AddItem(&layout,
          &line,
          dict,
          currency,
          "REFUNDS",
          "REFUND_COUNT" + postfix,
          "REFUND_AMOUNT" + postfix);

  line++;
  AddItem(&layout,
          &line,
          dict,
          currency,
          "OFFLINE SALE",
          "OFFLINE_SALE_COUNT" + postfix,
          "OFFLINE_SALE_AMOUNT" + postfix);

  line++;
  AddItem(&layout,
          &line,
          dict,
          currency,
          "VOID SALE",
          "VOID_COUNT" + postfix,
          "VOID_AMOUNT" + postfix);

  line++;
  AddItem(&layout,
          &line,
          dict,
          currency,
          "VOID REFUND",
          "VOID_REFUND_COUNT" + postfix,
          "VOID_REFUND_AMOUNT" + postfix);

  std::string kDotLine =
      "---------------------------------------------------------";
  line++;
  SetPrintData(layout, kDotLine.c_str(), line, GL_ALIGN_CENTER,
               kBodyFontSize,
               kBodyFontSize);

  if (is_grand_total_) {
    title = "TOTALS";
  } else {
    title = "CARD TOTALS";
  }

  line++;
  AddItem(&layout,
            &line,
            dict,
            currency,
            title,
            "TOTAL_COUNT" + postfix,
            "TOTAL_AMOUNT" + postfix);

  line++;
  SetPrintData(layout, kSpaceLine.c_str(), line, GL_ALIGN_CENTER,
               kBodyFontSize,
               kBodyFontSize);
}

}
