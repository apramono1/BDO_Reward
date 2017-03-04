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
#include <stdx/thread>
#include <tpcore/cam_device_guard.h>

#include "remove_chip_card.h"

namespace bdor {

void WaitForCardRemoval(Ui& ui) {
  tpcore::CamDeviceGuard cam_device_guard(tpcore::CamDeviceGuard::CAM_TERMINAL);

  ui.MessagePleaseRemoveCard();
  while (cam_device_guard.IsCardInserted() == true)
    stdx::this_thread::sleep_for(std::chrono::milliseconds(200));
}

}
