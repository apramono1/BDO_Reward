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
#ifndef BDOR__PIN_ENTRY_UI_H_
#define BDOR__PIN_ENTRY_UI_H_

#include <string>
#include <types/amount.h>
#include <tpcore/pin_entry_ui.h>
#include <tpcore/goal_handle.h>
#include <bdor/transaction_type.h>

namespace bdor {

class PinEntryUI : public tpcore::PinEntryUI {

 public:

  PinEntryUI(tpcore::GoalHandle& gl,
             const std::string& tx_type,
             const types::Amount& amount);

  virtual void DisplayPinDialogOnTerminal();
  virtual void UpdatePinLength(int pin_length, bool auto_refresh = true);
  virtual void UpdatePinRemainingAttempts(int pin_remaining_attempts,
                                          bool auto_refresh = true);

 private:
  tpcore::GoalHandle& gl_;
  std::string tx_type_;
  types::Amount amount_;
  int pin_length_;
  int pin_attempts_remaining_;

  std::string GetPinMask();

};

}

#endif
