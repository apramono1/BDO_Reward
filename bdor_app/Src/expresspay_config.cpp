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
#include "keys.h"

namespace bdor {

//Temporary solve lib include issue.
#define TAG_EXPRESSPAY_UNPREDICTABLE_NUMBER_RANGE   0x9F918200
#define TAG_EXPRESSPAY_INT_TERMINAL_AVN_LIST      0x9F91820A
#define TAG_EXPRESSPAY_TERMINAL_CAPABILITIES        0x9F6D
#define TAG_KERNEL_TERMINAL_SUPPORTED_LANGUAGES         0x9F91841D
#define TAG_EXPRESSPAY_TERMINAL_TRANSACTION_CAPABILITIES  0x9F6E
#define TAG_EXPRESSPAY_FULL_ONLINE_EMV_REMOVAL_TIMEOUT  0x9F91820F

types::TagValues expresspay_aid_tags(
    {
        { TAG_EP_KERNEL_TO_USE, { 0x00, 0x04 } },  // EXPRESSPAY
        { TAG_EP_AID_OPTIONS, { 0x25, 0x03, 0x00, 0x00 } },

        { TAG_EP_CLESS_TRANSACTION_LIMIT, { 0x00, 0x00, 0x00, 0x00, 0x15, 0x00 } },
        { TAG_EP_CLESS_FLOOR_LIMIT, { 0x00, 0x00, 0x00, 0x00, 0x12, 0x00 } },
        { TAG_EP_CLESS_CVM_REQUIRED_LIMIT, { 0x00, 0x00, 0x00, 0x00, 0x10, 0x00 } },
        { TAG_EMV_TERMINAL_FLOOR_LIMIT, { 0x00, 0x00, 0x04, 0xB0 } },

        { TAG_EXPRESSPAY_UNPREDICTABLE_NUMBER_RANGE, { 0x00, 0x3C } },
        { TAG_EXPRESSPAY_INT_TERMINAL_AVN_LIST, { 0x00, 0x01, 0x00, 0x02, 0x00, 0x03 } },
        { TAG_KERNEL_TERMINAL_SUPPORTED_LANGUAGES, { 0x65, 0x6E } }, // en
        { TAG_EMV_TERMINAL_CAPABILITIES, { 0xE0, 0xA8, 0x88 } }, // TAG_EMV_TERMINAL_CAPABILITIES : E0 68 88 (Plaintext Off PIN, PIN Online, Signature and No CVM, SDA and CDA)
        { TAG_EXPRESSPAY_TERMINAL_CAPABILITIES, { 0xC0 } }, // 00 (ExpressPay 1.0), 40 (ExpressPay 2.0 Magstripe Only), 80 (ExpressPay 2.0 EMV and Magstripe), C0 (ExpressPay Mobile - EMV)
        { TAG_EXPRESSPAY_TERMINAL_TRANSACTION_CAPABILITIES, { 0xD0, 0x30, 0x00, 0x00 } },  // Contact & Magstripe & EMV Full Online & Mobile
        { TAG_EXPRESSPAY_FULL_ONLINE_EMV_REMOVAL_TIMEOUT, { 0x00, 0x00, 0x27, 0x10 } },  // (4E20=20 sec) (2710=10 sec) (1388=5 sec)
        { TAG_EMV_INT_TAC_DEFAULT, { 0xDC, 0x50, 0x84, 0x00, 0x00 } },
        { TAG_EMV_INT_TAC_DENIAL, { 0x00, 0x10, 0x00, 0x00, 0x00 } },
        { TAG_EMV_INT_TAC_ONLINE, { 0xC4, 0x00, 0x00, 0x00, 0x00 } },
    });

contactless::ClessTerminalApplication GetExpressPayApp() {
  const types::Aid kClessExpressPayAid(
                                       { 0xA0, 0x00, 0x00, 0x00, 0x25, 0x01 });
  return contactless::ClessTerminalApplication(kClessExpressPayAid,
                                               expresspay_aid_tags,
                                               GetAmexTestCapks());
}

}
