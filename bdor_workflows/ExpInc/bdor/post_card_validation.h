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
#ifndef BDOR__POST_CARD_VALIDATION_H_
#define BDOR__POST_CARD_VALIDATION_H_

#include <bdor/acquirer.h>
#include <bdor/ui.h>
#include <bdor/application_settings.h>
#include <bdor/batch.h>
#include <bdor/host_switch.h>
#include <bdor/emv_workflow.h>

namespace bdor {

class PostCardValidation {
 public:
  PostCardValidation(ApplicationSettings& app_setting,
           Batch& batch,
           HostSwitch& host,
           emv::EmvConfig& emv_config,
           contactless::ClessConfig& cless_config,
           Ui& ui,
           Receipts& receipts,
           Password& password,
		   Acquirer& acquirer);

  bool IsResponseApproved(const std::string& response_code);
  bool IsResponseReceived(const Transaction& tx);
  void ProcessPostCardValidation(Transaction& tx);
  void ProcessPostCardValidation(Transaction& tx, EmvWorkflow& emv_workflow);
  void ProcessValidCard(Transaction& tx);
  void ProceedToExcessVisit(Transaction& tx);
  void ProceedToWithinLimitVisit(Transaction& tx);
  bool CancelTransaction();

 private:
  ApplicationSettings& app_setting_;
  Batch& batch_;
  HostSwitch& host_;
  emv::EmvConfig& emv_config_;
  contactless::ClessConfig& cless_config_;
  Ui& ui_;
  Receipts& receipts_;
  Password& password_;
  Acquirer& acquirer_;
};
}

#endif
