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
#include <receipt_settlement_header.h>

namespace bdor {

 ReceiptSettlementHeader::ReceiptSettlementHeader(tpcore::GoalHandle& gl,
                                                  unsigned  printer_timeout,
                                                  unsigned int host_index)
    : GoalReceipt(gl, printer_timeout),
      host_index_(stdx::to_string(host_index)) {
}

void ReceiptSettlementHeader::Expand(const Receipt::Map& dict) {

  T_GL_HWIDGET layout = GetGoalRootLayout();

  T_GL_HWIDGET print;
  T_GL_DIM line = 0;

  int header_font_size = 12;

  std::string postfix = "_" + host_index_;

  //DATE and TIME
   print = GL_Print_Create(layout);
   GL_Widget_SetText(print, "DATE: %s", GetValue(dict, "DATE" + postfix).c_str());
   GL_Widget_SetItem(print, 0, line);
   GL_Widget_SetFontSize(print, header_font_size, header_font_size, GL_UNIT_PIXEL);
   GL_Widget_SetBackAlign(print, GL_ALIGN_LEFT);

   print = GL_Print_Create(layout);
   GL_Widget_SetText(print, "TIME: %s", GetValue(dict, "TIME" + postfix).c_str());
   GL_Widget_SetItem(print, 0, line++);
   GL_Widget_SetFontSize(print, header_font_size, header_font_size, GL_UNIT_PIXEL);
   GL_Widget_SetBackAlign(print, GL_ALIGN_RIGHT);

  //TID and MID
  print = GL_Print_Create(layout);
  GL_Widget_SetText(print, "MID: %s", GetValue(dict, "MID" + postfix).c_str());
  GL_Widget_SetItem(print, 0, line);
  GL_Widget_SetFontSize(print, header_font_size, header_font_size, GL_UNIT_PIXEL);
  GL_Widget_SetBackAlign(print, GL_ALIGN_LEFT);

  print = GL_Print_Create(layout);
  GL_Widget_SetText(print, "TID: %s", GetValue(dict, "TID" + postfix).c_str());
  GL_Widget_SetItem(print, 0, line++);
  GL_Widget_SetFontSize(print, header_font_size, header_font_size, GL_UNIT_PIXEL);
  GL_Widget_SetBackAlign(print, GL_ALIGN_RIGHT);

  //BATCH NUMBER
  print = GL_Print_Create(layout);
  GL_Widget_SetText(print, "BATCH NO: %s", GetValue(dict, "BATCH_NO" + postfix).c_str());
  GL_Widget_SetItem(print, 0, line);
  GL_Widget_SetFontSize(print, header_font_size, header_font_size, GL_UNIT_PIXEL);
  GL_Widget_SetBackAlign(print, GL_ALIGN_LEFT);

  //HOST
  print = GL_Print_Create(layout);
  GL_Widget_SetText(print, "HOST: %s", GetValue(dict, "HOST" + postfix).c_str());
  GL_Widget_SetItem(print, 0, line++);
  GL_Widget_SetFontSize(print, header_font_size, header_font_size, GL_UNIT_PIXEL);
  GL_Widget_SetBackAlign(print, GL_ALIGN_RIGHT);

  //TITLE
  print = GL_Print_Create(layout);
  GL_Widget_SetText(print, "%s",GetValue(dict, "REPORT_TYPE" + postfix).c_str());
  GL_Widget_SetItem(print, 0, line++);
  GL_Widget_SetFontSize(print, 25, 25, GL_UNIT_PIXEL);
  GL_Widget_SetBackAlign(print, GL_ALIGN_CENTER);
  GL_Widget_SetFontStyle(print, GL_FONT_STYLE_BOLD);

// end of settlement header
}

}
