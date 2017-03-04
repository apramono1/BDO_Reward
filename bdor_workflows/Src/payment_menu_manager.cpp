/*
 ------------------------------------------------------------------------------
 INGENICO Technical Software Department
 ------------------------------------------------------------------------------
 Copyright (c) 2016 INGENICO S.A.
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
#include "payment_menu_manager.h"

#include <iostream>
#include <bdor/summary_report_workflow.h>

namespace bdor {

PaymentMenuManager::PaymentMenuManager(ApplicationSettings& app_setting,
                         Ui& ui,
                         Receipts& receipts,
                         Acquirer& acquirer,
                         bdor::Batch& batch,
                         Password& password)
    : app_setting_(app_setting),
      ui_(ui),
      receipts_(receipts),
      acquirer_(acquirer),
      batch_(batch),
      password_(password) {


  main_menu_.push_back(
        std::make_pair("HOW MANY GUEST(S)?",
                       [&]() {return ui_.DispatchMenu("PAYMENT OPTION", payment_option_menu);}));

  payment_option_menu = {
    { "REVIEW              ", [&]() {return acquirer_.StartBatchReview();}},
    { "REPRINT             ", [&]() {return ui_.DispatchMenu("REPRINT", reprint_menu_);}},
    { "REPORT              ", [&]() {return ui_.DispatchMenu("REPRINT", report_menu_);}},
    { "BATCH TOTAL         ", [&]() {return acquirer_.StartBatchTotal();}},
    { "TEST HOSTS          ", [&]() {return acquirer_.StartHostTest();}}
  };

}

bool MenuManager::OpenMainMenu() {
  return ui_.DispatchMenu("LOUNGE USAGE", guest_number_menu_);
}

}
