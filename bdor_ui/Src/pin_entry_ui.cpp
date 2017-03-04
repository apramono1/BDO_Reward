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
#include "pin_entry_ui.h"

#include "goal_res.h"

namespace bdor {

PinEntryUI::PinEntryUI(tpcore::GoalHandle& gl,
                       const std::string& tx_type,
                       const types::Amount& amount)
    : gl_(gl),
      tx_type_(tx_type),
      amount_(amount),
      pin_length_(0),
      pin_attempts_remaining_(0) {
}

std::string PinEntryUI::GetPinMask() {

  return std::string(pin_length_, '*');
}

void PinEntryUI::DisplayPinDialogOnTerminal() {
	std::string amount_display;
  T_GL_HWIDGET window = GL_GraphicLib_ReadResource(
      gl_.GetHandle(), GetDAL(RES_PIN_ENTRY__SCREEN).c_str());
  T_GL_HWIDGET label_tx_type = GL_Widget_SearchId(window, PIN_ENTRY__TX_TYPE);
  T_GL_HWIDGET label_amount = GL_Widget_SearchId(window, PIN_ENTRY__AMOUNT);
  T_GL_HWIDGET label_main_message = GL_Widget_SearchId(window,
                                                       PIN_ENTRY__MAIN_MESSAGE);
  T_GL_HWIDGET label_mask = GL_Widget_SearchId(window, PIN_ENTRY__MASK);
  T_GL_HWIDGET label_help_message = GL_Widget_SearchId(window,
                                                       PIN_ENTRY__HELP_MESSAGE);

  GL_Widget_SetText(label_tx_type, tx_type_.c_str());
  amount_display = amount_.GetCurrencyName() + amount_.ToString();
  GL_Widget_SetText(label_amount, amount_display.c_str());
  GL_Widget_SetText(label_main_message, "请输入密码");
  GL_Widget_SetText(label_mask, GetPinMask().c_str());

  if (pin_attempts_remaining_ == 1)
    GL_Widget_SetText(label_help_message, "Last Attempt");
  else
    GL_Widget_SetText(label_help_message, " ");

  GL_Window_Dispatch(window, 10);

  GL_Widget_Destroy(window);
}

void PinEntryUI::UpdatePinLength(int pin_length, bool auto_refresh) {
  pin_length_ = pin_length;
  if (auto_refresh)
    DisplayPinDialogOnTerminal();
}

void PinEntryUI::UpdatePinRemainingAttempts(int pin_remaining_attempts,
                                            bool auto_refresh) {
  pin_attempts_remaining_ = pin_remaining_attempts;
  if (auto_refresh)
    DisplayPinDialogOnTerminal();
}

}

