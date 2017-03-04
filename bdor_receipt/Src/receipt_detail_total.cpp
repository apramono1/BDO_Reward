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
#include <receipt_detail_total.h>
#include <stdx/string>
#include <types/amount.h>
#include <utils/strings.h>
#include <receipt_formatter.h>

namespace bdor {

namespace {

void AddItem(T_GL_HWIDGET* layout, T_GL_DIM* line,
             const receipt::Receipt::Map& dict,
             const int index) {

  using receipt::Receipt;
  std::string postfix = "_" + utils::ToString(index);
  std::string currency = Receipt::GetValue(dict, "CURRENCY");
  std::string card_name = Receipt::GetValue(dict, "CARD_NAME" + postfix);
  std::string count = Receipt::GetValue(dict, "TOTAL_COUNT" + postfix);
  std::string total_str = Receipt::GetValue(dict, "TOTAL_AMOUNT" + postfix);
    types::Amount amount(currency,utils::FromString<uint64_t>(total_str));
    std::string amount_str = currency + " "+ amount.ToString();

  SetPrintNormalLine(*layout, card_name, *line, GL_ALIGN_LEFT);
  SetPrintNormalLine(*layout, count, *line, GL_ALIGN_CENTER);
  SetPrintNormalLine(*layout, amount_str, *line, GL_ALIGN_RIGHT);
}

void AddItem(T_GL_HWIDGET* layout, T_GL_DIM* line,
             const receipt::Receipt::Map& dict,
             const std::string& currency,
             const std::string& card_name,
             const std::string& count_key,
             const std::string& amount_key) {

  using receipt::Receipt;

  SetPrintNormalLine(*layout,
                     Receipt::GetValue(dict, card_name.c_str()).c_str(),
                     *line,
                     GL_ALIGN_LEFT);

  SetPrintNormalLine(*layout,
                     Receipt::GetValue(dict, count_key.c_str()).c_str(),
                     *line,
                     GL_ALIGN_CENTER);

  std::string total_str = Receipt::GetValue(dict, amount_key.c_str());
  types::Amount amount(currency,utils::FromString<uint64_t>(total_str));
  std::string amount_str = currency + " "+ amount.ToString();

  SetPrintNormalLine(*layout, amount_str.c_str(), *line, GL_ALIGN_RIGHT);

}

}

void ReceiptDetailTotal::Expand(const Receipt::Map& dict) {
  T_GL_HWIDGET layout = GetGoalRootLayout();

  T_GL_DIM line = 0;
  std::string text;
  std::string currency = GetValue(dict, "CURRENCY");

  SetPrintNormalLine(layout, "\n\nSUMMARY\n", line++, GL_ALIGN_CENTER);
  SetPrintNormalLine(layout, "CARDTYPE", line, GL_ALIGN_LEFT);
  SetPrintNormalLine(layout, "COUNT", line, GL_ALIGN_CENTER);
  SetPrintNormalLine(layout, "AMOUNT", line++, GL_ALIGN_RIGHT);

  for (int i = 0; i < card_type_number_; i++)
      {
    AddItem(&layout, &line, dict, i);
  }

  line++;
  std::string dot_line = "---------------------------------------";
  SetPrintNormalLine(layout, dot_line.c_str(), line++, GL_ALIGN_CENTER);

  AddItem(&layout, &line, dict, currency, "GRAND_TOTAL_CARD_NAME",
          "GRAND_TOTAL_COUNT",
          "GRAND_TOTAL_AMOUNT");
}

}
