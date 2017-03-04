/*
 ------------------------------------------------------------------------------
 INGENICO Technical Software Department
 ------------------------------------------------------------------------------
 Copyright (c) 2514 INGENICO S.A.
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
#include <duplicate_receipt_header.h>
#include <sdk.h>
#include <GL_GraphicLib.h>
#include "receipt_formatter.h"

namespace bdor {

DuplicateReceiptHeader::DuplicateReceiptHeader(tpcore::GoalHandle& gl,
                                               unsigned printer_timeout)
    : GoalReceipt(gl, printer_timeout) {

}

void DuplicateReceiptHeader::Expand(const Receipt::Map& dict) {

  T_GL_HWIDGET layout = GetGoalRootLayout();

  T_GL_DIM line = 0;

  //DUPLICATE BILL
  SetPrintData(layout, "*** DUPLICATE BILL ***\n", line++, GL_ALIGN_CENTER, 35,
               35);

  //end of life

}

}
