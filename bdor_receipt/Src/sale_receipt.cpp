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
#include <receipt_footer.h>
#include "sale_receipt.h"

#include <stdx/memory>
#include "receipt_header.h"
#include "receipt_transaction_header.h"
#include "receipt_cvm.h"
#include "receipt_footer.h"
#include "receipt_formatter.h"
#include "receipt_utils.h"

namespace bdor {

SaleReceipt::SaleReceipt(tpcore::GoalHandle& gl,
                         unsigned printer_timeout,
                         CopyType copy,
                         ReprintType reprint,
						 TerminalConfiguration::TipProcessingMode terminal_tip_mode,
                         CvmType cvm_type)
    : GoalCompositeReceipt(gl, printer_timeout) {
  Add(stdx::make_unique<ReceiptHeader>(gl_, printer_timeout));
  Add(stdx::make_unique<ReceiptTransactionHeader>(gl_, printer_timeout));
  Add(stdx::make_unique<Body>(gl_, printer_timeout, terminal_tip_mode));
  Add(stdx::make_unique<ReceiptCVM>(gl_, printer_timeout, cvm_type, copy));
  Add(stdx::make_unique<ReceiptFooter>(gl_, printer_timeout, copy, reprint));

}

SaleReceipt::Body::Body(tpcore::GoalHandle& gl,
                        unsigned printer_timeout,
						TerminalConfiguration::TipProcessingMode terminal_tip_mode)
    : GoalReceipt(gl, printer_timeout),
      tip_mode(terminal_tip_mode) {
}

void SaleReceipt::Body::Expand(const Receipt::Map& dict) {
	T_GL_HWIDGET layout = GetGoalRootLayout();
	T_GL_DIM line = 0;

	SetPrintDataTxnTypeDebit(dict, layout, line);
	SetPrintDataTransactionDetails(dict, layout, line);

	SetPrintDataAmountsWithTip(dict, tip_mode, layout, line);
	SetPrintDataDccInformation(dict, layout, line);

	SetPrintDataHostResponseText(dict, layout, line);
	SetPrintDataLineFeed(layout, line);
}

}
