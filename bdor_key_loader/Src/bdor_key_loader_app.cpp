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
#include <ctime>
#include "bdor_key_loader_app.h"

extern "C" {
#include "SEC_interface.h"
}

#include <tpcore/telium_printer.h>
#include <version.h>

FdmsKeyLoaderApp::FdmsKeyLoaderApp(int app_no, int app_type)
    : TeliumApp(app_no, app_type),
      menu_(gl_) {
  RegisterService(IsNameService(this, kServiceHighPriority));
  RegisterService(MoreFunctionService(this, kServiceHighPriority));
  RegisterService(AfterResetService(this, kServiceHighPriority));
}

/*
 * Accessors for interfaces.
 */

const char *FdmsKeyLoaderApp::AppName() const {
  return TeliumApp::AppName();
}

unsigned char FdmsKeyLoaderApp::AppNo() const {
  return TeliumApp::AppNo();
}

/*
 * TeliumApp interface.
 */

void FdmsKeyLoaderApp::State() {
  tpcore::TeliumPrinter().
      Bold().PrintLn(AppName()).
      DefaultStyle().
      Print("File name : ").PrintLn(FileName()).
      WaitPrinted();

  Version().PrintVersion();

  tpcore::TeliumPrinter().
      DefaultStyle().
      Print("CRC     : ").PrintHex4(Crc()).PrintLn().
      Print("RAM Usage : ").Print(RamSizeKb()).PrintLn("kb").
      Print("HEAP Free : ").Print(HeapFreeKb()).PrintLn("kb").
      PrintLn().
      WaitPrinted();
}

bool FdmsKeyLoaderApp::IsDeleteAllowed() {
  return true;
}

bool FdmsKeyLoaderApp::IsEvolAllowed() {
  return true;
}

void FdmsKeyLoaderApp::FirstReset(bool is_remote_download) {
  NormalReset();
}

void FdmsKeyLoaderApp::NormalReset() {
  SEClib_Open();  // initialize Security DLL
}

void FdmsKeyLoaderApp::More() {
  menu_.Display();
}
