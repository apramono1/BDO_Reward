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

#include <contactless_config.h>
#include <map>
#include <types/capk.h>
#include <EntryPoint_Tags.h>
#include <Cless_Generic_Tags.h>
#include <EmvLib_Tags.h>
#include <Common_Kernels_tags.h>
#include <paypass_Tags.h>
#include <Keys.h>

namespace bdor {

contactless::ClessTerminalApplication GetPaypassMasterCardApp() {
  const types::Aid kClessMastercardAid(
                                       { 0xA0, 0x00, 0x00, 0x00, 0x04, 0x10,
                                           0x10 });           //Master card

  types::TagValues mastercard_aid_tags(
      {
          { TAG_EP_KERNEL_TO_USE, { 0x00, 0x02 } },
          { TAG_EP_AID_OPTIONS, { 0x01, 0x01, 0x00, 0x00 } },
          { TAG_GENERIC_AID_PROPRIETARY_IDENTIFIER, { 0x00, 0x00, 0x00, 0x02 } },
          { TAG_EMV_TERMINAL_CAPABILITIES, {0xE0, 0x28, 0xC8}},
          { TAG_PAYPASS_CLESS_TRANSACTION_LIMIT_NO_DCV, { 0x00, 0x00, 0x00,
              0x05, 0x00, 0x00 } },
          { TAG_PAYPASS_CLESS_TRANSACTION_LIMIT_DCV, { 0x00, 0x00, 0x00, 0x05,
              0x00, 0x00 } },
          { TAG_EP_CLESS_FLOOR_LIMIT, { 0x00, 0x00, 0x00, 0x02, 0x00, 0x00 } },
          { TAG_EP_CLESS_CVM_REQUIRED_LIMIT, { 0x00, 0x00, 0x00, 0x01, 0x00,  0x00 } },
          { TAG_EMV_TERMINAL_FLOOR_LIMIT, { 0x00, 0x00, 0x4E, 0x20 } },
          // { TAG_PAYPASS_INT_MSTRIPE_TERMINAL_AVN_LIST, { 0x01, 0x06, 0x02, 0x01, 0x02, 0x06} },  // obsolete
          // { TAG_PAYPASS_INT_MCHIP_TERMINAL_AVN_LIST, { 0x01, 0x05, 0x02, 0x00, 0x02, 0x05 } },  // obsolete

          { TAG_PAYPASS_MCHIP_CVM_CAPABILITY_CVM_REQUIRED, { 0x20 } },  // signature
          { TAG_PAYPASS_MCHIP_CVM_CAPABILITY_CVM_NOT_REQUIRED, { 0x08 } },  // No CVM

          { TAG_PAYPASS_MSTRIPE_CVM_CAPABILITY_CVM_REQUIRED, { 0x10 } },  // signature
          { TAG_PAYPASS_MSTRIPE_CVM_CAPABILITY_CVM_NOT_REQUIRED, { 0x00 } },  // No CVM

          { TAG_PAYPASS_DEFAULT_UDOL, { 0x9F, 0x6A, 0x04 } },

          { TAG_EMV_TERMINAL_RISK_MANAGEMENT_DATA, { 0x2C, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },

          // { TAG_PAYPASS_MSTRIPE_INDICATOR, { 0x01 } },  // obsolete

          { TAG_EMV_INT_TAC_DEFAULT, { 0xFC, 0x50, 0x80, 0x88, 0x00 } },  // FC 50 80 88 00
          { TAG_EMV_INT_TAC_DENIAL, { 0x00, 0x00, 0x00, 0x00, 0x00 } }, // 00 00 00 00 00
          { TAG_EMV_INT_TAC_ONLINE, { 0xFC, 0x50, 0x80, 0x88, 0x00 } }, // FC 50 80 88 00

          { TAG_PAYPASS_TRANSACTION_CATEGORY_CODE, { 0x52 } },
          { TAG_EMV_INT_DEFAULT_TDOL, { 0x9F, 0x08, 0x02 } },
          { TAG_KERNEL_TERMINAL_SUPPORTED_LANGUAGES, { 0x65, 0x6E, 0x7A, 0x68 } },
          //{TAG_EMV_INT_TRANSACTION_TYPE, { 0x00 } }, Commented
          { TAG_PAYPASS_KERNEL_CONFIGURATION, { 0x20 } }, // EMV, MagStripe, On-device

          { TAG_PAYPASS_SECURITY_CAPABILITY, { 0x08 } },
          { TAG_PAYPASS_INT_SUPPORTED_CAPK_INDEX_LIST, { 0x04, 0x05, 0x06, 0xEF, 0xF1, 0xF3, 0xF8, 0xFE, 0xFA } },
          //Extra Added
          { TAG_EMV_APPLI_VERSION_NUMBER_TERM, { 0x00, 0x02 } },
          { TAG_PAYPASS_CARD_DATA_INPUT_CAPABILITY, { 0xE0 } },
          { TAG_PAYPASS_MSTRIPE_APPLI_VERSION_NUMBER_TERM, { 0x00, 0x01 } },
          { TAG_PAYPASS_MERCHANT_CUSTOM_DATA, { 0x00, 0x00, 0x00, 0x00, 0x00,
              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
              0x00, 0x00, 0x00, 0x00 } }
          });

  return contactless::ClessTerminalApplication(kClessMastercardAid,
                                               mastercard_aid_tags,
                                               GetMasterCardTestCapks());
}

contactless::ClessTerminalApplication GetPaypassMaestroApp() {
  const types::Aid kClessMaestrocardAid( { 0xA0, 0x00, 0x00, 0x00, 0x04, 0x30,
      0x60 });          //Maestro Card     // Visa Electron

  types::TagValues maestro_aid_tags(
      {
          { TAG_EP_KERNEL_TO_USE, { 0x00, 0x02 } },
          { TAG_EP_AID_OPTIONS, { 0x45, 0x01, 0x00, 0x00 } },
          { TAG_GENERIC_AID_PROPRIETARY_IDENTIFIER, { 0x00, 0x00, 0x00, 0x02 } },
          { TAG_PAYPASS_CLESS_TRANSACTION_LIMIT_NO_DCV, { 0x00, 0x00, 0x00,
              0x00, 0x20, 0x00 } },
          { TAG_PAYPASS_CLESS_TRANSACTION_LIMIT_DCV, { 0x00, 0x00, 0x00, 0x00,
              0x20, 0x00 } },
          { TAG_EP_CLESS_FLOOR_LIMIT, { 0x00, 0x00, 0x00, 0x00, 0x15, 0x00 } },
          { TAG_EP_CLESS_CVM_REQUIRED_LIMIT, { 0x00, 0x00, 0x00, 0x00, 0x20,
              0x00 } },
          { TAG_EMV_TERMINAL_FLOOR_LIMIT, { 0x00, 0x00, 0x05, 0xAA } },
          // { TAG_PAYPASS_INT_MSTRIPE_TERMINAL_AVN_LIST, { 0x01, 0x06, 0x02, 0x01, 0x02, 0x06} },  // obsolete
          // { TAG_PAYPASS_INT_MCHIP_TERMINAL_AVN_LIST, { 0x01, 0x05, 0x02, 0x00, 0x02, 0x05 } },  // obsolete
          { TAG_PAYPASS_MCHIP_CVM_CAPABILITY_CVM_REQUIRED, { 0xF8 } },
          { TAG_PAYPASS_MCHIP_CVM_CAPABILITY_CVM_NOT_REQUIRED, { 0x08 } },
          { TAG_PAYPASS_MSTRIPE_CVM_CAPABILITY_CVM_REQUIRED, { 0x00 } },
          { TAG_PAYPASS_MSTRIPE_CVM_CAPABILITY_CVM_NOT_REQUIRED, { 0x00 } },
          { TAG_PAYPASS_DEFAULT_UDOL, { 0x9F, 0x6A, 0x04 } },
          // { TAG_PAYPASS_MSTRIPE_INDICATOR, { 0x01 } },  // obsolete
          { TAG_EMV_INT_TAC_DEFAULT, { 0xD8, 0x40, 0x00, 0xA8, 0x00 } },
          { TAG_EMV_INT_TAC_DENIAL, { 0x00, 0x00, 0x00, 0x00, 0x00 } },
          { TAG_EMV_INT_TAC_ONLINE, { 0xD8, 0x40, 0x04, 0xF8, 0x00 } },
          { TAG_PAYPASS_TRANSACTION_CATEGORY_CODE, { 0x52 } },
          { TAG_EMV_INT_DEFAULT_TDOL, { 0x9F, 0x08, 0x02 } },
          { TAG_KERNEL_TERMINAL_SUPPORTED_LANGUAGES, { 0x65, 0x6E, 0x66, 0x72 } },
          { TAG_EMV_INT_TRANSACTION_TYPE, { 0x00 } },
          { TAG_PAYPASS_KERNEL_CONFIGURATION, { 0xA0 } },
          //Extra Added
          { TAG_EMV_APPLI_VERSION_NUMBER_TERM, { 0x00, 0x02 } },
          { TAG_PAYPASS_CARD_DATA_INPUT_CAPABILITY, { 0xE0 } },
          { TAG_PAYPASS_MSTRIPE_APPLI_VERSION_NUMBER_TERM, { 0x00, 0x01 } },
          { TAG_PAYPASS_MERCHANT_CUSTOM_DATA, { 0x00, 0x00, 0x00, 0x00, 0x00,
              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
              0x00, 0x00, 0x00, 0x00 } },
          { TAG_PAYPASS_SECURITY_CAPABILITY, { 0x08 } },
      });
  return contactless::ClessTerminalApplication(kClessMaestrocardAid,
                                               maestro_aid_tags,
                                               GetMasterCardTestCapks());
}

}
