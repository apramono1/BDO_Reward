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
#include "reversal_receipt.h"

#include <stdx/memory>
#include <bdor/receipts.h>
#include "receipt_header.h"
#include "receipt_transaction_header.h"
#include "receipt_formatter.h"

using stdx::make_unique;

namespace bdor {

ReversalReceipt::ReversalReceipt(tpcore::GoalHandle& gl,
                                 unsigned printer_timeout)
    : GoalCompositeReceipt(gl, printer_timeout) {

  Add(make_unique<ReceiptHeader>(gl_, printer_timeout));
  Add(make_unique<ReceiptTransactionHeader>(gl_, printer_timeout, true));
  Add(make_unique<Body>(gl_, printer_timeout));
}

ReversalReceipt::Body::Body(tpcore::GoalHandle& gl, unsigned printer_timeout)
    : GoalReceipt(gl, printer_timeout) {
}

void ReversalReceipt::Body::Expand(const Receipt::Map& dict) {

  T_GL_HWIDGET layout = GetGoalRootLayout();

  T_GL_DIM line = 0;
  std::string text;

  bool is_emv = false;

  std::string AID = GetValue(dict, "AID").c_str();
  if (!AID.empty())
    is_emv = true;

  SetPrintData(layout, "TRANSACTION FAILED", line++, GL_ALIGN_CENTER, 35, 35,
               GL_FONT_STYLE_BOLD,
               true);

  //transaction type
  SetPrintData(layout, GetValue(dict, "TXN_TYPE").c_str(), line++,
               GL_ALIGN_CENTER,
               35, 35);

  // APP NAME(in case of EMV transaction)
  if (is_emv) {
    text = "APP NAME: ", GetValue(dict, "APP_NAME");
    SetPrintData(layout, text.c_str(), line++, GL_ALIGN_LEFT);
  }

  //PAN
  SetPrintData(layout, GetValue(dict, "PAN").c_str(), line++, GL_ALIGN_CENTER);

  if (is_emv) {
    text = "EXP DATE: " + GetValue(dict, "EXP_DATE");
    SetPrintData(layout, text.c_str(), line, GL_ALIGN_LEFT);

    text = "AID: " + GetValue(dict, "AID");
    SetPrintData(layout, text.c_str(), line++, GL_ALIGN_RIGHT);
  } else {
    text = "EXP DATE: " + GetValue(dict, "EXP_DATE");
    SetPrintData(layout, text.c_str(), line++, GL_ALIGN_LEFT);
  }

  // CARD TYPE and ENTRY MODE
  text = "CARD TYPE: " + GetValue(dict, "CARD_TYPE");
  SetPrintData(layout, text.c_str(), line, GL_ALIGN_LEFT, 22, 22);

  text = "ENTRY MODE: " + GetValue(dict, "ENTRY_MODE");
  SetPrintData(layout, text.c_str(), line++, GL_ALIGN_RIGHT, 22, 22);

  SetPrintData(layout, "TRANSACTION FAILED", line++, GL_ALIGN_CENTER, 35, 35,
               GL_FONT_STYLE_BOLD,
               true);

  SetPrintData(layout, "Please Contact Your Card Issuer", line++,
               GL_ALIGN_CENTER,
               25, 25, GL_FONT_STYLE_BOLD, true);

  SetPrintData(layout, "For Reversal Of Debit If Any.", line++, GL_ALIGN_CENTER,
               25,
               25, GL_FONT_STYLE_BOLD, true);

  SetPrintData(layout, "\n\n\n", line, GL_ALIGN_CENTER);
}

}
