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
#include <bdor/reprint_last_settlement_workflow.h>
#include <tpcore/goal_handle.h>
#include "workflow_activities.h"

namespace bdor {

ReprintLastSettlementWorkflow::ReprintLastSettlementWorkflow(
    ApplicationSettings& app_setting, Ui& ui,
    Receipts& receipts)
    : app_setting_(app_setting),
      ui_(ui),
      receipts_(receipts) {
}

bool ReprintLastSettlementWorkflow::Start()
{
  tpcore::GoalHandle gl;
  receipt::Receipt::uptr recovered_receipt = nullptr;

  if (CheckFileExist(app_setting_.GetSettlementReceiptBmpPath())) {
    recovered_receipt = receipts_.LoadImageFile(
        app_setting_.GetSettlementReceiptBmpPath().c_str());
  }

  if (recovered_receipt) {
    PrintDuplicateHeader();
    PrintLoop(ui_, *recovered_receipt);
  }
  else {
    ui_.MessageNoLastSettlementRecord();
  }
  return true;
}

void ReprintLastSettlementWorkflow::PrintDuplicateHeader() {
  const std::map<std::string, std::string> dict;

  auto dup_header = receipts_.GetDuplicateReceiptHeader();
  dup_header->Expand(dict);
  PrintLoop(ui_, *dup_header);
}

}
