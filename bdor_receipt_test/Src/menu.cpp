/*
 ------------------------------------------------------------------------------
 INGENICO Technical Software Department
 ------------------------------------------------------------------------------
 Copyright (c) 2014 INGENICO S.A.
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
#include "menu.h"

#include <tpcore/telium_keyboard_guard.h>
#include "receipt_other_transaction_test.h"

void Menu::OpenMainMenu() {
  tpcore::TeliumKeyboardGuard keyboard_guard;
  const char *MenuUser[] = { "INSTALMENT SALE", 0 };
  const char *TestCaseMenu[] = { "CUSTOMER COPY", "MERCHANT COPY", "PIN",
      "SIGNATURE", "TIP", "CHIP", "AMEX", 0 };

  do {

    bdor::Transaction txn1;
    test_case_ = 0;
    copy_ = bdor::CopyType::MERCHANT;
    is_tip_merchant_ = false;
    cvm_type_ = bdor::CvmType::NO_CVM;

    receipt_type_ = GL_Dialog_Menu(gl_.GetHandle(), "MAIN MENU", MenuUser,
                                   receipt_type_, GL_BUTTON_VALID_CANCEL,
                                   GL_KEY_1,
                                   60 * 1000);
    switch (receipt_type_) {

      case GL_KEY_CANCEL:
        return;

      default:
        test_case_ = GL_Dialog_Menu(gl_.GetHandle(), MenuUser[receipt_type_],
                                    TestCaseMenu, test_case_,
                                    GL_BUTTON_VALID_CANCEL, GL_KEY_1,
                                    60 * 1000);
        break;
    }

    switch (test_case_) {
      case 0:
        copy_ = bdor::CopyType::CUSTOMER;
        break;

      case 1:
        break;

      case 2:
        cvm_type_ = bdor::CvmType::PIN;
        txn1.pin_entered = true;
        break;

      case 3:
        cvm_type_ = bdor::CvmType::SIGNATURE;
        txn1.signature_required = true;
        break;

      case 4:
        is_tip_merchant_ = true;
        if (receipt_type_ == 1) {
          GL_Dialog_Message(gl_.GetHandle(), TestCaseMenu[test_case_],
                            "Not Applicable!", GL_ICON_INFORMATION,
                            GL_BUTTON_NONE, 2 * GL_TIME_SECOND);
          continue;
        }
        break;

      case 5: {
        txn1.aid = types::Aid( utils::HexStringToBytes("A000000025010801"));
        txn1.pos_entry_mode = types::PosEntryMode::CHIP;
        txn1.transaction_certificate = "AABBCCDDEEFFAABB";
        txn1.application_prefered_name = "VISA CREDIT";
        //txn1.pin_entered = true;
        txn1.signature_required = true;
      }
        break;

      case 6:

        txn1.amex_se_number = "0123456789";
        txn1.card_name = "AMEX";
        txn1.aid = types::Aid( utils::HexStringToBytes("A000000025010801"));
        txn1.pos_entry_mode = types::PosEntryMode::CHIP;
        txn1.transaction_certificate = "AABBCCDDEEFFAABB";
        txn1.application_prefered_name = "AMERICAN EXPRESS";
        txn1.signature_required = true;
        break;

      case GL_KEY_CANCEL:
      case GL_RESULT_INACTIVITY:
      default:
        continue;
    }

    GL_Dialog_Message(gl_.GetHandle(), TestCaseMenu[test_case_],
                      "Printing Receipt...", GL_ICON_INFORMATION,
                      GL_BUTTON_NONE, 0);
    switch (receipt_type_) {
      case 0:
        TestPrintInstalmentSale(copy_, txn1, receipts_,
                                app_settings_);
        break;

      default:
        break;
    }
  } while (receipt_type_ != GL_KEY_CANCEL);
}
