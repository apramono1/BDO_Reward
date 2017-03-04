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
#include "receipt_settlement_version.h"
#include <stdx/string>
#include <sdk.h>
#include <GL_GraphicLib.h>

namespace bdor {
ReceiptSettlementVersion::ReceiptSettlementVersion(tpcore::GoalHandle& gl,
                                                 unsigned printer_timeout)
    : GoalReceipt(gl, printer_timeout) {
}

void ReceiptSettlementVersion::Expand(const Receipt::Map& dict) {

  T_GL_HWIDGET layout = GetGoalRootLayout();

  T_GL_HWIDGET print;
  T_GL_DIM line = 0;

//version
  print = GL_Print_Create(layout);
  GL_Widget_SetText(print, "%s\n\n\n\n\n", GetValue(dict, "VERSION").c_str());
  GL_Widget_SetItem(print, 0, line++);
  GL_Widget_SetFontSize(print, 25, 25, GL_UNIT_PIXEL);
  GL_Widget_SetBackAlign(print, GL_ALIGN_CENTER);

//end of life

}

}

