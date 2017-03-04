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
#include <bdor/host_test_workflow.h>

namespace bdor {

HostTestWorkfow::HostTestWorkfow(ApplicationSettings& app_settings,
                                 HostSwitch& host,
                Ui& ui)
    : app_settings_(app_settings),
      host_(host),
      ui_(ui) {

}

void HostTestWorkfow::Start() {
  // get host list
  auto host_list = app_settings_.GetAllHostDefinitions();

  int nb_error = 0;

  for (unsigned int i=0; i<host_list.size(); i++) {
    ui_.DisplayHostTestingProgress(i+1, host_list.size(), nb_error);

    HostDefinition& host = host_list[i];
    if (host.host_protocol != HostProtocol::FDMS_BASE24) {
      continue;
    }

    bool ret = host_.PreConnect(host.index);
    if (!ret) {
      host_.Disconnect();
      nb_error++;
      break;
    }

    TestTransaction test_tx;
    test_tx.tpdu = host.settlement_tpdu;
    test_tx.nii = utils::FromString<uint32_t>(host.settlement_nii);
    test_tx.tid = host.terminal_id;
    test_tx.mid = host.merchant_id;

    HostSwitch::Status status = host_.PerformFdmsTestTransaction(test_tx);
    host_.Disconnect();

    if (status != HostSwitch::Status::COMPLETED) {
      nb_error++;
    }

  }

  ui_.DisplayHostTestingResult(host_list.size(), nb_error);
}

}

