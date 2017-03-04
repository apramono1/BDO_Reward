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

#include <stdx/memory>
#include <stdx/string>
#include <receipt_header.h>
#include <receipt_formatter.h>
#include <detail_report_receipt.h>
#include <receipt_detail_transaction.h>
#include <receipt_detail_report_header.h>
#include <receipt_detail_report_footer.h>
#include "receipt_settlement_version.h"
#include <receipt_detail_total.h>

namespace bdor {

DetailReportReceipt::DetailReportReceipt(tpcore::GoalHandle& gl,
                                         unsigned printer_timeout,
                                         int transaction_number,
                                         int card_type_number)
    :
      GoalCompositeReceipt(gl, printer_timeout) {

  Add(stdx::make_unique<ReceiptHeader>(gl_, printer_timeout));
  Add(stdx::make_unique<ReceiptDetailReportHeader>(gl_, printer_timeout));
  for (int i = 0; i < transaction_number; i++) {
    Add(
        stdx::make_unique<ReceiptDetailTransaction>(gl_,
                                                    printer_timeout,
                                                    i));
  }

  Add(stdx::make_unique<ReceiptDetailTotal>(gl_, printer_timeout,
                                            card_type_number));
  Add(stdx::make_unique<ReceiptDetailReportFooter>(gl_, printer_timeout));
  Add(stdx::make_unique<ReceiptSettlementVersion>(gl_, printer_timeout));
}

}
