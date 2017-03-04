/*
 ------------------------------------------------------------------------------
 INGENICO Technical Software Department
 ------------------------------------------------------------------------------
 Copyright (c) 2016 INGENICO S.A.
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
#ifndef BDOR__ADVICE_PROCESSING_H_
#define BDOR__ADVICE_PROCESSING_H_

#include <bdor/application_settings.h>
#include <bdor/batch.h>
#include <bdor/host_switch.h>

namespace bdor {

bool ProcessAllPendingAdvices(Batch& batch,
                              HostSwitch& host,
                              unsigned int host_index);

bool ProcessNextPendingAdvice(Batch& batch,
                              HostSwitch& host,
                              unsigned int host_index);

}

#endif
