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
#include <paywave_config.h>
#include <paypass_config.h>
#ifndef __TELIUM3__
#include <jspeedy_config.h>
#endif
#include <expresspay_config.h>
namespace bdor {
contactless::ClessConfig::uptr BuildClessConfig() {
  types::TagValues global_tags( {
      { TAG_GENERIC_DETECTION_TYPE, { 0x03, 0x00, 0x00, 0x00 } },
      { TAG_GENERIC_DETECTION_GLOBAL_TIMEOUT, { 0x00, 0x00, 0x17, 0x70 } },
      { TAG_GENERIC_DETECTION_NB_CARDS_TO_DETECT, { 0x01 } },
      { TAG_EMV_TERMINAL_COUNTRY_CODE, { 0x07, 0x02 } },
      { TAG_EMV_ADD_TERMINAL_CAPABILITIES, { 0x60, 0x00, 0xF0, 0xA0, 0x01 } },
      { TAG_EMV_TERMINAL_TYPE, { 0x22 } },
      { TAG_EMV_ACQUIRER_IDENTIFIER, { 0x01, 0x23, 0x45, 0x67, 0x89, 0x01 } },
      { TAG_EMV_MERCHANT_CATEGORY_CODE, { 0x00, 0x00 } },
      { TAG_EMV_MERCHANT_IDENTIFIER, { 0x31, 0x31, 0x32, 0x32, 0x33, 0x33, 0x34,
          0x34,
          0x35, 0x35, 0x36, 0x36, 0x37, 0x37, 0x38 } },
      { TAG_EMV_TERMINAL_IDENTIFICATION, { 0x31, 0x32, 0x33, 0x34, 0x35, 0x36,
          0x37,
          0x38 } }, });

  auto apps = std::vector<contactless::ClessTerminalApplication> {
      GetPaywaveVisaApp(),
      GetPaypassMasterCardApp(),
      GetPaypassMasterCardApp(),
      #ifndef __TELIUM3__
      GetJSpeedyApp(),
#endif
      GetExpressPayApp()
  };

  return stdx::make_unique<contactless::ClessConfigPredefined>(global_tags,
                                                               apps);
}

}
