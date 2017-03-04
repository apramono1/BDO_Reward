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
#ifndef BDOR__ACQUIRER_H_
#define BDOR__ACQUIRER_H_

#include <tpcore/pin_entry.h>
#include <emv/emv_config.h>
#include <contactless/cless_config.h>
#include <bdor/receipts.h>
#include <bdor/batch.h>
#include <bdor/host_switch.h>
#include <bdor/lounge_usage.h>
#include <bdor/ui.h>
#include <bdor/receipts.h>
#include <bdor/password.h>

namespace bdor {

class Acquirer {
 public:
  Acquirer(ApplicationSettings& app_setting,
           Batch& batch,
           HostSwitch& host,
		   Acquirer& acquirer,
           emv::EmvConfig& emv_config,
           contactless::ClessConfig& cless_config,
           tpcore::PinEntry& pin_entry,
           Ui& ui,
           Receipts& receipts,
           Password& password);

  bool StartCash();
  bool StartSale();
  bool StartPreauth();
  bool StartCompletionOnline();
  bool StartCompletionOffline();
  bool StartMotoPreauth();
  bool StartVoid();
  bool StartVoidPreAuth();
  bool StartTipAdjust();
  bool StartSettlement(bool pos_trigger = false);
  bool StartRefund();
  bool StartReward();
  bool StartRedemption();
  bool StartOfflineSale();
  bool StartMOTOSale();
  bool StartInstalmentSale();
  bool StartReprintLastTransaction(bool pos_trigger = false);

  bool StartClearReversal();
  bool CreatDummyReversal();
  bool StartMOTORefund();
  bool StartHostTest();

  bool StartClearBatch();
  bool StartKeyExchange();
  bool StartReprintLastSettlement();
  bool StartReprintAnyTransaction();
  bool StartPrintSummaryReport();
  bool PrintDetailReport();
  bool StartBatchReview();
  bool StartBatchTotal();
  std::unique_ptr<Transaction> StartSale(const types::Amount& amount);
  std::unique_ptr<Transaction> StartRefund(const types::Amount& amount);
  std::unique_ptr<Transaction> StartPreauth(const types::Amount& amount);
  bool TODOFunction();

 private:
  ApplicationSettings& app_setting_;
  Batch& batch_;
  HostSwitch& host_;
  Acquirer& acquirer_;
  emv::EmvConfig& emv_config_;
  contactless::ClessConfig& cless_config_;
  tpcore::PinEntry& pin_entry_;
  Ui& ui_;
  Receipts& receipts_;
  Password& password_;
};

}

#endif
