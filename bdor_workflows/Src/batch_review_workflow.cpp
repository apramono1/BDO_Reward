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
#include <bdor/batch_review_workflow.h>

#include <utils/logger.h>
#include <idalloc/idalloc.h>
#include "show_transaction_info.h"
#include "workflow_activities.h"
#include <bdor/transaction_type_name.h>
#include "utils/mask.h"
#include <stdx/string>

namespace bdor {

BatchReviewWorkflow::BatchReviewWorkflow(ApplicationSettings& app_setting,
                                         Batch& batch,
                                         Ui& ui)
    : app_settings_(app_setting),
      batch_(batch),
      ui_(ui) {
}

void BatchReviewWorkflow::Start() {
  logger::debug("Starting Batch Review workflow");

  Ui::RecordSearchOption search_option;
  if (!ui_.SelectRecordSearchOption(search_option))
    return;

  auto txs = RetrieveTransactions(app_settings_, batch_, ui_, search_option);

  if (txs.empty())
    return;

  if (txs.size() > 1) {
    std::vector<TxReviewDetail> tx_menu;
    for (auto const& i : txs) {
      TxReviewDetail tx_detail = GetTxDetailToDisplay(app_settings_, i);
      tx_menu.push_back(tx_detail);
    }
    DispatchMenu(tx_menu);
  } else {
    TxReviewDetail tx_detail = GetTxDetailToDisplay(app_settings_,txs[0]);
    ui_.DisplayTxReview(tx_detail);
  }
  return;
}

void BatchReviewWorkflow::DispatchMenu(
    const std::vector<TxReviewDetail>& txs) const {

  std::vector<std::string> items;
  for (auto menu_item : txs) {
    std::string tx = menu_item.tx_name + " "
        + menu_item.amount.GetCurrencyName() + " "
        + menu_item.amount.ToString();
    items.push_back(tx);
  }
  unsigned long selection = 0;
  while (true) {
    selection = ui_.MenuSelectionIndex("REVIEW", items);
    if (selection > txs.size()) {
      ui_.MessageProcessAborted();
      return;
    }
    TxReviewDetail tx_detail = txs[selection];
    ui_.DisplayTxReview(tx_detail);
  }

  return;
}

}
