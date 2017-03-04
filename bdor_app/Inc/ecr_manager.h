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
#ifndef BDOR__ECR_MANAGER_H_
#define BDOR__ECR_MANAGER_H_

#include <chrono>
#include <posgate/ecr_interface.h>
#include <posgate/manager.h>
#include <bdor/application_settings.h>
#include <bdor/batch.h>
#include <bdor/ui.h>
#include <bdor/receipts.h>
#include "acquirer.h"

namespace bdor {

class EcrManager : public posgate::EcrInterface {
 public:
  EcrManager(ApplicationSettings& app_setting,
             Ui& ui,
             Receipts& receipts,
             Acquirer& acquirer,
             Batch& batch);
  ~EcrManager();

  void MessageReceived();
  posgate::Message Submit(const posgate::Message& message) final;

 private:

  /*
   * Methods called after selection by Telium Manager.
   */

  std::unique_ptr<Transaction> ProcessSale(const types::Amount& amount) const;
  std::unique_ptr<Transaction> ProcessPreauth(
      const types::Amount& amount) const;
  int ProcessOfflineSale(const types::Amount& amount);
  std::unique_ptr<Transaction> ProcessRefund(const types::Amount& amount) const;
  int ProcessCashAdvance(const types::Amount& amount);
  int ProcessMotoSale(const types::Amount& amount);
  int ProcessMotoRefund(const types::Amount& amount);
  int ProcessTipAdjust(const types::Amount& amount);
  int ProcessVoid(const std::string& invoice_number);
  int ProcessInstalmentSale(const types::Amount& amount);
  int ProcessAuthorizationCompletion(const types::Amount& amount);
  posgate::Message ProcessTransaction(const posgate::Message& message);
  posgate::Message ProcessDuplicate();
  posgate::Message ProcessSettlement();

  /*
   * Methods called as a service call, with Telium Manager staying in Idle.
   */

  int GetLastTransaction(utils::bytes&);
  bool GetLastReceipt();
  int CancelTransaction();

  ApplicationSettings& app_setting_;
  Ui& ui_;
  Receipts& receipts_;
  Acquirer& acquirer_;
  Batch& batch_;
  posgate::Manager pg_manager_;
};

}

#endif

