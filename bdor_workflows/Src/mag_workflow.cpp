/*
 ------------------------------------------------------------------------------
 INGENICO Technical Software Department
 ------------------------------------------------------------------------------
 Copyright (c) 2015 INGENICO S.A.
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
#include <bdor/mag_workflow.h>

#include <types/pan.h>
#include <utils/strings.h>

namespace bdor {

MagWorkflow::MagWorkflow(Ui& ui, ApplicationSettings& app_setting)
    : ui_(ui),
      app_setting_(app_setting) {
}

MagWorkflow::~MagWorkflow() {
}

bool MagWorkflow::PerformCardValidation(const types::MagStripe& mag_stripe) {
  return mag_stripe.ValidateMagCard();
}

void MagWorkflow::ReadTransactionData(const types::MagStripe& mag_stripe, Transaction* tx) {
  tx->card_holder_name = mag_stripe.GetCardHolderName();
  tx->expiration_date = mag_stripe.GetExpiryDate();
  tx->service_code = mag_stripe.GetServiceCode();
  tx->pan = mag_stripe.GetPan();
  tx->track1_data = mag_stripe.GetIsoTrack1();
  tx->track2_equivalent_data = mag_stripe.GetTrack2EquivalentData();
  tx->signature_required = true;
}

}

