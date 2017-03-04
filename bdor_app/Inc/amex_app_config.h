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
#ifndef BDOR__AMEX_APP_CONFIG_H_
#define BDOR__AMEX_APP_CONFIG_H_

#include <types/aid.h>
#include <emv/emv_terminal_application.h>

namespace bdor {

static const types::Aid kAmexCardAid({ 0xA0, 0x00, 0x00, 0x00, 0x25 });

emv::EmvTerminalApplication GetAmexApp();

}

#endif
