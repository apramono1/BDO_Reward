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
#ifndef BDOR__CONTSCTLESS_WORKFLOW_H_
#define BDOR__CONTSCTLESS_WORKFLOW_H_

#include <cstdio>
#include <memory>
#include <contactless/cless.h>
#include <contactless/cless_explicit.h>
#include <contactless/cless_implicit.h>
#include <contactless/paywave_kernel.h>
#include <contactless/paypass_kernel.h>

#ifndef __TELIUM3__
#include <contactless/jspeedy_kernel.h>
#endif
#include <contactless/expresspay_kernel.h>
#include <bdor/application_settings.h>
#include <bdor/transaction.h>
#include <bdor/ui.h>
#include <bdor/workflow_status.h>

namespace bdor {

class ContactlessWorkflow {
 public:
  ContactlessWorkflow(contactless::ClessConfig& cless_config,
                      ApplicationSettings& app_setting,
                      Ui& ui,
                      const contactless::Cless::TransactionType txn_type,
                      types::Amount amount);
  ~ContactlessWorkflow();

  typedef std::unique_ptr<ContactlessWorkflow> uptr;

  bool Initialize();
  bool StartDetection();

  WorkflowStatus ActivateTransaction(Transaction& tx);
  WorkflowStatus ActivateRefundTransaction(Transaction& tx);

  bool IsOnlinePinRequired();
  void CancelDetection();

 private:
  contactless::ClessConfig& cless_config_;
  contactless::ClessExplicit::uptr cless_explicit_;
  ApplicationSettings& app_setting_;
  Ui& ui_;
  types::Amount amount_;

  contactless::Cless::ClessKernel kernel_to_use_;
  contactless::PayPassKernel::uptr paypass_kernel_;
  contactless::PayWaveKernel::uptr paywave_kernel_;
  #ifndef __TELIUM3__
  contactless::JSpeedyKernel::uptr jspeedy_kernel_;
#endif
  contactless::ExpressPayKernel::uptr expresspay_kernel_;
  bool is_online_pin_required_;

  WorkflowStatus Restart(Transaction& tx);
  bool GetDetectionResult();
  bool PerformSelection();
  WorkflowStatus RunSchemeSpecificFlow(Transaction& tx);
  bool ReadTransactionData(Transaction& tx);

  WorkflowStatus RunPayPassFlow(Transaction& tx);
  void ReadPayPassApplicationData(Transaction& tx);

  WorkflowStatus RunPayWaveFlow(Transaction& tx);
  void ReadPayWaveApplicationData(Transaction& tx);
  #ifndef __TELIUM3__
  WorkflowStatus RunJSpeedyFlow(Transaction& tx);
  void ReadJSpeedyApplicationData(Transaction& tx);
#endif
  WorkflowStatus RunExpressPayFlow(Transaction& tx);
  void ReadExpressPayApplicationData(Transaction& tx);

  std::vector<unsigned int> ConstructTagsForPaywave();
  std::vector<unsigned int> ConstructTagsForPaypass();
  #ifndef __TELIUM3__
  std::vector<unsigned int> ConstructTagsForJSpeedy();
#endif
  std::vector<unsigned int> ConstructTagsForExpressPay();

  WorkflowStatus TranslateStatus(
      contactless::PayWaveKernel::Status status) const;
  WorkflowStatus TranslateStatus(
      contactless::PayPassKernel::Status status) const;
  #ifndef __TELIUM3__
  WorkflowStatus TranslateStatus(
      contactless::JSpeedyKernel::Status status) const;
#endif
  WorkflowStatus TranslateStatus(
      contactless::ExpressPayKernel::Status status) const;

};

}

#endif
