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
#include "receipt_header.h"

#include <sdk.h>
#include <GL_GraphicLib.h>
#include "receipt_formatter.h"

namespace bdor {

ReceiptHeader::ReceiptHeader(tpcore::GoalHandle& gl,
                             unsigned printer_timeout)
    : GoalReceipt(gl, printer_timeout) {
}

void ReceiptHeader::Expand(const Receipt::Map& dict) {

  T_GL_HWIDGET layout = GetGoalRootLayout();

  T_GL_HWIDGET print;
  T_GL_DIM line = 0;

  int merchant_name_font_size = 15;
  std::string font_name = "Roboto Mono";

  //bank logo
  print = GL_Picture_Create(layout);
  GL_Widget_SetSource(print, "file://flash/HOST/BANK_LOGO.PNG");
  GL_Widget_SetItem(print, 0, line++);

  //Header line 1
  if (dict.count("MERCHANT_NAME") > 0) {
    SetPrintData(layout, GetValue(dict, "MERCHANT_NAME").c_str(), line++,
                 GL_ALIGN_CENTER,
                 merchant_name_font_size, merchant_name_font_size,
                 GL_FONT_STYLE_BOLD,
                 false, GL_DIRECTION_WIDTH, font_name);
  }
  //Header line 2
  if (dict.count("MERCHANT_ADDRESS_1") > 0) {
    SetPrintData(layout, GetValue(dict, "MERCHANT_ADDRESS_1").c_str(), line++,
                 GL_ALIGN_CENTER,
                 merchant_name_font_size, merchant_name_font_size,
                 GL_FONT_STYLE_BOLD,
                 false, GL_DIRECTION_WIDTH, font_name);
  }
  //Header line 3
  if (dict.count("MERCHANT_ADDRESS_2") > 0) {
    SetPrintData(layout, GetValue(dict, "MERCHANT_ADDRESS_2").c_str(), line++,
                 GL_ALIGN_CENTER,
                 merchant_name_font_size, merchant_name_font_size,
                 GL_FONT_STYLE_BOLD,
                 false, GL_DIRECTION_WIDTH, font_name);
  }
  //Header line 4
  if (dict.count("MERCHANT_ADDRESS_3") > 0) {
    SetPrintData(layout, GetValue(dict, "MERCHANT_ADDRESS_3").c_str(), line++,
                 GL_ALIGN_CENTER,
                 merchant_name_font_size, merchant_name_font_size,
                 GL_FONT_STYLE_BOLD,
                 false, GL_DIRECTION_WIDTH, font_name);
  }
  //Header line 5
  if (dict.count("MERCHANT_ADDRESS_4") > 0) {
    SetPrintData(layout, GetValue(dict, "MERCHANT_ADDRESS_4").c_str(), line++,
                 GL_ALIGN_CENTER,
                 merchant_name_font_size, merchant_name_font_size,
                 GL_FONT_STYLE_BOLD,
                 false, GL_DIRECTION_WIDTH, font_name);
  }
  SetPrintData(layout, "", line++, GL_ALIGN_CENTER, merchant_name_font_size,
               merchant_name_font_size);
// end of header
}

}
