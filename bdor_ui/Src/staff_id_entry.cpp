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
#include <bdor/goal_ui.h>
#include <utils/strings.h>
#include <tpcore/telium_keyboard_guard.h>

namespace bdor {

bool GoalUi::RequestStaffId(std::string& staff_id) {

  tpcore::TeliumKeyboardGuard keyboard_guard;

  auto mask = std::string(2 * kStaffIdLength, 0);
  auto it = mask.begin();
  while (it != mask.end()) {
    *it++ = '/';
    *it++ = 'c';
  }

  staff_id.clear();
  staff_id.resize(kStaffIdLength);
  while (true)
  {
    ulong ret = GL_Dialog_Text(gl_.GetHandle(), "Staff ID",
                               "ENTER Staff ID:",
                               mask.c_str(),
                               (char*)staff_id.data(),
                               staff_id.capacity() + 1,
                               screen_timeout);

    if (ret != GL_KEY_VALID) {
      staff_id.clear();
      return false;
    }

    if (strlen(staff_id.c_str()) == kStaffIdLength) {
      break;
    }

    staff_id[0] = 0;
    GL_Dialog_Message(gl_.GetHandle(), NULL,
                      "Please Enter Correct Staff ID.",
                      GL_ICON_WARNING,
                      GL_BUTTON_NONE,
                      msg_duration_);
  }

  return true;
}

}
