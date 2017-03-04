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
#include "emv_config.h"
#include "visa_app_config.h"
#include "mastercard_app_config.h"
#include "jcb_app_config.h"
#include "cup_app_config.h"
#include "amex_app_config.h"

#include <emvconfig/emv_config_predefined.h>
#include <tpcore/telium_manager.h>

#include "EmvLib_Tags.h"
#include <EMV_ApiTags.h>

namespace bdor {

emvconfig::EmvConfigPredefined BuildEmvConfig() {

  types::TagValues global_tags( {
      { TAG_EMV_OPTION_PSE, { 1 } },
      { TAG_EMV_INT_AS_ALLOW_BLOCKED_AID, { 0 } },
      { TAG_EMV_ADD_TERMINAL_CAPABILITIES, { 0xf0, 0x00, 0xf0, 0xa0, 0x01 } },
      { TAG_EMV_TERMINAL_COUNTRY_CODE, { 0x07, 0x02 } },
      { TAG_EMV_TERMINAL_IDENTIFICATION, { 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
          0x30, 0x31 } },
      { TAG_EMV_TERMINAL_TYPE, { 0x22 } },
      { TAG_EMV_POS_ENTRY_MODE, { 0 } } });

  return emvconfig::EmvConfigPredefined(global_tags, { GetVisaApp(),
                                            GetMasterCardApp(), GetJcbApp(),
                                            GetCupApp(), GetAmexApp() },
                                        tpcore::GetTerminalSerialNumber());
}

}

