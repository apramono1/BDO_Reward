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
#include <receipt_detail_transaction.h>
#include <stdx/string>
#include <utils/strings.h>
#include <receipt_formatter.h>

namespace bdor {

void ReceiptDetailTransaction::Expand(const Receipt::Map& dict) {
  T_GL_HWIDGET layout = GetGoalRootLayout();
  T_GL_DIM line = 0;
  std::string dot_line = "---------------------------------------\n";

  std::string postfix = "_" + utils::ToString(transaction_number_);
  std::string text;

  SetPrintNormalLine(layout, GetValue(dict, ("CARD_NAME" + postfix).c_str()), line,
                  GL_ALIGN_LEFT);
  SetPrintNormalLine(layout, GetValue(dict, ("PAN" + postfix).c_str()), line++,
                  GL_ALIGN_RIGHT);

  SetPrintNormalLine(layout, GetValue(dict, ("EXP_DATE" + postfix).c_str()), line,
                  GL_ALIGN_LEFT);
  SetPrintNormalLine(layout, GetValue(dict, ("TRACE_NO" + postfix).c_str()),
                  line++,
                  GL_ALIGN_RIGHT);

  SetPrintNormalLine(layout, GetValue(dict, ("TXN_TYPE" + postfix).c_str()), line,
                  GL_ALIGN_LEFT);
  SetPrintNormalLine(layout, GetValue(dict, ("AMOUNT" + postfix).c_str()), line++,
                  GL_ALIGN_RIGHT);

  SetPrintNormalLine(layout, GetValue(dict, ("AUTH_CODE" + postfix).c_str()), line,
                  GL_ALIGN_LEFT);
  SetPrintNormalLine(layout, GetValue(dict, ("INVOICE" + postfix).c_str()), line++,
                  GL_ALIGN_RIGHT);

  SetPrintNormalLine(layout, GetValue(dict, ("DATE" + postfix).c_str()), line,
                  GL_ALIGN_LEFT);
  SetPrintNormalLine(layout, GetValue(dict, ("TIME" + postfix).c_str()), line++,
                  GL_ALIGN_RIGHT);
  SetPrintNormalLine(layout, dot_line, line++, GL_ALIGN_CENTER);
}

}
