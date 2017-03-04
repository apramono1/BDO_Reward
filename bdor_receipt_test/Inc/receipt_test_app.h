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
#ifndef  RECEIPT_TEST_APP_H_
#define  RECEIPT_TEST_APP_H_

#include <memory>
#include <ctime>
#include <tpcore/telium_app.h>
#include <tpcore/with_more_function.h>
#include <tpcore/with_after_reset.h>
#include <tpcore/goal_handle.h>
#include <bdor/receipts.h>
#include "menu.h"

class ReceiptTestApp : public tpcore::TeliumApp,
    public tpcore::WithMoreFunction, public tpcore::WithAfterReset {

 public:
  ReceiptTestApp(int app_no, int app_type);

  // virtual variant of TeliumApp methods for event interfaces.
  const char *AppName() const override;
  unsigned char AppNo() const override;

  // event specializations
  void State() override;
  bool IsDeleteAllowed() override;
  bool IsEvolAllowed() override;

  void More() override;

  void FirstReset(bool is_remote_download) override;
  void NormalReset() override;

 private:
  tpcore::GoalHandle gl_;
  std::unique_ptr<bdor::Receipts> receipts_;
  std::unique_ptr<bdor::ApplicationSettings> app_setting_ ;
  std::unique_ptr<Menu> menu_;
};

#endif
