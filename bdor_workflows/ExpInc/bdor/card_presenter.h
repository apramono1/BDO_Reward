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
#ifndef BDOR__CARD_PRESENTER_H_
#define BDOR__CARD_PRESENTER_H_

#include <cstdio>
#include <contactless/cless_config.h>
#include <tpcore/telium_event.h>
#include <bdor/application_settings.h>
#include <bdor/transaction.h>
#include <bdor/ui.h>
#include <bdor/contactless_workflow.h>
#include <bdor/card_presenter_user.h>

namespace bdor {

enum class FallBackType {
  EMV_FALLBACK,
  CLESS_FALLBACK,
  NONE
};

class CardPresenter {
 public:
  CardPresenter(ApplicationSettings& app_settings,
                    contactless::ClessConfig& cless_config,
                    Ui& ui);

  bool PromptCardToStart(CardPresenterUser* wf, Transaction& tx);
  bool PromptManualEntryForMoto(CardPresenterUser* wf,
                         Transaction& tx);
 private:

  std::vector<tpcore::TeliumEvent::EventType> GetEvents(
      const Transaction& tx, const FallBackType fall_back_type) const;

  types::Amount GetTotalAuthoriseAmount(const Transaction& tx) const;

  void CancelOtherDetection(const Transaction& tx) const;

  void MessagePresentCard(const Transaction& tx,
                          const FallBackType fall_back_type) const;

  ApplicationSettings& app_settings_;
  contactless::ClessConfig& cless_config_;
  Ui& ui_;
  ContactlessWorkflow::uptr cless_workflow_;
};

}

#endif
