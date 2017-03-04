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
#ifndef BDOR__SETTLEMENT_WORKFLOW_H_
#define BDOR__SETTLEMENT_WORKFLOW_H_

#include <bdor/application_settings.h>
#include <bdor/batch.h>
#include <bdor/host_switch.h>
#include <bdor/receipts.h>
#include <bdor/settlement_data.h>
#include <bdor/ui.h>

namespace bdor {

class SettlementWorkflow {
 public:
  SettlementWorkflow(ApplicationSettings& app_setting,
                     Batch& batch,
                     HostSwitch& host,
                     Ui& ui,
                     Receipts& receipts);

  void Start();
  bool PosStart();

  SingleHostSettlementData ProcessSettlement(
      const HostDefinition& host_config,
      Ui::SettlementOption settlement_option);

 private:
  ApplicationSettings& app_setting_;
  Batch& batch_;
  HostSwitch& host_;
  Ui& ui_;
  Receipts& receipts_;

  receipt::PrintStatus PrintDetailReport();
  void PrintSingleHostSettlementReport(
      const SingleHostSettlementData& settlement_data);
  void ComputeAndPrintAllHostsSettlementReport(
      AllHostsSettlementData& all_host_settlement_data);

  void SaveLastSettlement(const Receipts::receipt_uptr& receipt) const;

  void PerformPreAuthPurge(unsigned int host_index);
  void PerformPreAuthPurge();

};

}

#endif
