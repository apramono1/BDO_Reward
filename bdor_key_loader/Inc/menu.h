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

#ifndef ATOS_KEY_LOADER_MENU_H_
#define ATOS_KEY_LOADER_MENU_H_

#include <tpcore/goal_handle.h>
#include "key_injection.h"

class Menu {
 public:
  Menu(tpcore::GoalHandle& gl)
      : gl_(gl),
        key_injection_(gl_) {
  }

  void Display();

 private:
  tpcore::GoalHandle& gl_;
  KeyInjection key_injection_;
};

#endif
