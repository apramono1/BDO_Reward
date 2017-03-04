#include <bdor/clear_reversal_workflow.h>

namespace bdor {

bool ClearReversalWorkFlow::Start() {

  bool reversal_found = false;

  // get host list
  auto host_list = app_setting_.GetAllHostDefinitions();

  for (unsigned int i = 0; i < host_list.size(); i++) {

    HostDefinition& host = host_list[i];

    std::vector<Transaction> reversal = batch_.GetReversal(host.index);
    std::unique_ptr<Transaction> tx_;

    if (reversal.empty()) {
      continue;
    }
    else {
      reversal_found = true;
      Transaction& tx = reversal[0];

      //Ask for confirmation
      if (!ui_.ConfirmClearReversal(tx.invoice_num, tx.GetTotalAmount()))
        return false;
    }

    bool ret = batch_.ClearReversal(host.index);

    if (ret)
      ui_.ReversalCleared();
  }

  if (!reversal_found) {
    ui_.MessageReversalNotFound();
  }

  return true;
}
}
