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
#ifndef BDOR__WORKFLOW_ACTIVITIES_H_
#define BDOR__WORKFLOW_ACTIVITIES_H_

#include <receipt/receipt.h>
#include <bdor/receipts.h>
#include <bdor/ui.h>
#include <bdor/application_settings.h>
#include <bdor/batch.h>
#include <bdor/host_switch.h>
#include <types/amount.h>

namespace bdor {

void PrintReceipts(ApplicationSettings& app_setting, Ui& ui,
                   TransactionStatus transaction_status,
                   const receipt::Receipt& reversal_receipt,
                   const receipt::Receipt& merchant_receipt,
                   const receipt::Receipt& customer_receipt,
                   receipt::Receipt& save_merchant_receipt,
                   receipt::Receipt& save_customer_receipt);

void PrintLoop(Ui& ui, const receipt::Receipt& receipt);

bool DisplayPanAndExpiryDate(const ApplicationSettings& app_settings, Ui* ui,
                             const std::uint64_t issuer_index,
                             const types::Pan& pan,
                             const std::string& expiry_date);

bool IsMemoryFull();

bool IsValidAmount(const ApplicationSettings& app_settings_,
                   const types::Amount& amount);

bool IsValidRefundAmount(const ApplicationSettings& app_settings_,
                         const types::Amount& amount);

bool IsValidInstalmentAmount(
    stdx::optional<InstalmentDefinition>& instalment_config_,
    const types::Amount& amount);

bool IsValidPAN(const CardDefinition& card_definition_config,
                const types::Pan pan);

bool ProcessUploadTC(ApplicationSettings& app_settings,
                     Batch& batch,
                     Host& host,
                     unsigned int host_index);

std::string GetOfflineRRN(time_t tx_datetime, uint32_t stan);

bool RetrieveConfigsFromPan(
    const ApplicationSettings& app_settings,
    const types::Pan& pan,
    stdx::optional<CardDefinition>& card_definition_config,
    stdx::optional<IssuerDefinition>& issuer_definition_config,
    stdx::optional<HostDefinition>& host_definition_config,
    const Transaction& tx);

bool RetrieveInstalmentConfigsFromPan(
    const ApplicationSettings& app_settings,
    const types::Pan& pan,
    stdx::optional<CardDefinition>& card_definition_config,
    stdx::optional<IssuerDefinition>& issuer_definition_config,
    stdx::optional<HostDefinition>& host_definition_config,
    stdx::optional<InstalmentDefinition>& instalment_definition_config);

void SetTransactionDataFromConfigs(
    const stdx::optional<CardDefinition>& card_definition_config,
    const stdx::optional<IssuerDefinition>& issuer_definition_config,
    const stdx::optional<HostDefinition>& host_definition_config,
    Transaction* tx);

void SetInstalmentTransactionDataFromConfigs(
    const stdx::optional<InstalmentDefinition>& instalment_definition_config,
    Transaction* tx);

bool IsBelowFloorLimit(const types::Amount total_amount,
                       const std::uint64_t floor_limit);

uint64_t GetTransactionFloorLimit(const CardDefinition& card_definition_config,
                                  const Transaction& tx);

void SetOfflineApproveTransactionData(Transaction* tx);

bool RequestCvv(const CardDefinition& card_definition_config, Ui& ui,
                Transaction& tx);

bool IsCardExpired(const std::string expiry_date_yymm);
bool CheckCardExpired(Ui& ui, const std::string expiry_date_yymm);
bool IsFallbackAllowed(const ApplicationSettings& app_settings);

void UploadPendingItemsAfterTxnApproved(const ApplicationSettings& app_settings,
                                        Batch& batch,
                                        HostSwitch& host,
                                        unsigned int host_index);
bool IsQPSTransaction(const Transaction& tx,
                      const std::uint64_t qps_floor_limit);
bool CheckAndOpenBatch(const uint64_t host_index, Ui& ui, Batch& batch);

bool IsTxEnabled(TerminalConfiguration& terminal_config,
                 const Transaction& tx);

bool IsTxEnabledPerCard(const CardDefinition& card_definition_config,
                        const Transaction& tx);

bool VerifyTipLimit(const types::Amount& sale_amount,
                    const types::Amount& tip_amount,
                    uint64_t tip_limit);
bool RequestAmount(const ApplicationSettings& app_settings, Ui& ui,
                   const bool tip_required,
                   Transaction& tx);

bool RequestAmount(const ApplicationSettings& app_settings,
                   Ui& ui,
                   Transaction& tx);

bool RequestPreauthAmount(const ApplicationSettings& app_settings,
                          Ui& ui,
                          Transaction& tx);

uint32_t GetNextUniqueInvoice(Batch& batch);

void SaveLastTransactionReceipt(const ApplicationSettings& app_setting,
                                receipt::Receipt& merchant_receipt,
                                receipt::Receipt& customer_receipt);

bool CheckFileExist(std::string path);
TxReviewDetail GetTxDetailToDisplay(const ApplicationSettings& app_settings,
                                    const Transaction& tx);

bool RunDccFlow(
                const ApplicationSettings& app_settings,
                Ui& ui,
                Batch& batch,
                HostSwitch& host,
                Transaction& tx,
                HostAuthorizedFunc dcc_query_func,
                HostAuthorizedFunc dcc_auth_func,
                HostSwitch::Status& host_status);

}

#endif
