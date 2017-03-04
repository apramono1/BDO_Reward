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
#include <bdor/transaction.h>
#include <utils/strings.h>

namespace bdor {

void Transaction::SetTransactionStatus(TransactionStatus status) {
  previous_transaction_status = transaction_status;
  transaction_status = status;
}

void Transaction::SetTransactionInProgressStatus(InProgressStatus status) {
  in_progress_status = status;
}

void Transaction::RevertToPreviousTransactionStatus() {
  transaction_status = previous_transaction_status;
  previous_transaction_status = TransactionStatus::IN_PROGRESS;
}

void Transaction::SetTransactionAsVoided() {
  previous_transaction_status = transaction_status;
  transaction_status = TransactionStatus::APPROVED;
  is_voided = true;
}

void Transaction::SetTcUploadStatus(TcUploadStatus status) {
  tc_upload_status = status;
}

stdx::optional<types::Amount> Transaction::GetTotalAmount() const {
  if (!amount.has_value())
    return stdx::nullopt;

  stdx::optional<types::Amount> total_amount = amount;

  if (secondary_amount.has_value())
    total_amount = *total_amount + *secondary_amount;

  return total_amount;
}

stdx::optional<types::Amount> Transaction::GetTotalPreauthAmount() const {
  if (!preauth_amount.has_value())
    return stdx::nullopt;

  stdx::optional<types::Amount> total_amount = preauth_amount;

  if (secondary_amount.has_value())
    total_amount = *total_amount + *secondary_amount;

  return total_amount;
}

stdx::optional<types::Amount> Transaction::GetTotalOriginalAmount() const {
  if (!amount.has_value())
    return stdx::nullopt;

  stdx::optional<types::Amount> total_amount = amount;

  if (original_secondary_amount.has_value())
    total_amount = *total_amount + *original_secondary_amount;

  return total_amount;
}

void Transaction::UpdateDccAmounts() {
  if (dcc_status == DccStatus::OPT_IN) {
    double conversion_rate = utils::FromString<double>(dcc_conversion_rate);
    auto local_tip_value = (*secondary_amount).GetValue();
    auto dcc_tip_value = (uint64_t)(local_tip_value * conversion_rate);

    auto dcc_tip_amount = types::Amount((*dcc_amount).GetCurrencyName(),
                                        dcc_tip_value);

    dcc_amount_original = dcc_amount;
    dcc_amount = *dcc_amount + dcc_tip_amount;
  }
}

bool Transaction::TipSupported() const {
  if (transaction_type != TransactionType::SALE)
    return false;

  if (pos_condition_code == types::PosConditionCode::MOTO)
    return false;

  return true;
}

}
