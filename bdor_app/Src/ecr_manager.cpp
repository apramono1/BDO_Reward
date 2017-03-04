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
#include "ecr_manager.h"
#include <utils/logger.h>

namespace bdor {

using namespace posgate;

EcrManager::EcrManager(ApplicationSettings& app_setting,
                       Ui& ui,
                       Receipts& receipts,
                       Acquirer& acquirer,
                       Batch& batch)
    : app_setting_(app_setting),
      ui_(ui),
      receipts_(receipts),
      acquirer_(acquirer),
      batch_(batch),
      pg_manager_(app_setting.GetTerminalConfiguration().GetEcrServerPort(),
                  *this) {

  pg_manager_.Start();
}

EcrManager::~EcrManager() {
  pg_manager_.Stop();
}

void EcrManager::MessageReceived() {
  pg_manager_.MessageReceived();
}

Message EcrManager::Submit(const Message& message) {

  logger::debug("start EcrManager::Submit");

  auto ei_command_type = GetEiCommandType(message);
  switch (*ei_command_type) {
    case EiCommandType::eicPing: {
      return GetPingResponse();
    }
    case EiCommandType::eicFinancial: {
      return ProcessTransaction(message);
    }
    case EiCommandType::eicSettle: {
      return ProcessSettlement();
    }
    case EiCommandType::eicDuplicate: {
      return ProcessDuplicate();
    }
    default:
      return GetBaseResponse(*ei_command_type,
                             EiResult::eiErrorInvalidCommand);
  }
  return Message();
}

}
