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
#include <bdor/ui.h>

#include <stdx/ctime>
#include <stdx/memory>
#include <utils/strings.h>
#include <tpcore/telium_keyboard_guard.h>
#include "pin_entry_ui.h"
#include "goal_res.h"

namespace bdor {

void Ui::SetMenuTitle(std::string title) {
  txn_type_title_ = title.c_str();
}
bool Ui::DispatchMenu(const char* title, const MenuDesc &menu_info) {

  std::vector<std::string> items;
  for (auto menu_item : menu_info)
    items.push_back(menu_item.first);

  // menu loop
  // - stay in loop until user selects CANCEL or item action requests exit
  unsigned long selection = 0;
  bool more = true;
  while (more) {
    // display menu and wait for user input
    selection = MenuSelectionIndex(title, items);

    // on CANCEL or timeout, exit from this menu and continue in caller's menu
    if (selection > menu_info.size())
      return true;

    // call action associated with the selected menu item
    // if the function returns true continue in the current menu
    txn_type_title_ = menu_info[selection].first.c_str();
    more = menu_info[selection].second();
  }

  return false;
}

std::string Ui::GetBatchName(unsigned int host_index) {
  return "";
  // TODO maybe ?
  /*  switch (group_id) {
   case GroupId::CREDIT:
   return "CREDIT";
   case GroupId::DEBIT:
   return "DEBIT";
   case GroupId::AMEX:
   return "AMEX";
   default:
   return "";
   }*/
}

std::string Ui::GetTxTypeString(TransactionType tx_type) {
  switch (tx_type) {
    case TransactionType::PREAUTH:
      return "PREAUTH";
    case TransactionType::PREAUTH_CANCELLATION:
      return "VOID PREAUTH";
    case TransactionType::PREAUTH_COMPLETION_OFFLINE:
      case TransactionType::PREAUTH_COMPLETION_ONLINE:
      return "PREAUTH COMP";
    case TransactionType::CASH_ADVANCE:
      return "CASH ADVANCE";
    case TransactionType::REFUND:
      return "REFUND";
    case TransactionType::SALE:
      default:
      return "SALE";
  }
}

}
