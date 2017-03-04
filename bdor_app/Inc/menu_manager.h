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
#ifndef BDOR__MENU_MANAGER_H_
#define BDOR__MENU_MANAGER_H_

#include <memory>
#include <bdor/ui.h>
#include <bdor/receipts.h>
#include <bdor/password.h>
#include <types/password_access_level.h>
#include "acquirer.h"

namespace bdor {

class MenuManager {

 public:
  MenuManager(ApplicationSettings& app_setting,
                          Ui& ui,
                          Receipts& receipts,
                          Acquirer& acquirer,
                          Batch& batch,
                          Password& password);

  bool OpenMainMenu();

 private:
  MenuDesc main_menu_;
  MenuDesc preauth_menu_;
  MenuDesc report_menu_;
  MenuDesc reprint_menu_;
  MenuDesc instalment_menu_;
  MenuDesc moto_menu_;
  MenuDesc admin_function_menu_;
  MenuDesc merchant_menu;

  ApplicationSettings& app_setting_;
  Ui& ui_;
  Receipts& receipts_;
  Acquirer& acquirer_;
  Batch& batch_;
  Password& password_;

};

}
#endif

