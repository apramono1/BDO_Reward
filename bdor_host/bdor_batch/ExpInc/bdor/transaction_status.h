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
#ifndef BDOR__TRANSACTION_STATUS_H_
#define BDOR__TRANSACTION_STATUS_H_

namespace bdor {

enum TransactionStatus {
  IN_PROGRESS,
  APPROVED,
  DECLINED,
  NOT_ALLOWED,
  CANCELLED,
  CARD_BLOCKED,
  CARD_REMOVED_FROM_READER,
  CARD_ERROR,
  TERMINAL_ERROR,
  REVERSED,
  TO_REVERSE,
  TO_ADVISE,
  UNKNOWN_ERROR
};

enum InProgressStatus {
  IN_PROGRESS_NONE,
  IN_PROGRESS_VOID,
  IN_PROGRESS_PREAUTH_COMPLETION
};

enum TcUploadStatus {
  TC_UPLOAD_NOT_NEEDED,
  TC_UPLOAD_NEEDED,
  TC_UPLOAD_COMPLETED
};

enum class DccStatus {
  DISABLED,   // terminal is not DCC enabled
  ENQUIRY,
  COMPLETE_NON_DCC,  // DCC not allowed (locals card or currency not supported), processing done in local currency
  DCC_ELIGIBLE, // DCC Eligible, to prompt cardholder to choose
  OPT_OUT,
  OPT_IN
};

enum class DccProvider {
  FEXCO,
  PURE_COMMERCE,
  FINTRAX
};

}

#endif
