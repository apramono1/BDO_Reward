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
//#include <Common_Kernels_tags.h>
//#include <payWave_Tags.h>
#include <Keys.h>

namespace bdor {

#define TAG_PAYWAVE_TERM_SUPPORTED_FDDA_VERSIONS           0x9f918307

types::TagValues visa_aid_tags(
     {
         { TAG_EP_KERNEL_TO_USE, { 0x00, 0x03 } },
         { TAG_EP_AID_OPTIONS, { 0x01, 0x01, 0x00, 0x00 } },
         { TAG_EP_TERMINAL_TRANSACTION_QUALIFIERS, { 0x32, 0x80, 0x40, 0x00 } },
         { TAG_GENERIC_AID_PROPRIETARY_IDENTIFIER, { 0x00, 0x00, 0x00, 0x01 } },
         { TAG_EP_CLESS_TRANSACTION_LIMIT,
             { 0x00, 0x00, 0x00, 0x05, 0x00, 0x00 } },
         { TAG_EP_CLESS_FLOOR_LIMIT, { 0x00, 0x00, 0x00, 0x02, 0x00, 0x00 } },
         { TAG_EP_CLESS_CVM_REQUIRED_LIMIT, { 0x00, 0x00, 0x00, 0x01, 0x00, 0x00 } },
         { TAG_EMV_TERMINAL_FLOOR_LIMIT, { 0x00, 0x00, 0x4E, 0x20 } },
         { TAG_PAYWAVE_TERM_SUPPORTED_FDDA_VERSIONS, { 0x00, 0x01 } },
         { TAG_EMV_TERMINAL_CAPABILITIES, { 0xE0, 0x28, 0xC8 } }
     });

contactless::ClessTerminalApplication GetPaywaveVisaApp() {
  const types::Aid kClessVisadcAid( { 0xA0, 0x00, 0x00, 0x00, 0x03, 0x10, 0x10 });  //Visa credi debit
  return contactless::ClessTerminalApplication(kClessVisadcAid,
                                                      visa_aid_tags,
                                                      GetVisaTestCapks());
}

contactless::ClessTerminalApplication GetPaywaveElectronApp() {
  const types::Aid kClessVisaElectronAid( { 0xA0, 0x00, 0x00, 0x00, 0x03, 0x20,
      0x10 });       // Visa Electron

  return contactless::ClessTerminalApplication(kClessVisaElectronAid,
                                                        visa_aid_tags,
                                                        GetVisaTestCapks());
}

}
