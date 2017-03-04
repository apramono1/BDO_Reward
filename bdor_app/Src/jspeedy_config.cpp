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
//#include <JCB_Tags.h>
#include <Keys.h>

//TAG_JCB_COMBINAISON_OPTIONS : Mandatory - combination options (see JCB specifications A.3)
//TAG_JCB_TERMINAL_INTERCHANGE_PROFILE_S : Mandatory - Terminal Interchange Profile (static). (see JCB specifications A.5)
//TAG_EMV_ACQUIRER_IDENTIFIER : Mandatory - Acquirer identifier.
//TAG_EMV_MERCHANT_NAME_AND_LOCATION : Mandatory - Merchant Name and Location.
//TAG_EMV_TERMINAL_COUNTRY_CODE : Mandatory - Terminal country code.
//TAG_EMV_TERMINAL_TYPE : Mandatory - Terminal Type.
//TAG_EMV_TRANSACTION_CURRENCY_CODE : Mandatory - Transaction currency code.
//TAG_EMV_TRANSACTION_CURRENCY_EXPONENT : Mandatory - Transaction currency exponent.
//TAG_EMV_AMOUNT_AUTH_NUM : Mandatory - Amount Authorised, Numeric format. Take care to be coherent with the provided value in TAG_EMV_AMOUNT_AUTH_BIN. No verification will be performed by the kernel itself.
//TAG_EMV_AMOUNT_OTHER_NUM : Mandatory - Amount Other, Numeric format.
//TAG_EMV_TRANSACTION_DATE : Mandatory - Transaction date.
//TAG_EMV_TRANSACTION_TIME : Mandatory - Transaction time.
//TAG_EMV_TRANSACTION_TYPE : Mandatory - Transaction type.
//TAG_EP_KERNEL_TO_USE : Mandatory - Kernel to be used (for JCB, it shall be set to DEFAULT_EP_KERNEL_JCB_C5).
//TAG_EP_FINAL_SELECT_COMMAND_SENT : Mandatory - Final SELECT command (for internal JCB mandatory checkings).
//TAG_EP_FINAL_SELECT_RESPONSE : Mandatory - Final SELECT response provided by the application selection. It will be analysed by the JCB kernel itself.
//TAG_EP_FINAL_SELECT_STATUS_WORD : Mandatory - Final SELECT response status word provided by the application selection. It will be analysed by the JCB kernel itself.
//TAG_KERNEL_PAYMENT_FLOW_CUSTOM : Mandatory - Data field indicating steps where customisation is necessary.

namespace bdor {

#define TAG_JCB_COMBINAISON_OPTIONS                 0x9F918F01
#define TAG_JCB_IMPLEMENTATION_OPTIONS              0x9F918F24
#define TAG_JCB_TERMINAL_INTERCHANGE_PROFILE_S      0x9F918F09
#define TAG_JCB_THRESHOLD_VALUE_BIASED_RAND_SEL     0x9F918F02
#define TAG_JCB_MDOL                                0x9F5C

types::TagValues jspeedy_aid_tags(
     {
         { TAG_EP_KERNEL_TO_USE, { 0x00, 0x05 } },  // JCB
         { TAG_EP_AID_OPTIONS, { 0x01, 0x01, 0x00, 0x00 } },

         { TAG_EP_CLESS_TRANSACTION_LIMIT,
             { 0x00, 0x00, 0x00, 0x05, 0x00, 0x00 } },
         { TAG_EP_CLESS_FLOOR_LIMIT, { 0x00, 0x00, 0x00, 0x02, 0x00, 0x00 } },
         { TAG_EP_CLESS_CVM_REQUIRED_LIMIT, { 0x00, 0x00, 0x00, 0x01, 0x00, 0x00 } },
         { TAG_EMV_TERMINAL_FLOOR_LIMIT, { 0x00, 0x00, 0x4E, 0x20 } },  // $200.00

         { TAG_JCB_COMBINAISON_OPTIONS, { 0x2B, 0x00 } },
         { TAG_JCB_IMPLEMENTATION_OPTIONS, { 0x07 } },
         { TAG_JCB_TERMINAL_INTERCHANGE_PROFILE_S, { 0xD3, 0x00, 0x00 } },
         { TAG_JCB_THRESHOLD_VALUE_BIASED_RAND_SEL, { 0x00, 0x00, 0x00, 0x00, 0x10, 0x00 } },
         { TAG_EMV_INT_TARGET_PERC_RAND_SEL, { 0x20 } },
         { TAG_EMV_INT_MAX_TARGET_PERC_BIASED_RAND_SEL, { 0x4A } },
         { TAG_EMV_MERCHANT_NAME_AND_LOCATION, { 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20 } },
         { TAG_JCB_MDOL, { 0x9F, 0x02, 0x06, 0x9F, 0x1A, 0x02, 0x5F, 0x2A, 0x02, 0x9A, 0x03, 0x9C, 0x01, 0x9F, 0x53, 0x03, 0x9F, 0x4E, 0x14 } },
         { TAG_EMV_INT_TAC_DEFAULT, { 0xFC, 0x50, 0x80, 0x88, 0x00 } },
         { TAG_EMV_INT_TAC_DENIAL, { 0x00, 0x00, 0x00, 0x00, 0x00 } },
         { TAG_EMV_INT_TAC_ONLINE, { 0xFC, 0x50, 0x80, 0x88, 0x00 } },

         // may need TAG_EMV_MERCHANT_NAME_AND_LOCATION, TAG_EMV_AMOUNT_OTHER_NUM

     });

contactless::ClessTerminalApplication GetJSpeedyApp() {
  const types::Aid kClessJspeedyAid(
      { 0xA0, 0x00, 0x00, 0x00, 0x65, 0x10, 0x10 });
  return contactless::ClessTerminalApplication(kClessJspeedyAid,
                                               jspeedy_aid_tags,
                                               GetJcbTestCapks());
}

}
