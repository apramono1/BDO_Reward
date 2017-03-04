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
#ifndef BDOR__MASTERCARD_APP_CONFIG_H_
#define BDOR__MASTERCARD_APP_CONFIG_H_

#include <types/Aid.h>
#include <emvconfig/emv_config_predefined.h>

namespace bdor {

static const types::Aid kMasterCardAid({ 0xA0, 0x00, 0x00, 0x00, 0x04 });

emv::EmvTerminalApplication GetMasterCardApp();

}

#endif
