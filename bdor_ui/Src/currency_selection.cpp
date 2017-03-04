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

bool GoalUi::SelectCurrency(const types::Amount& base_amount,
                            const types::Amount& dcc_amount,
                            const std::string& str_dcc_rate,
                            types::Amount& selected_amount) {
  std::string str_header = "SELECT CURRENCY\nEXCHANGE RATE: " + str_dcc_rate;
  std::string base = base_amount.GetCurrencyName() + " "
      + base_amount.ToString();
  std::string dcc = dcc_amount.GetCurrencyName() + " " + dcc_amount.ToString();

  std::map<std::string, types::Amount> map_menu;
  map_menu[base] = base_amount;
  map_menu[dcc] = dcc_amount;

  std::vector<std::string> menu_content = { base, dcc };

  std::string item_selected = MenuSelection(str_header.c_str(), menu_content);

  if (item_selected.empty()) {
    return false;
  }

  selected_amount = map_menu[item_selected];

  return true;
}

}
