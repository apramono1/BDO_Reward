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
#include "show_transaction_info.h"
#include <utils/logger.h>
#include <bdor/transaction_type_name.h>
#include "workflow_activities.h"

namespace bdor {

std::unique_ptr<Transaction> AskUserToSelectTransaction(
    ApplicationSettings& app_setting,
    Ui& ui,
    const std::vector<Transaction>& tx_list) {

  std::unique_ptr<Transaction> tx_uptr;

  if (tx_list.empty()) {
    return nullptr;
  }

  if (tx_list.size() > 1) {
    int index = 0;
    std::vector<TxReviewDetail> tx_menu;
    for (auto const& i : tx_list) {
      TxReviewDetail tx_detail = GetTxDetailToDisplay(app_setting, i);
      tx_menu.push_back(tx_detail);
    }
    if (!ui.SelectTransaction(tx_menu, &index)) {
      return nullptr;
    }
    tx_uptr = stdx::make_unique<Transaction>(tx_list[index]);
  } else {
    tx_uptr = stdx::make_unique<Transaction>(tx_list[0]);
  }

  return tx_uptr;
}

std::unique_ptr<Transaction> RetrieveTransactionInfo(
    ApplicationSettings& app_setting,
    Batch& batch,
    Ui& ui,
    Ui::RecordSearchOption search_option) {

  std::unique_ptr<Transaction> tx_uptr = nullptr;

  std::vector<Transaction> tx_list = RetrieveTransactions(app_setting, batch,
                                                          ui,
                                                          search_option);

  if (tx_list.empty()) {
    ui.MessageTransactionNotFound();
    return nullptr;
  }

  tx_uptr = AskUserToSelectTransaction(app_setting, ui, tx_list);

  return tx_uptr;
}

Transactions RetrieveTransactions(
                                  ApplicationSettings& app_setting,
                                  Batch& batch,
                                  Ui& ui,
                                  Ui::RecordSearchOption search_option) {

  Transactions tx_list;
  std::string currency_name = app_setting.managed_settings_->terminal_config
      .TerminalBaseCurrency().GetName();
  types::Amount amount(currency_name.c_str(), 0);

  std::vector<uint8_t> pan_no;
  switch (search_option) {
    case Ui::SEARCH_BY_TRACE_NUMBER:
      uint32_t stan_no;
      if (!ui.RequestTraceNumber(stan_no))
        return tx_list;
      tx_list = batch.FindTransactionByStan(stan_no);
      break;
    case Ui::SEARCH_BY_ACCOUNT_PAN:
      if (!ui.RequestPan(pan_no))
        return tx_list;
      tx_list = batch.FindTransactionByPan(pan_no);
      break;
    case Ui::SEARCH_BY_INVOICE_NUMBER:
      uint32_t invoice;
      if (!ui.RequestInvoice(invoice))
        return tx_list;
      tx_list = batch.FindTransactionByInvoice(invoice);
      break;
    case Ui::SEARCH_BY_TX_AMOUNT:
      if (!ui.RequestAmount(amount))
        return tx_list;
      tx_list = batch.FindTransactionByAmount(amount);
      break;
    default:
      break;
  }

  if (tx_list.empty()) {
    ui.MessageTransactionNotFound();
    return tx_list;
  }

  return tx_list;
}

}
