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

#ifndef BDOR__WORKFLOW_STATUS_H_
#define BDOR__WORKFLOW_STATUS_H_

namespace bdor {
enum class WorkflowStatus {
  BATCH_ERROR,
  COMM_ERROR,

  FLOW_ERROR,
  FLOW_COMPLETE,

  MANUAL_SUCCESS,
  MANUAL_TIMEOUT_OR_CANCEL,
  MANUAL_ERROR,

  MAG_SUCCESS,
  MAG_TIMEOUT_OR_CANCEL,
  MAG_ERROR,
  CHIP_CARD_SWIPED,

  EMV_SUCCESS,
  EMV_CARD_BLOCKED,
  EMV_FALLBACK_NEEDED,
  EMV_ERROR,
  EMV_CANCELLED,

  CLESS_SUCCESS,
  CLESS_FALLBACK_NEEDED,
  CLESS_ERROR,
  CLESS_CANCELLED,
  CLESS_DECLINED,
  CLESS_MOBILE
};
}

#endif
