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
#include "receipt_transaction_header.h"

#include <sdk.h>
#include <GL_GraphicLib.h>

namespace bdor {

ReceiptTransactionHeader::ReceiptTransactionHeader(tpcore::GoalHandle& gl,
                                                   unsigned printer_timeout,
                                                   bool is_reversal)
    : GoalReceipt(gl, printer_timeout),
      is_reversal_(is_reversal) {
}

void ReceiptTransactionHeader::Expand(const Receipt::Map& dict) {

  T_GL_HWIDGET layout = GetGoalRootLayout();

  T_GL_HWIDGET print;
  T_GL_DIM line = 0;

  int header_font_size = 15;

	std::string text;

	int kRightPadding = 18;
	char padding_char = ' ';

	char font_name[20] = "Roboto Mono";

  //DATE and TIME
  print = GL_Print_Create(layout);
  GL_Widget_SetText(print, "DATE: %s", GetValue(dict, "DATE").c_str());
  GL_Widget_SetItem(print, 0, line);
  GL_Widget_SetFontSize(print, header_font_size, header_font_size, GL_UNIT_PIXEL);
  GL_Widget_SetBackAlign(print, GL_ALIGN_LEFT);
  GL_Widget_SetFontName(print, font_name);
  GL_Widget_SetFontStyle(print, GL_FONT_STYLE_BOLD);

  print = GL_Print_Create(layout);
  text = "TIME: " + GetValue(dict, "TIME");
  text.append(kRightPadding - text.size(), padding_char);
  GL_Widget_SetText(print, text.c_str());
  GL_Widget_SetItem(print, 0, line++);
  GL_Widget_SetFontSize(print, header_font_size, header_font_size, GL_UNIT_PIXEL);
  GL_Widget_SetBackAlign(print, GL_ALIGN_RIGHT);
  GL_Widget_SetFontName(print, font_name);
  GL_Widget_SetFontStyle(print, GL_FONT_STYLE_BOLD);

  if (is_reversal_ == true) {
    //AMOUNT
    print = GL_Print_Create(layout);
    GL_Widget_SetText(print, "SALE AMT  ");
    GL_Widget_SetItem(print, 0, line);
    GL_Widget_SetFontSize(print, 35, 35, GL_UNIT_PIXEL);
    GL_Widget_SetBackAlign(print, GL_ALIGN_LEFT);

    print = GL_Print_Create(layout);
    GL_Widget_SetText(print, "%s", GetValue(dict, "AMOUNT").c_str());
    GL_Widget_SetItem(print, 0, line++);
    GL_Widget_SetFontSize(print, 35, 35, GL_UNIT_PIXEL);
    GL_Widget_SetBackAlign(print, GL_ALIGN_RIGHT);
  }

  //BATCH NUMBER and INVOICE
  print = GL_Print_Create(layout);
  GL_Widget_SetText(print, "BATCH NO: %s", GetValue(dict, "BATCH_NO").c_str());
  GL_Widget_SetItem(print, 0, line);
  GL_Widget_SetFontSize(print, header_font_size, header_font_size, GL_UNIT_PIXEL);
  GL_Widget_SetBackAlign(print, GL_ALIGN_LEFT);
  GL_Widget_SetFontName(print, font_name);
  GL_Widget_SetFontStyle(print, GL_FONT_STYLE_BOLD);

  print = GL_Print_Create(layout);
  text = "INVOICE NO: " + GetValue(dict, "INVOICE_NO");
  text.append(kRightPadding - text.size(), padding_char);
  GL_Widget_SetText(print, text.c_str());
  GL_Widget_SetItem(print, 0, line++);
  GL_Widget_SetFontSize(print, header_font_size, header_font_size, GL_UNIT_PIXEL);
  GL_Widget_SetBackAlign(print, GL_ALIGN_RIGHT);
  GL_Widget_SetFontName(print, font_name);
  GL_Widget_SetFontStyle(print, GL_FONT_STYLE_BOLD);

  //TID and MID
  print = GL_Print_Create(layout);
  GL_Widget_SetText(print, "MID: %s", GetValue(dict, "MID").c_str());
  GL_Widget_SetItem(print, 0, line);
  GL_Widget_SetFontSize(print, header_font_size, header_font_size, GL_UNIT_PIXEL);
  GL_Widget_SetBackAlign(print, GL_ALIGN_LEFT);
  GL_Widget_SetFontName(print, font_name);
  GL_Widget_SetFontStyle(print, GL_FONT_STYLE_BOLD);

  print = GL_Print_Create(layout);
  text = "TID: " + GetValue(dict, "TID");
  text.append(kRightPadding - text.size(), padding_char);
  GL_Widget_SetText(print, text.c_str());
  GL_Widget_SetItem(print, 0, line++);
  GL_Widget_SetFontSize(print, header_font_size, header_font_size, GL_UNIT_PIXEL);
  GL_Widget_SetBackAlign(print, GL_ALIGN_RIGHT);
  GL_Widget_SetFontName(print, font_name);
  GL_Widget_SetFontStyle(print, GL_FONT_STYLE_BOLD);

}

}

