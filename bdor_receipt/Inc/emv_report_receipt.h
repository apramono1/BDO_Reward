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
#ifndef BDOR__EMV_REPORT_RECEIPT_H_
#define BDOR__EMV_REPORT_RECEIPT_H_

#include <receipt/goal_composite_receipt.h>

namespace bdor {

class EmvReportReceipt : public receipt::GoalCompositeReceipt {

  class Body : public receipt::GoalReceipt {
   public:
    Body(tpcore::GoalHandle& gl, unsigned printer_timeout);
    void Expand(const Receipt::Map& dict) override;

    const int kMaxLineLength = 54;

   private:

    int PrintTagLengthValue(const receipt::Receipt::Map& dict,
                            T_GL_HWIDGET layout,
                            T_GL_DIM line,
                            const char* name,
                            const char* tag);

    std::string FormatTagValue(const std::string& data);

  };

 public:
  EmvReportReceipt(tpcore::GoalHandle& gl, unsigned printer_timeout);

};

}

#endif
