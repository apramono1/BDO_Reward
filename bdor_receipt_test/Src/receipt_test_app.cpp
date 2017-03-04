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
#include <receipt_test_app.h>

#include <stdx/memory>
#include <tpcore/telium_printer.h>
#include <bdor/goal_receipts.h>

#include "version.h"

ReceiptTestApp::ReceiptTestApp(int app_no, int app_type)
    : TeliumApp(app_no, app_type) {

  RegisterService(IsNameService(this, kServiceHighPriority));
  RegisterService(MoreFunctionService(this, kServiceHighPriority));
  RegisterService(AfterResetService(this, kServiceHighPriority));
}

/*
 * Accessors for interfaces.
 */

const char *ReceiptTestApp::AppName() const {
  return TeliumApp::AppName();
}

unsigned char ReceiptTestApp::AppNo() const {
  return TeliumApp::AppNo();
}

/*
 * TeliumApp interface.
 */

void ReceiptTestApp::State() {
  tpcore::TeliumPrinter().Bold().PrintLn(AppName()).DefaultStyle().Print(
      "File name : ").PrintLn(FileName()).WaitPrinted();

  Version().PrintVersion();

  tpcore::TeliumPrinter().DefaultStyle().Print("CRC     : ").PrintHex4(Crc())
      .PrintLn().Print("RAM Usage : ").Print(RamSizeKb()).PrintLn("kb").Print(
      "HEAP Free : ").Print(HeapFreeKb()).PrintLn("kb").PrintLn().WaitPrinted();
}

bool ReceiptTestApp::IsDeleteAllowed() {
  return true;
}

bool ReceiptTestApp::IsEvolAllowed() {
  return true;
}

void ReceiptTestApp::FirstReset(bool is_remote_download) {
  NormalReset();
}

void ReceiptTestApp::NormalReset() {
  using stdx::make_unique;
  const std::string kFdmsPartitionName = "fdms";
  app_setting_ = make_unique<bdor::ApplicationSettings>(kFdmsPartitionName);
  receipts_ = stdx::make_unique<bdor::GoalReceipts>(gl_, 1000);
  menu_ = stdx::make_unique<Menu>(gl_, *receipts_, *app_setting_);
}

void ReceiptTestApp::More() {
  int initial_header = IsHeaderDisplayed();
  DisplayHeader(OFF);

  menu_->OpenMainMenu();

  if (initial_header == 1)
    DisplayHeader(ON);
}
