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

#include <field_utils.h>
#include <iso8583/encoder.h>
#include <utils/converter.h>
#include <utils/logger.h>
#include <types/pos_entry_mode.h>
#include <types/pos_condition_code.h>
#include "table.h"

using namespace types;

namespace bdor {

std::string GetProcessingCodeValue(TransactionType tx_type,
                                   bool is_voided_or_adjusted) {
  std::string output = GetProcessingCodeTransactionType(tx_type,
                                                        is_voided_or_adjusted);
  output += "00";  // TODO: wait for FDMS reply about that
  output += "00";
  return output;
}

std::string GetProcessingCodeTransactionType(TransactionType tx_type,
                                             bool is_voided_or_adjusted) {
  std::string output;
  switch (tx_type) {
    case TransactionType::SALE:
      case TransactionType::OFFLINE_SALE:
      case TransactionType::INSTALMENT_SALE:
      case TransactionType::PREAUTH_COMPLETION_OFFLINE:
      if (is_voided_or_adjusted) {
        output = "02";
      } else {
        output = "00";
      }
      break;
    case TransactionType::CASH_ADVANCE:
      output = "01";
      break;
    case TransactionType::REFUND:
      if (is_voided_or_adjusted) {
        output = "22";
      } else {
        output = "20";
      }
      break;
    case TransactionType::PREAUTH_CANCELLATION:
      output = "20";
      break;
    case TransactionType::PREAUTH_COMPLETION_ONLINE:
      if (is_voided_or_adjusted) {
        output = "20";
      } else {
        output = "00";
      }
      break;
    case TransactionType::PREAUTH:
      output = "30";
      break;
    case TransactionType::REWARD:
      output = "33";
      break;
    default:
      logger::die_unless(false, "Transition type not implemented");
      break;
  }
  return output;
}

std::string GetPosEntryModeValue(types::PosEntryMode pos_entry_mode) {
  std::string value;
  switch (pos_entry_mode) {
    case types::PosEntryMode::MANUAL:
      value = "01";
      break;
    case types::PosEntryMode::MAGSTRIPE:
      value = "02";
      break;
    case types::PosEntryMode::CHIP:
      value = "05";
      break;
    case types::PosEntryMode::CONTACTLESS:
      value = "07";
      break;
    case types::PosEntryMode::FALLBACK_MAGSTRIPE:
      value = "80";
      break;
    case types::PosEntryMode::CONTACTLESS_MAGSTRIPE:
      value = "91";
      break;
    default:
      logger::die_unless(false, "Unknown Pos Entry Mode");
      break;
  }

  value += "1";  // TODO: review that. Currently assuming all terminals have PIN Entry capability
  // 1 is only expected for EMV transactions; otherwise all other transactions should have the value of 2

  return value;
}

uint32_t GetPosConditionCodeValue(types::PosConditionCode pos_condition_code) {
  const uint32_t kPosContionCodeNormal = 0u;
  const uint32_t kPosContionCodeMoto = 8u;
  const uint32_t kPosContionCodePreAuth = 6u;
  const uint32_t kPosContionCodeMotoPreAuth = 99u;

  switch (pos_condition_code) {
    case types::PosConditionCode::NORMAL:
      return kPosContionCodeNormal;

    case types::PosConditionCode::MOTO:
      return kPosContionCodeMoto;

    case types::PosConditionCode::PREAUTH:
      return kPosContionCodePreAuth;

    case types::PosConditionCode::MOTO_PREAUTH:
      return kPosContionCodeMotoPreAuth;

    default:
      return kPosContionCodeNormal;
  }
}

bool IsTrack2ToBeSent(types::PosEntryMode pos_entry_mode) {
  return pos_entry_mode == CHIP ||
      pos_entry_mode == MAGSTRIPE ||
      pos_entry_mode == FALLBACK_MAGSTRIPE ||
      pos_entry_mode == CONTACTLESS ||
      pos_entry_mode == CONTACTLESS_MAGSTRIPE;
}

}
