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
#include <receipt/goal_receipt.h>
#ifndef BDOR__RECEIPT_FORMATTER_H_
#define BDOR__RECEIPT_FORMATTER_H_

namespace bdor {
void SetPrintNormalLine(T_GL_HWIDGET layout, const std::string text, T_GL_DIM line,
                     const T_GL_ALIGN align);
void SetPrintTitleLine(T_GL_HWIDGET layout, const std::string text,
                    T_GL_DIM line);
void SetPrintData(T_GL_HWIDGET layout, const char* text, T_GL_DIM line_num,
                  T_GL_ALIGN line_align, T_GL_DIM width = 25, T_GL_DIM height = 25,
                  T_GL_FONT_STYLE font_style = GL_FONT_STYLE_BOLD,
                  bool to_reverse = false,
                  T_GL_DIRECTION widget_set = GL_DIRECTION_WIDTH,
                  std::string font_name = std::string(),
                  T_GL_UNIT font_unit = GL_UNIT_PIXEL);
std::string GetAmountPrintingFormat(std::string currency, std::string amount, int max_print_length, char padding_char =' ');
std::string GetAmountInputPrintingFormat(std::string currency, int separater_size, char separater_char , int max_print_length, char padding_char = '_');

}

#endif

