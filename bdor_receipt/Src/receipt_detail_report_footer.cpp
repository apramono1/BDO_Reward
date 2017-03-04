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
#include "receipt_detail_report_footer.h"

#include <sdk.h>
#include <GL_GraphicLib.h>
#include <receipt_formatter.h>

namespace bdor {

ReceiptDetailReportFooter::ReceiptDetailReportFooter(tpcore::GoalHandle& gl,
                                                     unsigned printer_timeout)
    : GoalReceipt(gl, printer_timeout) {
}

void ReceiptDetailReportFooter::Expand(const Receipt::Map& dict) {

  T_GL_HWIDGET layout = GetGoalRootLayout();

  T_GL_DIM line = 0;

  SetPrintNormalLine(layout, "END OF REPORT", line, GL_ALIGN_CENTER);
}

}

