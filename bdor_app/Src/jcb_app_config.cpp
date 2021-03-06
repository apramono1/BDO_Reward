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
#include "jcb_app_config.h"

#include <emvconfig/emv_config_predefined.h>
#include <tpcore/telium_manager.h>

#include "EmvLib_Tags.h"
#include <EMV_ApiTags.h>
#include "keys.h"

namespace bdor {

std::vector<unsigned long> GetRequiredTagForJcb() {
  std::vector<unsigned long> tag_list = { 0x82, 0x84, 0x95, 0x9A, 0x9C, 0x5F2A, 0x5F34, 0x9F02, 0x9F03,
                                      0x9F09, 0x9F10, 0x9F1A, 0x9F1E, 0x9F26, 0x9F27, 0x9F33, 0x9F34,
                                      0x9F35, 0x9F36, 0x9F37, 0x9F41, 0x9F42, 0x9F5A, 0x9F5B };

  return tag_list;
}

emv::EmvTerminalApplication GetJcbApp(){
  types::TagValues aid_tags( {
           { TAG_EMV_APPLI_VERSION_NUMBER_TERM, { 0x00, 0x00 } },
           { TAG_EMV_TERMINAL_CAPABILITIES, { 0xe0, 0x20, 0xc8 } },
           { TAG_EMV_TERMINAL_FLOOR_LIMIT, { 0x00, 0x00, 0x03, 0xE8 } },  // 9F1B
           { TAG_EMV_INT_THRESHOLD_VALUE_BIASED_RAND_SEL, { 0x00, 0x00, 0x00,
               0x00 } },
           { TAG_EMV_INT_TARGET_PERC_RAND_SEL, { 0 } },
           { TAG_EMV_INT_MAX_TARGET_PERC_BIASED_RAND_SEL, { 0 } },
           { TAG_EMV_INT_TAC_DENIAL, { 0x00, 0x10, 0x00, 0x00, 0x00 } },
           { TAG_EMV_INT_TAC_ONLINE, { 0xFC, 0x60, 0x24, 0xF8, 0x00 } },
           { TAG_EMV_INT_TAC_DEFAULT, { 0xFC, 0x60, 0x24, 0xA8, 0x00 } },
           { TAG_EMV_INT_DEFAULT_DDOL, { 0x9F, 0x37, 0x04 } },
           { TAG_EMV_INT_DEFAULT_TDOL, { 0x95, 0x05, 0x9A, 0x03 } } });

  return emv::EmvTerminalApplication(kJcbAid, aid_tags, true, GetJcbTestCapks(),
                                     GetRequiredTagForJcb(), emv::EmvTerminalApplication::FORMAT_BERTLV, 8);
}
}
