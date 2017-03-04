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
#ifndef BDOR__SHOW_TRANSACTION_INFO_H_
#define BDOR__SHOW_TRANSACTION_INFO_H_

#include <stdx/memory>

#include <bdor/transaction.h>
#include <bdor/batch.h>
#include <bdor/ui.h>
#include <bdor/application_settings.h>

namespace bdor {

std::unique_ptr<Transaction> AskUserToSelectTransaction(ApplicationSettings& app_setting,
    Ui& ui, const std::vector<Transaction>& tx_list);

std::unique_ptr<Transaction> RetrieveTransactionInfo(
    ApplicationSettings& app_setting,
    Batch& batch,
    Ui& ui,
    Ui::RecordSearchOption search_option);
Transactions RetrieveTransactions(
    ApplicationSettings& app_setting,
    Batch& batch,
    Ui& ui,
    Ui::RecordSearchOption search_option);
}

#endif
