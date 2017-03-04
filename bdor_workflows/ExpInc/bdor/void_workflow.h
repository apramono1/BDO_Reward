/*
 ------------------------------------------------------------------------------
 INGENICO Technical Software Department
 ------------------------------------------------------------------------------
 Copyright (c) 2015-2016 INGENICO S.A.
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
#ifndef BDOR__VOID_WORKFLOW_H_
#define BDOR__VOID_WORKFLOW_H_

#include <bdor/application_settings.h>
#include <bdor/transaction.h>
#include <bdor/batch.h>
#include <bdor/ui.h>
#include <bdor/host_switch.h>
#include <bdor/receipts.h>

namespace bdor {

class VoidWorkflow {
 public:
  VoidWorkflow(ApplicationSettings& app_setting,
               Batch& batch,
               HostSwitch& host,
               Ui& ui,
               Receipts& receipts);

  void Start();

 private:
  ApplicationSettings& app_settings_;
  Batch& batch_;
  HostSwitch& host_;
  Ui& ui_;
  Receipts& receipts_;

  void RunPrintReceiptFlow(const Transaction& tx);
  bool IsTxOfflineVoidable(const Transaction& tx) const;
  bool CheckTransactionTypeAndStatus(const Transaction& tx) const;
};

}

#endif
