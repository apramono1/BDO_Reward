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
#ifndef BDOR__TRANSACTION_TYPE_H_
#define BDOR__TRANSACTION_TYPE_H_

namespace bdor {

enum TransactionType {
  PREAUTH,
  PREAUTH_CANCELLATION,
  PREAUTH_COMPLETION_ONLINE,
  PREAUTH_COMPLETION_OFFLINE,
  SALE,
  CASH_ADVANCE,
  REFUND,
  OFFLINE_SALE,
  INSTALMENT_SALE,
  REWARD_POINT,
};

}

#endif

