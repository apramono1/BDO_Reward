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
#include "receipt_settlement_footer.h"
#include <stdx/string>
#include <sdk.h>
#include <GL_GraphicLib.h>

namespace bdor {
ReceiptSettlementFooter::ReceiptSettlementFooter(tpcore::GoalHandle& gl,
                                                 unsigned printer_timeout,
                                                 unsigned int host_index)
    : GoalReceipt(gl, printer_timeout),
      host_index_(stdx::to_string(host_index)) {
}

void ReceiptSettlementFooter::Expand(const Receipt::Map& dict) {

  T_GL_HWIDGET layout = GetGoalRootLayout();

  T_GL_HWIDGET print;
  T_GL_DIM line = 0;

  std::string host_index_postfix = "_" + host_index_;

  //disclaimer line 1
  if (dict.count("SETTLEMENT_STATUS" + host_index_postfix) > 0) {
    print = GL_Print_Create(layout);
    std::string text = "SETTLEMENT "
        + GetValue(dict, "SETTLEMENT_STATUS" + host_index_postfix) + "\n";
    GL_Widget_SetText(print, text.c_str());
    GL_Widget_SetItem(print, 0, line++);
    GL_Widget_SetFontSize(print, 25, 25, GL_UNIT_PIXEL);
    GL_Widget_SetBackAlign(print, GL_ALIGN_CENTER);
    if (GetValue(dict, "SETTLEMENT_STATUS" + host_index_postfix) == "FAILED") {
      GL_Widget_SetBackColor(print, GL_COLOR_BLACK);
      GL_Widget_SetForeColor(print, GL_COLOR_WHITE);
      GL_Widget_SetExpand(print, GL_DIRECTION_WIDTH);
    }
  }
//batch upload
  if (dict.count("BATCH_UPLOAD" + host_index_postfix) > 0) {
    print = GL_Print_Create(layout);
    GL_Widget_SetText(
        print, "%s\n",
        GetValue(dict, "BATCH_UPLOAD" + host_index_postfix).c_str());
    GL_Widget_SetItem(print, 0, line++);
    GL_Widget_SetFontSize(print, 25, 25, GL_UNIT_PIXEL);
    GL_Widget_SetBackAlign(print, GL_ALIGN_CENTER);
  }

}

}

