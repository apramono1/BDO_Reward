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

#include <cstdio>
#include <vector>
#include <sdk.h>
#include <tpcore/telium_keyboard_guard.h>

#include "key_injection.h"
#include "menu.h"

#define __OEM_VFS_DEF_H_INCLUDED__

void Menu::Display() {
  tpcore::TeliumKeyboardGuard keyboard_guard;
  ulong ret = 0;

  while (true) {
    char title[] = "ATOS Mockup Key Loader";
    char item_inject_tcmk[] = "Inject TMK";
    char item_clear_secret_area[] = "Clear Secret Area";

    std::vector<const char*> menu_list;
    menu_list.push_back(item_inject_tcmk);
    menu_list.push_back(item_clear_secret_area);
    menu_list.push_back((const char*) (0));
    ret = GL_Dialog_Menu(gl_.GetHandle(), title, menu_list.data(), 0,
                         GL_BUTTON_DEFAULT, GL_KEY_0, GL_TIME_INFINITE);
    switch (ret) {
      case 0:
        key_injection_.InjectTmk();
        break;
      case 1:
        key_injection_.ClearSecretArea();
        break;
      case GL_KEY_CANCEL:
        default:
        return;
    }
  }
}
