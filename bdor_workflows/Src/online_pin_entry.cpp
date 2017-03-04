/*
 ------------------------------------------------------------------------------
 INGENICO Technical Software Department
 ------------------------------------------------------------------------------
 Copyright (c) 2015 INGENICO S.A.
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
#include "online_pin_entry.h"

#include <utils/logger.h>

namespace bdor {

bool PerformOnlinePinEntry(tpcore::PinEntry& pin_entry,
                           Ui& ui,
                           Transaction* tx) {

  logger::die_unless(tx->amount.has_value(), "missing amount");

  auto pin_entry_ui = ui.GetPinEntryUI(tx->transaction_type,
                                       *tx->amount);

  int pin_length = 0;

  auto st = pin_entry.AskForPinOnline(*pin_entry_ui, 1, &pin_length);
  if (st != types::PinEntryStatus::ENTRY_SUCCESS) {
    ui.MessageProcessAborted();
    return false;
  }

  tx->pin_entered = true;
  tx->signature_required = false;

  return true;
}

}
