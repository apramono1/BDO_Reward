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

#ifndef  OCBC_RECEIPT_TEST_MENU_H_
#define  OCBC_RECEIPT_TEST_MENU_H_

#include <tpcore/goal_handle.h>
#include <bdor/receipts.h>
#include <bdor/cvm_type.h>

class Menu {
 public:
  Menu(tpcore::GoalHandle& gl, bdor::Receipts& receipts,bdor::ApplicationSettings& app_settings)
      : gl_(gl),
        receipts_(receipts),
        app_settings_(app_settings){
  }

  void OpenMainMenu();

 private:
  tpcore::GoalHandle& gl_;
  bdor::Receipts& receipts_;
  bdor::ApplicationSettings& app_settings_;

  ulong receipt_type_;
  ulong test_case_;
  bdor::CopyType copy_;
  bool is_tip_merchant_;
  bdor::CvmType cvm_type_;
};

#endif
