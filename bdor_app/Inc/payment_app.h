/*
 ------------------------------------------------------------------------------
 INGENICO Technical Software Department
 ------------------------------------------------------------------------------
 Copyright (c) 2014-2016 INGENICO S.A.
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
#ifndef BDOR__PAYMENT_APP_H_
#define BDOR__PAYMENT_APP_H_

#include <cstdio>
#include <tpcore/telium_app.h>
#include <tpcore/with_after_reset.h>
#include <tpcore/with_more_function.h>
#include <tpcore/with_message_received.h>
#include <tpcore/disk_manager.h>
#include <tpcore/goal_handle.h>
#include <emv/emv_config.h>
#include <posgate/ecr_interface.h>
#include <bdor/application_settings.h>
#include <bdor/batch.h>
#include <bdor/host_switch.h>
#include <bdor/password.h>
#include <menu_manager.h>
#include <ecr_manager.h>
#include <version.h>

namespace bdor {

class PaymentApp final:
    public tpcore::TeliumApp,
    public tpcore::WithAfterReset,
    public tpcore::WithMoreFunction,
    public tpcore::WithMessageReceived {

 public:

  PaymentApp(int app_no, int app_type);

 public:

  // virtual variant of TeliumApp methods for event interfaces.
  const char *AppName() const final;
  unsigned char AppNo() const final;

  // event specializations
  void State() final;
  bool IsDeleteAllowed() final;
  bool IsEvolAllowed() final;

  void FirstReset(bool is_remote_download) final;
  void NormalReset() final;

  void More() final;

  void Message(const S_MESSAGE_IAM& message) final;

 private:
  Version version_;
  std::unique_ptr<tpcore::DiskManager> disk_manager_;
  tpcore::GoalHandle gl_;
  emv::EmvConfig::uptr emv_config_;
  contactless::ClessConfig::uptr cless_config_;
  std::unique_ptr<tpcore::PinEntry> pin_entry_;
  std::unique_ptr<ApplicationSettings> app_setting_;
  std::unique_ptr<Ui> ui_;
  std::unique_ptr<Receipts> receipts_;
  std::unique_ptr<HostSwitch> host_;
  std::unique_ptr<Batch> batch_;
  std::unique_ptr<Acquirer> acquirer_;
  std::unique_ptr<MenuManager> menu_manager_;
  std::unique_ptr<Password> password_;
  std::unique_ptr<EcrManager> ecr_manager_;

  void InitializeApplication(bool migrate_batch);
};

}
#endif
