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
#include <bdor/receipt_body_detail_report.h>
#include <receipt_formatter.h>

using namespace bdor;

void ReceiptBodyDetailReport::Expand(
    const std::map<std::string, std::string>& dict) {

  T_GL_HWIDGET layout = GetGoalRootLayout();

  T_GL_DIM line = 0;
  std::string text;

  text = GetValue(dict, "TXN_TYPE") + "     " + GetValue(dict, "INVOICE_NO");
  SetPrintData(layout, text.c_str(), line, GL_ALIGN_LEFT, 20, 20);
  text = GetValue(dict, "AUTH_CODE");
  SetPrintData(layout, text.c_str(), line++, GL_ALIGN_RIGHT, 20, 20);

  text = GetValue(dict, "PAN");
  SetPrintData(layout, text.c_str(), line, GL_ALIGN_LEFT, 20, 20);
  text = GetValue(dict, "EXP_DATE");
  SetPrintData(layout, text.c_str(), line++, GL_ALIGN_RIGHT, 20, 20);

  SetPrintData(layout, "₹", line, GL_ALIGN_LEFT, 24, 24, GL_FONT_STYLE_BOLD,
               false, GL_DIRECTION_WIDTH, "rupee symbol font");
  text = GetValue(dict, "AMOUNT");
  SetPrintData(layout, text.c_str(), line, GL_ALIGN_CENTER, 20, 20);
  text = GetValue(dict, "CARD_TYPE");
  SetPrintData(layout, text.c_str(), line++, GL_ALIGN_RIGHT, 20, 20);

  text = "\n\n";
  SetPrintData(layout, text.c_str(), line++, GL_ALIGN_CENTER, 20, 20);

  text = "ISSUER: " + GetValue(dict, "ISSUER");
  SetPrintData(layout, text.c_str(), line, GL_ALIGN_LEFT, 20, 20);
  text = "HOST: " + GetValue(dict, "HOST");
  SetPrintData(layout, text.c_str(), line++, GL_ALIGN_RIGHT, 20, 20);

  text =
      "-----------------------------------------------------------------------------";
  SetPrintData(layout, text.c_str(), line++, GL_ALIGN_CENTER, 20, 20);

  SetPrintData(layout, "TRANSACTION    COUNT", line, GL_ALIGN_LEFT, 20, 20);
  SetPrintData(layout, "AMOUNT", line++, GL_ALIGN_RIGHT, 20, 20);

  text =
      "-----------------------------------------------------------------------------";
  SetPrintData(layout, text.c_str(), line++, GL_ALIGN_CENTER, 20, 20);

  //total sale count and total amount
  text = "SALE                    " + GetValue(dict, "SALE_COUNT");
  SetPrintData(layout, text.c_str(), line, GL_ALIGN_LEFT, 20, 20);

  SetPrintData(layout, "₹", line, GL_ALIGN_CENTER, 24, 24, GL_FONT_STYLE_BOLD,
               false, GL_DIRECTION_WIDTH, "rupee symbol font");

  SetPrintData(layout, GetValue(dict, "SALE_AMOUNT").c_str(), line++,
               GL_ALIGN_RIGHT, 20, 20);

  //total void count and total amount
  text = "VOID                     " + GetValue(dict, "VOID_COUNT");
  SetPrintData(layout, text.c_str(), line, GL_ALIGN_LEFT, 20, 20);

  SetPrintData(layout, "₹", line, GL_ALIGN_CENTER, 24, 24, GL_FONT_STYLE_BOLD,
               false, GL_DIRECTION_WIDTH, "rupee symbol font");

  SetPrintData(layout, GetValue(dict, "VOID_AMOUNT").c_str(), line++,
               GL_ALIGN_RIGHT, 20, 20);

  //total refund count and total amount
  text = "REFUND               " + GetValue(dict, "REFUND_COUNT");
  SetPrintData(layout, text.c_str(), line, GL_ALIGN_LEFT, 20, 20);

  SetPrintData(layout, "₹", line, GL_ALIGN_CENTER, 24, 24, GL_FONT_STYLE_BOLD,
               false, GL_DIRECTION_WIDTH, "rupee symbol font");

  SetPrintData(layout, GetValue(dict, "REFUND_AMOUNT").c_str(), line++,
               GL_ALIGN_RIGHT, 20, 20);

  //total void of refund count and total amount
  text = "VOID REFUND      " + GetValue(dict, "VOID_REFUND_COUNT");
  SetPrintData(layout, text.c_str(), line, GL_ALIGN_LEFT, 20, 20);

  SetPrintData(layout, "₹", line, GL_ALIGN_CENTER, 24, 24, GL_FONT_STYLE_BOLD,
               false, GL_DIRECTION_WIDTH, "rupee symbol font");

  SetPrintData(layout, GetValue(dict, "VOID_REFUND_AMOUNT").c_str(), line++,
               GL_ALIGN_RIGHT, 20, 20);

  //----
  text =
      "-----------------------------------------------------------------------------";
  SetPrintData(layout, text.c_str(), line++, GL_ALIGN_CENTER, 20, 20);

  //total count of batch and total amount
  text = "TOTAL                  " + GetValue(dict, "TOTAL_COUNT");
  SetPrintData(layout, text.c_str(), line, GL_ALIGN_LEFT, 20, 20);
  SetPrintData(layout, "₹", line, GL_ALIGN_CENTER, 24, 24, GL_FONT_STYLE_BOLD,
               false, GL_DIRECTION_WIDTH, "rupee symbol font");
  SetPrintData(layout, GetValue(dict, "TOTAL_AMOUNT").c_str(), line++,
               GL_ALIGN_RIGHT, 22, 22);
  //----
  text =
      "-----------------------------------------------------------------------------";
  SetPrintData(layout, text.c_str(), line++, GL_ALIGN_CENTER, 20, 20);

  text = "\n\n";
  SetPrintData(layout, text.c_str(), line++, GL_ALIGN_CENTER, 20, 20);

  //end of receipt
}

