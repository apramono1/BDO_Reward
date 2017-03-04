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
#include "../Inc/receipt_footer.h"

#include <sdk.h>
#include <GL_GraphicLib.h>
#include <receipt_formatter.h>
#include "receipt_utils.h"

namespace {

using namespace receipt;
using namespace bdor;

const int disclaimer_font_size = 15;
const std::string font_name = "Roboto Mono";

void SetPrintDataCopyType(T_GL_HWIDGET layout,
                          T_GL_DIM& line,
                          CopyType copy) {
  std::string copy_text = "** MERCHANT COPY **\n";
  if (copy == CopyType::CUSTOMER)
    copy_text = "** CUSTOMER COPY **\n";

  SetPrintData(layout, copy_text.c_str(), line++, GL_ALIGN_CENTER,
               disclaimer_font_size,
               disclaimer_font_size,
               GL_FONT_STYLE_BOLD, false, GL_DIRECTION_WIDTH, font_name);
}

void SetPrintDataReprintType(T_GL_HWIDGET layout,
                          T_GL_DIM& line,
                          ReprintType reprint) {
  if (reprint == ReprintType::DUPLICATE) {
    SetPrintData(layout, "** DUPLICATE **\n", line++, GL_ALIGN_CENTER,
                 disclaimer_font_size,
                 disclaimer_font_size,
                 GL_FONT_STYLE_BOLD, false, GL_DIRECTION_WIDTH, font_name);
  }
}

void SetPrintDataVersion(const Receipt::Map& dict,
                              T_GL_HWIDGET layout,
                              T_GL_DIM& line) {
  std::string buffer = Receipt::GetValue(dict, "VERSION") + "\n\n\n\n\n\n\n\n";
  SetPrintData(layout, buffer.c_str(), line++, GL_ALIGN_CENTER,
               disclaimer_font_size, disclaimer_font_size, GL_FONT_STYLE_BOLD,
               false, GL_DIRECTION_WIDTH, font_name);
}

void SetPrintDataFooter(const std::string& footer_name,
                        const Receipt::Map& dict,
                              T_GL_HWIDGET layout,
                              T_GL_DIM& line) {
  std::string text = Receipt::GetValue(dict, footer_name).c_str();
   if (!text.empty())
   {
     SetPrintData(layout, text.c_str(), line++, GL_ALIGN_CENTER,
                  disclaimer_font_size,
                  disclaimer_font_size,
                  GL_FONT_STYLE_BOLD, false, GL_DIRECTION_WIDTH, font_name);
   }
}
}

namespace bdor {

ReceiptFooter::ReceiptFooter(tpcore::GoalHandle& gl,
                             unsigned printer_timeout,
                             CopyType copy,
                             ReprintType reprint)
    : GoalReceipt(gl, printer_timeout),
      copy_(copy),
      reprint_(reprint) {
}

void ReceiptFooter::Expand(const Receipt::Map& dict) {

  T_GL_HWIDGET layout = GetGoalRootLayout();

  T_GL_DIM line = 0;

  SetPrintDataLineFeed(layout, line);

  SetPrintDataFooter("FOOTER_1", dict, layout, line);
  SetPrintDataFooter("FOOTER_2", dict, layout, line);
  SetPrintDataFooter("FOOTER_3", dict, layout, line);
  SetPrintDataFooter("FOOTER_4", dict, layout, line);
  SetPrintDataFooter("FOOTER_5", dict, layout, line);
  SetPrintDataFooter("FOOTER_6", dict, layout, line);
  SetPrintDataFooter("FOOTER_7", dict, layout, line);
  SetPrintDataFooter("FOOTER_8", dict, layout, line);
  SetPrintDataFooter("FOOTER_9", dict, layout, line);
  SetPrintDataFooter("FOOTER_10", dict, layout, line);
  SetPrintDataFooter("FOOTER_11", dict, layout, line);
  SetPrintDataFooter("FOOTER_12", dict, layout, line);
  SetPrintDataFooter("FOOTER_13", dict, layout, line);
  SetPrintDataFooter("FOOTER_14", dict, layout, line);
  SetPrintDataFooter("FOOTER_15", dict, layout, line);
  SetPrintDataFooter("FOOTER_16", dict, layout, line);
  SetPrintDataFooter("FOOTER_17", dict, layout, line);
  SetPrintDataFooter("FOOTER_18", dict, layout, line);
  SetPrintDataFooter("FOOTER_19", dict, layout, line);
  SetPrintDataFooter("FOOTER_20", dict, layout, line);

  SetPrintDataLineFeed(layout, line);

  SetPrintDataCopyType(layout, line, copy_);
  SetPrintDataReprintType(layout, line, reprint_);
  SetPrintDataVersion(dict, layout, line);
}

}

