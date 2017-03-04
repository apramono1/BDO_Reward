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
#include <stdx/string>
#include <sdk.h>
#include <GL_GraphicLib.h>
#include "receipt_formatter.h"
#include <receipt_detail_report_header.h>

namespace bdor {

ReceiptDetailReportHeader::ReceiptDetailReportHeader(tpcore::GoalHandle& gl,
                                                     unsigned printer_timeout)
    : GoalReceipt(gl, printer_timeout) {
}

void ReceiptDetailReportHeader::Expand(const Receipt::Map& dict) {

  T_GL_HWIDGET layout = GetGoalRootLayout();

  T_GL_DIM line = 0;

  std::string text;

  std::string dot_line = "---------------------------------------";

  text = "DATE: " + GetValue(dict, "DATE");
  SetPrintNormalLine(layout, text, line, GL_ALIGN_LEFT);

  text = "TIME: " + GetValue(dict, "TIME");
  SetPrintNormalLine(layout, text, line++, GL_ALIGN_RIGHT);

  text = "MID: " + GetValue(dict, "MID");
  SetPrintNormalLine(layout, text, line, GL_ALIGN_LEFT);

  text = "TID: " + GetValue(dict, "TID");
  SetPrintNormalLine(layout, text, line++, GL_ALIGN_RIGHT);

  text = "BATCH NO: " + GetValue(dict, "BATCH_NO");
  SetPrintNormalLine(layout, text, line, GL_ALIGN_LEFT);

  text = "HOST: " + GetValue(dict, "HOST");
  SetPrintNormalLine(layout, text, line++, GL_ALIGN_RIGHT);

  SetPrintTitleLine(layout, "\nDETAIL REPORT\n", line++);
  SetPrintNormalLine(layout, "CARD NAME", line, GL_ALIGN_LEFT);
  SetPrintNormalLine(layout, "CARD NUMBER", line++, GL_ALIGN_RIGHT);

  SetPrintNormalLine(layout, "EXP DATE", line, GL_ALIGN_LEFT);
  SetPrintNormalLine(layout, "TRACE NUM", line++, GL_ALIGN_RIGHT);

  SetPrintNormalLine(layout, "TRANSACTION", line, GL_ALIGN_LEFT);
  SetPrintNormalLine(layout, "AMOUNT", line++, GL_ALIGN_RIGHT);

  SetPrintNormalLine(layout, "APPROVAL CODE", line, GL_ALIGN_LEFT);
  SetPrintNormalLine(layout, "INVOICE", line++, GL_ALIGN_RIGHT);

  SetPrintNormalLine(layout, "TX DATE", line, GL_ALIGN_LEFT);
  SetPrintNormalLine(layout, "TX TIME", line++, GL_ALIGN_RIGHT);

  SetPrintNormalLine(layout, dot_line, line++, GL_ALIGN_RIGHT);
}

}
