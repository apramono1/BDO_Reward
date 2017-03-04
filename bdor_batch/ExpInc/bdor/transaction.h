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
#ifndef BDOR__TRANSACTION_H_
#define BDOR__TRANSACTION_H_

#include <string>
#include <vector>
#include <memory>
#include <ctime>

#include <stdx/optional>
#include <stdx/ctime>

#include <types/aid.h>
#include <types/pan.h>
#include <types/amount.h>
#include <types/currency.h>
#include <types/pos_entry_mode.h>
#include <types/pos_condition_code.h>
#include <emv/track2_equivalent_data.h>

#include <bdor/transaction_type.h>
#include <bdor/transaction_status.h>

namespace bdor {

struct Transaction {

  Transaction()
      : rowid(0),
        tx_datetime(stdx::time(NULL)),
        transaction_status(TransactionStatus::IN_PROGRESS),
        previous_transaction_status(TransactionStatus::IN_PROGRESS),
        in_progress_status(InProgressStatus::IN_PROGRESS_NONE),
        tc_upload_status(TcUploadStatus::TC_UPLOAD_NOT_NEEDED),
        dcc_status(DccStatus::DISABLED),
        dcc_provider(DccProvider::FEXCO),
        transaction_type(TransactionType::SALE),
        is_voided(false),
        is_preauth_completed(false),
        is_fallback(false),
        is_adjusted(false),
        pos_entry_mode(types::PosEntryMode::MAGSTRIPE),
        pos_condition_code(types::PosConditionCode::NORMAL),
        batch_num(0),
        invoice_num(0),
        original_invoice_number(0),
        signature_required(false),
        online_authorization_required(false),
        pin_entered(false),
        express_payment(false),
        stan(0),
        tc_upload_stan(0),
        tpdu(""),
        nii(0),
        host_index(0),
        card_index(0),
        issuer_index(0),
        instalment_index(0),
        tenure(0),
        interest_rate(0),
        roi_tenure(0),
        processing_fee(0){
  }

  void SetTransactionStatus(TransactionStatus status);
  void SetTransactionInProgressStatus(InProgressStatus status);
  void RevertToPreviousTransactionStatus();
  void SetTransactionAsVoided();
  void SetTcUploadStatus(TcUploadStatus status);

  void UpdateDccAmounts();
  bool TipSupported() const;

  stdx::optional<types::Amount> GetTotalAmount() const;
  stdx::optional<types::Amount> GetTotalPreauthAmount() const;
  stdx::optional<types::Amount> GetTotalOriginalAmount() const;

  std::int64_t rowid;
  time_t tx_datetime;
  TransactionStatus transaction_status;
  TransactionStatus previous_transaction_status;
  InProgressStatus in_progress_status;
  TcUploadStatus tc_upload_status;
  DccStatus dcc_status;
  DccProvider dcc_provider;
  TransactionType transaction_type;
  bool is_voided;
  bool is_preauth_completed;
  bool is_fallback;
  bool is_adjusted;

  types::PosEntryMode pos_entry_mode;
  types::PosConditionCode pos_condition_code;

  uint32_t batch_num;
  uint32_t invoice_num;
  uint32_t original_invoice_number;

  bool signature_required;  // if a signature is needed for cardholder verification
  bool online_authorization_required;
  bool pin_entered;
  bool express_payment;
  uint32_t stan;
  uint32_t stan_original;  //for detail report
  uint32_t tc_upload_stan;
  std::string processing_code;     //processing code
  std::string tpdu;
  uint32_t nii;

  uint64_t host_index;
  uint64_t card_index;
  uint64_t issuer_index;
  uint64_t instalment_index;

  stdx::optional<types::Pan> pan;
  stdx::optional<unsigned int> pan_sequence_number;
  stdx::optional<types::Amount> amount;  // transaction base amount
  stdx::optional<types::Amount> secondary_amount;  //secondary amount(currently used for tip)
  stdx::optional<types::Amount> original_secondary_amount;
  stdx::optional<types::Amount> dcc_amount;  // amount, settlement
  stdx::optional<types::Amount> dcc_amount_original;  // dcc amount before tip
  stdx::optional<types::Amount> preauth_amount;  // amount, pre-authorization

  std::string expiration_date;  // YYMM
  stdx::optional<emv::Track2EquivalentData> track2_equivalent_data;
  std::string rrn;							//RRN
  std::string auth_id_response;		//Authorization code
  std::string auth_id_response_original;	//Authorization code original (before voided)
  std::string response_code;
  std::string tid;							// terminal id
  std::string mid;
  std::string field63_data;
  std::vector<uint8_t> icc_data; 	//ICC data to be send in DE55
  std::vector<uint8_t> issuer_emv_response;
  std::string cvv;
  std::string number_of_guest;
  std::string card_holder_name;
  std::string application_prefered_name; 	//app name retrieved from ICC to print on receipt
  std::string card_name;        //card name for eg: AMEX/VISA/MASTER

  std::string amex_se_number;   //TODO: check if needed Amex card 4DBC

  std::string transaction_certificate;
  std::vector<uint8_t> tvr;
  std::vector<uint8_t> tsi;
  std::string service_code;
  std::string track1_data;

  std::vector<uint8_t> pin_data;				//temporary data
  stdx::optional<types::Aid> aid;
  stdx::optional<uint64_t> available_offline_spending_limit;  //VISA, AOSA
  std::string signature_file_path;

  std::string instalment_description;

  uint64_t tenure;
  uint32_t interest_rate;
  uint32_t roi_tenure;
  stdx::optional<types::Amount> interest_amount;
  stdx::optional<types::Amount> total_prod_amount;
  stdx::optional<types::Amount> emi_per_month;

  uint32_t processing_fee;
  std::string program_id;
  std::string product_id;

  std::string dcc_staff_id;
  std::string dcc_response;
  std::string dcc_conversion_rate;
  std::string dcc_margin;
  std::string dcc_leg;
  std::string dcc_multi_rate_flag;

  std::vector<types::Amount> dcc_amount_list;
  std::vector<std::string> dcc_conversion_rate_list;
};

using Transactions = std::vector<Transaction>;

}

#endif

