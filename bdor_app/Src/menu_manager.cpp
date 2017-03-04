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
#include "menu_manager.h"

#include <iostream>
#include <bdor/summary_report_workflow.h>

namespace bdor {

MenuManager::MenuManager(ApplicationSettings& app_setting,
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

  if (app_setting_.managed_settings_->terminal_config.TxOfflineSaleEnabled())
    main_menu_.push_back(
        std::make_pair("OFFLINE",
                       [&]() {return acquirer_.StartOfflineSale();}));

  /* if (app_setting_.managed_settings_->terminal_config.TxRefundEnabled())
    main_menu_.push_back(
        std::make_pair("REFUND", [&]() {return acquirer_.StartRefund();}));*/

  if (app_setting_.managed_settings_->terminal_config.TxVoidEnabled())
    main_menu_.push_back(
        std::make_pair("VOID", [&]() {return acquirer_.StartVoid();}));

  /* if (app_setting_.managed_settings_->terminal_config.TipProcessing()
      != bdor::TerminalConfiguration::TIP_DISABLED)
    main_menu_.push_back(
        std::make_pair("TIPS ADJUST",
                       [&]() {return acquirer_.StartTipAdjust();})); */

  /* if (app_setting_.managed_settings_->terminal_config.TxPreAuthEnabled())
    main_menu_.push_back(
        std::make_pair(
            "PRE-AUTH",
            [&]() {return ui_.DispatchMenu("PRE-AUTH", preauth_menu_);})); */

  /* if (app_setting_.managed_settings_->terminal_config.TxInstalmentSaleEnabled())
    main_menu_.push_back(
        std::make_pair(
            "INSTALMENT",
            [&]() {return ui_.DispatchMenu("INSTALMENT", instalment_menu_);})); */

  /* if (true)  // if moto enable
    main_menu_.push_back(
        std::make_pair("MOTO",
                       [&]() {return ui_.DispatchMenu("MOTO", moto_menu_);})); */

  main_menu_.push_back(
        std::make_pair("BDO Reward",
                       [&]() {return acquirer_.StartReward();}));

  /* main_menu_.push_back(
      std::make_pair("SETTLEMENT",
                     [&]() {return acquirer_.StartSettlement();})); */

  /* main_menu_.push_back(
      std::make_pair(
          "MERCHANT",
          [&]() {return ui_.DispatchMenu("MERCHANT", merchant_menu);})); */

  /* main_menu_.push_back(
      std::make_pair(
          "ADMIN",
          [&]() {return password_.PerformManagerPasswordAccess() && ui_.DispatchMenu("ADMIN", admin_function_menu_);}));*/

  // moto_menu_
  if (app_setting_.managed_settings_->terminal_config.TxMotoSaleEnabled())
    moto_menu_.push_back(
        std::make_pair("MOTO SALE", [&]() {return acquirer_.StartMOTOSale();}));
  if (true)  // moto void enable
    moto_menu_.push_back(
        std::make_pair("MOTO VOID",
                       [&]() {return acquirer_.TODOFunction();}));
  if (app_setting_.managed_settings_->terminal_config.TxMotoRefundEnabled())
    moto_menu_.push_back(
        std::make_pair("MOTO REFUND",
                       [&]() {return acquirer_.StartMOTORefund();}));
  if (true)  // moto pre-auth enable
    moto_menu_.push_back(
        std::make_pair("MOTO PRE-AUTH",
                       [&]() {return acquirer_.StartMotoPreauth();}));
  if (true)  // moto pre-auth completion enable
    moto_menu_.push_back(
        std::make_pair("MOTO PRE-AUTH COMP",
                       [&]() {return acquirer_.TODOFunction();}));
  if (true)  // moto pre-auth cancellation enable
    moto_menu_.push_back(
        std::make_pair("MOTO VOID PRE-AUTH CANCELLATION",
                       [&]() {return acquirer_.TODOFunction();}));
  if (true)  // moto pre-auth completion cancellation enable
    moto_menu_.push_back(
        std::make_pair("MOTO VOID PRE-AUTH COMP",
                       [&]() {return acquirer_.TODOFunction();}));

  // revised function
  //preauth_menu_
  if (app_setting_.managed_settings_->terminal_config.TxPreAuthEnabled())
    preauth_menu_.push_back(
        std::make_pair("PRE-AUTH", [&]() {return acquirer_.StartPreauth();}));
  if (true)
    preauth_menu_.push_back(
        std::make_pair("PRE-AUTH COMP OFFLINE",
                       [&]() {return acquirer_.StartCompletionOffline();}));
  if (true)
    preauth_menu_.push_back(
        std::make_pair("PRE-AUTH COMP",
                       [&]() {return acquirer_.StartCompletionOnline();}));
  if (true)
    preauth_menu_.push_back(
        std::make_pair("VOID PRE-AUTH",
                       [&]() {return acquirer_.StartVoidPreAuth();}));

  // instalment_menu_
  if (true)  // instalment sale enable
    instalment_menu_.push_back(
        std::make_pair("INSTALMENT SALE",
                       [&]() {return acquirer_.StartInstalmentSale();}));

//Should be enabled only if fdms requires otherwise use void for installment cancellation
  if (true)  // instalment void enable
    instalment_menu_.push_back(
        std::make_pair("INSTALMENT CANCELLATION",
                       [&]() {return acquirer_.StartVoid();}));

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

bool MenuManager::OpenMainMenu() {
  return ui_.DispatchMenu("MAIN", main_menu_);
}

}
