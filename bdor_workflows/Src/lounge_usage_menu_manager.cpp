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
#include "lounge_usage_menu_manager.h"
#include <stdx/memory>

#include <iostream>
#include <bdor/summary_report_workflow.h>

namespace bdor {

LoungeUsageMenuManager::LoungeUsageMenuManager(ApplicationSettings& app_setting,
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

  if (app_setting_.managed_settings_->terminal_config.TxSaleEnabled())
    main_menu_.push_back(
        std::make_pair("SALE", [&]() {return acquirer_.StartSale();}));

  if (app_setting_.managed_settings_->terminal_config.TxVoidEnabled())
    main_menu_.push_back(
        std::make_pair("VOID", [&]() {return acquirer_.StartVoid();}));

  main_menu_.push_back(
        std::make_pair("BDO Reward",
                       [&]() {return acquirer_.StartReward();}));

  admin_function_menu_ = {
    { "CLEAR REVERSAL     +", [&]() {return acquirer_.StartClearReversal();}}};

  report_menu_ = {
    { "DETAIL        ", [&]() {return acquirer_.PrintDetailReport();}},
    { "SUMMARY       ", [&]() {return acquirer_.StartPrintSummaryReport();}}};

  reprint_menu_ = {
    { "LAST TRANSACTION    ", [&]() {return acquirer_.StartReprintLastTransaction();}},
    { "ANY TRANSACTION     ", [&]() {return acquirer_.StartReprintAnyTransaction();}},
    { "LAST SETTLEMENT     ", [&]() {return acquirer_.StartReprintLastSettlement();}}};

  merchant_menu = {
    { "REVIEW              ", [&]() {return acquirer_.StartBatchReview();}},
    { "REPRINT             ", [&]() {return ui_.DispatchMenu("REPRINT", reprint_menu_);}},
    { "REPORT              ", [&]() {return ui_.DispatchMenu("REPRINT", report_menu_);}},
    { "BATCH TOTAL         ", [&]() {return acquirer_.StartBatchTotal();}},
    { "TEST HOSTS          ", [&]() {return acquirer_.StartHostTest();}}
  };

}

bool LoungeUsageMenuManager::OpenLoungeUsageMenu() {
  return ui_.DispatchMenu("LOUNGE USAGE", lounge_usage_menu_);
}

}
