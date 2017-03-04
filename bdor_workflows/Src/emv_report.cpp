/*
 ------------------------------------------------------------------------------
 INGENICO Technical Software Department
 ------------------------------------------------------------------------------
 Copyright (c) 2015-2016 INGENICO S.A.
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
#include "fdms/emv_report.h"
#include <bdor/receipts.h>

namespace bdor {

void SaveEmvReportReceipt(ApplicationSetting& app_setting,
                          Transaction& tx,
                          Ui& ui, Receipts& receipts, Emvreport& emv) {
  PrintWorkflow printworkflow(app_setting, ui, tx);
  Receipts::receipt_uptr receipt_emv;

  if (tx.pos_entry_mode == types::PosEntryMode::CHIP
      || tx.pos_entry_mode == types::PosEntryMode::CONTACTLESS) {
    //receipt_emv = receipts.GetEmvReportReceipt();
    //printworkflow.PrintEmvReport(*receipt_emv, emv);
  }

}
}

