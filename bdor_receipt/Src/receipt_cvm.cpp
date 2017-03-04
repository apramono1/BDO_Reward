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
#include "receipt_cvm.h"

#include <sdk.h>
#include <GL_GraphicLib.h>

namespace {

const std::string font_name = "Roboto Mono";

void PrintPinOk(T_GL_HWIDGET& layout, T_GL_DIM& line) {
  T_GL_HWIDGET print = GL_Print_Create(layout);
  GL_Widget_SetText(print, "PIN VERIFIED OK\n");
  GL_Widget_SetItem(print, 0, line++);
  GL_Widget_SetFontSize(print, 25, 25, GL_UNIT_PIXEL);
  GL_Widget_SetBackAlign(print, GL_ALIGN_CENTER);
  GL_Widget_SetFontStyle(print, GL_FONT_STYLE_BOLD);
  GL_Widget_SetFontName(print, font_name.c_str());
}

void PrintSignature(T_GL_HWIDGET& layout, T_GL_DIM& line,
                    const std::string& signature_file) {
  if (signature_file.size() != 0) {
    T_GL_HWIDGET signature = GL_Signature_Create(layout);
    GL_Signature_Load(signature, signature_file.c_str());
    GL_Widget_SetBackColor(signature, GL_COLOR_WHITE);
    GL_Widget_SetForeColor(signature, GL_COLOR_BLACK);
    GL_Widget_SetBorders(signature, 0, 0, 0, 0, GL_UNIT_PIXEL,
    GL_COLOR_WHITE);
  }

  T_GL_HWIDGET print = GL_Print_Create(layout);
  GL_Widget_SetText(print, "\nSign:_________________________________");
  GL_Widget_SetItem(print, 0, line++);
  GL_Widget_SetFontSize(print, 25, 25, GL_UNIT_PIXEL);
  GL_Widget_SetBackAlign(print, GL_ALIGN_CENTER);
}

void PrintNoSignatureRequired(T_GL_HWIDGET& layout, T_GL_DIM& line) {
  T_GL_HWIDGET print = GL_Print_Create(layout);
  GL_Widget_SetText(print, "NO SIGNATURE REQUIRED\n\n");
  GL_Widget_SetItem(print, 0, line++);
  GL_Widget_SetFontSize(print, 25, 25, GL_UNIT_PIXEL);
  GL_Widget_SetBackAlign(print, GL_ALIGN_CENTER);
  GL_Widget_SetFontStyle(print, GL_FONT_STYLE_BOLD);
  GL_Widget_SetFontName(print, font_name.c_str());
}

void PrintExpressPayment(T_GL_HWIDGET& layout, T_GL_DIM& line) {
  T_GL_HWIDGET print = GL_Print_Create(layout);
  GL_Widget_SetText(print, "(EXPRESS PAYMENT)\n\n");
  GL_Widget_SetItem(print, 0, line++);
  GL_Widget_SetFontSize(print, 15, 15, GL_UNIT_PIXEL);
  GL_Widget_SetBackAlign(print, GL_ALIGN_CENTER);
  GL_Widget_SetFontStyle(print, GL_FONT_STYLE_BOLD);
  GL_Widget_SetFontName(print, font_name.c_str());
}

void PrintCustomerName(T_GL_HWIDGET& layout, T_GL_DIM& line,
                       const std::string& customer_name) {

  T_GL_HWIDGET print = GL_Print_Create(layout);

  GL_Widget_SetText(print, "%s\n", customer_name.c_str());
  GL_Widget_SetItem(print, 0, line++);
  if (customer_name.size() > 25)
    GL_Widget_SetFontSize(print, 24, 24, GL_UNIT_PIXEL);
  else
    GL_Widget_SetFontSize(print, 25, 25, GL_UNIT_PIXEL);
  GL_Widget_SetBackAlign(print, GL_ALIGN_CENTER);
  GL_Widget_SetFontStyle(print, GL_FONT_STYLE_BOLD);
  GL_Widget_SetFontName(print, font_name.c_str());
}

}
namespace bdor {

ReceiptCVM::ReceiptCVM(tpcore::GoalHandle& gl,
                       unsigned printer_timeout,
                       CvmType cvm_type,
                       CopyType copy)
    : GoalReceipt(gl, printer_timeout) {

  if ((copy == CopyType::CUSTOMER)
      &&
      (cvm_type == CvmType::SIGNATURE || cvm_type == CvmType::PIN_AND_SIGNATURE)) {
    cvm_to_print = CvmType::NO_CVM;
  }
  else
    cvm_to_print = cvm_type;
}

void ReceiptCVM::Expand(const Receipt::Map& dict) {
  T_GL_HWIDGET layout = GetGoalRootLayout();

  T_GL_DIM line = 0;
  std::string signature_file = GetValue(dict, "SIGNATURE");

  if (cvm_to_print == CvmType::PIN_AND_SIGNATURE) {
    PrintPinOk(layout, line);
    PrintSignature(layout, line, signature_file);
  }
  else if (cvm_to_print == CvmType::SIGNATURE) {
    PrintSignature(layout, line, signature_file);
  }
  else if (cvm_to_print == CvmType::PIN) {
    PrintPinOk(layout, line);
    PrintNoSignatureRequired(layout, line);
  }
  else if (cvm_to_print == CvmType::EXPRESS_PAYMENT) {
    PrintNoSignatureRequired(layout, line);
    PrintExpressPayment(layout, line);
  }
  else {
    PrintNoSignatureRequired(layout, line);
  }

  PrintCustomerName(layout, line, GetValue(dict, "NAME"));
}

}
