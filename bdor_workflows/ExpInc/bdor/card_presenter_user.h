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
#ifndef BDOR__CARD_PRESENTER_USER_H_
#define BDOR__CARD_PRESENTER_USER_H_

#include <contactless/cless_config.h>
#include <bdor/transaction.h>
#include <bdor/contactless_workflow.h>
#include <bdor/workflow_status.h>

namespace bdor {

class CardPresenterUser {
 public:
  virtual ~CardPresenterUser() {
  }
  virtual WorkflowStatus RunManualFlow(Transaction& tx) = 0;
  virtual WorkflowStatus RunMagFlow(types::MagStripe, Transaction& tx) = 0;
  virtual WorkflowStatus RunEmvFlow(Transaction& tx) = 0;
  virtual WorkflowStatus RunClessFlow(ContactlessWorkflow& cless_workflow,
                                       Transaction& tx) = 0;
};

}

#endif
