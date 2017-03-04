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
#include <bdor/transaction_type_name.h>

namespace bdor {

std::string GetTransactionTypeName(const Transaction& txn) {
  std::string txn_type;
  if (txn.is_voided)
    txn_type = "VOID ";

  if (txn.is_adjusted)
    txn_type += "ADJUST ";

  switch (txn.transaction_type) {
    case TransactionType::SALE:
      txn_type += "SALE";
      break;

    case TransactionType::OFFLINE_SALE:
      txn_type += "OFFLINE SALE";
      break;

    case TransactionType::CASH_ADVANCE:
      txn_type += "CASH ADVANCE";
      break;

    case TransactionType::REFUND:
      txn_type += "REFUND";
      break;

    case TransactionType::REWARD:
      txn_type += "REWARD";
      break;

    case TransactionType::PREAUTH:
      case TransactionType::PREAUTH_CANCELLATION:
      if (txn.is_preauth_completed)
        txn_type += "PREAUTH COMPLETED";
      else
        txn_type += "PREAUTH";
      break;
    case TransactionType::PREAUTH_COMPLETION_OFFLINE:
      case TransactionType::PREAUTH_COMPLETION_ONLINE:
      txn_type += "PREAUTH COMP";
      break;
    case TransactionType::INSTALMENT_SALE:
         txn_type += "INSTALMENT SALE";
         break;
    default:
      break;
  }

  return txn_type;
}

}
