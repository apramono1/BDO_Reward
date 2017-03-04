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
#include "receipt_formatter.h"

namespace bdor {
void SetPrintNormalLine(T_GL_HWIDGET layout, const std::string text, T_GL_DIM line,
                     const T_GL_ALIGN align) {
  const int font_size = 15;
  const std::string font_name = "Roboto Mono";
  T_GL_HWIDGET print;

  print = GL_Print_Create(layout);
  GL_Widget_SetText(print, text.c_str());
  GL_Widget_SetItem(print, 0, line);
  GL_Widget_SetFontSize(print, font_size, font_size, GL_UNIT_PIXEL);
  GL_Widget_SetBackAlign(print, align);
  GL_Widget_SetFontName(print, font_name.c_str());
  GL_Widget_SetFontStyle(print, GL_FONT_STYLE_BOLD);
}

void SetPrintTitleLine(T_GL_HWIDGET layout, const std::string text,
                    T_GL_DIM line) {
  const int font_size = 25;
  const std::string font_name = "Roboto Mono";
  T_GL_HWIDGET print;

  print = GL_Print_Create(layout);
  GL_Widget_SetText(print, text.c_str());
  GL_Widget_SetItem(print, 0, line);
  GL_Widget_SetFontSize(print, font_size, font_size, GL_UNIT_PIXEL);
  GL_Widget_SetBackAlign(print, GL_ALIGN_CENTER);
  GL_Widget_SetFontName(print, font_name.c_str());
  GL_Widget_SetFontStyle(print, GL_FONT_STYLE_BOLD);
}

void SetPrintData(T_GL_HWIDGET layout, const char* text, T_GL_DIM line_num,
                  T_GL_ALIGN line_align,
                  T_GL_DIM font_width,
                  T_GL_DIM font_height,
                  T_GL_FONT_STYLE font_style, bool to_reverse,
                  T_GL_DIRECTION widget_set,
                  std::string font_name,
                  T_GL_UNIT font_unit) {

  T_GL_HWIDGET print;

  print = GL_Print_Create(layout);
  if (!font_name.empty()) {
    GL_Widget_SetFontName(print, font_name.c_str());
  }
  if (to_reverse) {
    GL_Widget_SetBackColor(print, GL_COLOR_BLACK);
    GL_Widget_SetForeColor(print, GL_COLOR_WHITE);
    GL_Widget_SetExpand(print, widget_set);
  }

  GL_Widget_SetText(print, text);
  GL_Widget_SetItem(print, 0, line_num);
  GL_Widget_SetFontSize(print, font_width, font_width, font_unit);
  GL_Widget_SetFontStyle(print, font_style);
  GL_Widget_SetBackAlign(print, line_align);
}

std::string GetAmountPrintingFormat(std::string currency, std::string amount,
                                    int max_print_length, char padding_char)
                                    {
  std::string result;
  int total_length = currency.length() + amount.length();
  if (max_print_length > total_length)
    result = currency.append(max_print_length - total_length, padding_char)
        + amount;
  else
    result = currency + amount;
  return result;
}

std::string GetAmountInputPrintingFormat(std::string currency,
                                         int separater_size,
                                         char separater_char,
                                         int max_print_length,
                                         char padding_char)
                                         {
  std::string result;
  int total_length = currency.length() + separater_size;
  if (max_print_length > total_length)
    result = currency.append(separater_size, separater_char).append(
        max_print_length - total_length, padding_char);
  else
    // not likely to happen
    result = currency.append(separater_size, separater_char);
  return result;
}

}
