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

#include <bdor/contactless_workflow.h>
#include <utils/converter.h>
#include <utils/strings.h>
#include <iso8583/encoder.h>
#include <utils/logger.h>
#include <tpcore/telium_event.h>
#include <tpcore/cam_device_guard.h>

namespace bdor {

ContactlessWorkflow::ContactlessWorkflow(
    contactless::ClessConfig& cless_config, ApplicationSettings& app_setting,
    Ui& ui,
    const contactless::Cless::TransactionType txn_type,
    types::Amount amount)
    : cless_config_(cless_config),
      app_setting_(app_setting),
      ui_(ui)
{
  amount_ = amount;
  is_online_pin_required_ = false;
  cless_explicit_ = stdx::make_unique<contactless::ClessExplicit>(cless_config_,
                                                                  txn_type);
}

ContactlessWorkflow::~ContactlessWorkflow() {
}

bool ContactlessWorkflow::Initialize() {
  return cless_explicit_->Init(&amount_);
}

WorkflowStatus ContactlessWorkflow::ActivateTransaction(Transaction& tx) {
  if (!GetDetectionResult())
    return WorkflowStatus::CLESS_ERROR;

  if (!PerformSelection())
    return WorkflowStatus::CLESS_ERROR;

  WorkflowStatus status = RunSchemeSpecificFlow(tx);
  if (status != WorkflowStatus::CLESS_SUCCESS)
    return status;

  if (!ReadTransactionData(tx))
    return WorkflowStatus::CLESS_ERROR;

  return WorkflowStatus::CLESS_SUCCESS;
}

WorkflowStatus ContactlessWorkflow::ActivateRefundTransaction(Transaction& tx) {
  if (!GetDetectionResult())
    return WorkflowStatus::CLESS_ERROR;

  if (!PerformSelection())
    return WorkflowStatus::CLESS_ERROR;

  WorkflowStatus status = RunSchemeSpecificFlow(tx);
  if (status != WorkflowStatus::CLESS_SUCCESS
      && status != WorkflowStatus::CLESS_DECLINED)
    return status;

  if (!ReadTransactionData(tx))
    return WorkflowStatus::CLESS_ERROR;

  return WorkflowStatus::CLESS_SUCCESS;
}

bool ContactlessWorkflow::StartDetection() {
  return cless_explicit_->StartDetection();
}

bool ContactlessWorkflow::GetDetectionResult() {
  ui_.MessageProcessingCard();
  if (!cless_explicit_->GetDetectionResult()) {
    ui_.MessageCardError();
    return false;
  }
  return true;
}

bool ContactlessWorkflow::PerformSelection() {
  contactless::ClessExplicit::SelectionStatus selection_status = cless_explicit_
      ->PerformSelection();
  if (selection_status
      != contactless::ClessExplicit::SelectionStatus::SEL_SUCCESS) {
    ui_.MessageProcessAborted();
    logger::error("ClessWorkflow - Error during Selection");
    return false;
  }

  logger::debug("ClessWorkflow - Selection successful");
  return true;
}

void ContactlessWorkflow::CancelDetection() {
  cless_explicit_->CancelDetectionAndCloseClessDriver();
}

WorkflowStatus ContactlessWorkflow::RunSchemeSpecificFlow(Transaction& tx) {
  tx.pos_entry_mode = types::PosEntryMode::CONTACTLESS;
  kernel_to_use_ = cless_explicit_->GetKernelToUse();
  if (kernel_to_use_ == contactless::Cless::ClessKernel::KERNEL_UNKNOWN) {
    ui_.MessagePleaseUseOtherInterfaces();
    return WorkflowStatus::CLESS_FALLBACK_NEEDED;
  }

  switch (kernel_to_use_) {
    case contactless::Cless::ClessKernel::KERNEL_PAYPASS:
      return RunPayPassFlow(tx);
    case contactless::Cless::ClessKernel::KERNEL_VISA:
      return RunPayWaveFlow(tx);
#ifndef __TELIUM3__
      case contactless::Cless::ClessKernel::KERNEL_JCB_C5:
      return RunJSpeedyFlow(tx);
#endif
    case contactless::Cless::ClessKernel::KERNEL_AMEX:
      return RunExpressPayFlow(tx);
    default:
      ui_.MessagePleaseUseOtherInterfaces();
      return WorkflowStatus::CLESS_FALLBACK_NEEDED;
  }
}

bool ContactlessWorkflow::ReadTransactionData(Transaction& tx) {
  switch (kernel_to_use_) {
    case contactless::Cless::ClessKernel::KERNEL_PAYPASS:
      ReadPayPassApplicationData(tx);
      break;
    case contactless::Cless::ClessKernel::KERNEL_VISA:
      ReadPayWaveApplicationData(tx);
      break;
#ifndef __TELIUM3__
      case contactless::Cless::ClessKernel::KERNEL_JCB_C5:
      ReadJSpeedyApplicationData(tx);
      break;
#endif
    case contactless::Cless::ClessKernel::KERNEL_AMEX:
      ReadExpressPayApplicationData(tx);
      break;
    default:
      ui_.MessagePleaseUseOtherInterfaces();
      return false;
  }

  return true;
}

std::vector<unsigned int> ContactlessWorkflow::ConstructTagsForPaywave() {
  std::vector<unsigned int> tags;

  tags.push_back(0x82);   //TAG_EMV_AIP
  tags.push_back(0x84);   //TAG_EMV_DF_NAME
  tags.push_back(0x95);   //TAG_EMV_TVR
  tags.push_back(0x9A);   //TAG_EMV_TRANSACTION_DATE
  tags.push_back(0x9C);   //TAG_EMV_TRANSACTION_TYPE
  tags.push_back(0x5F2A);  //TAG_EMV_TRANSACTION_CURRENCY_CODE
  tags.push_back(0x5F34);  //TAG_EMV_APPLI_PAN_SEQUENCE_NUMBER
  tags.push_back(0x9F02);  //TAG_EMV_AMOUNT_AUTH_NUM
  tags.push_back(0x9F03);  //TAG_EMV_AMOUNT_OTHER_NUM
  tags.push_back(0x9F09);  //TAG_EMV_APPLI_VERSION_NUMBER_TERM
  tags.push_back(0x9F10);  //TAG_EMV_ISSUER_APPLI_DATA
  tags.push_back(0x9F1A);  //TAG_EMV_TERMINAL_COUNTRY_CODE
  tags.push_back(0x9F1E);  //TAG_EMV_IFD_SERIAL_NUMBER
  tags.push_back(0x9F26);  //TAG_EMV_APPLICATION_CRYPTOGRAM
  tags.push_back(0x9F27);  //TAG_EMV_CRYPTOGRAM_INFO_DATA
  tags.push_back(0x9F33);  //TAG_EMV_TERMINAL_CAPABILITIES
  tags.push_back(0x9F34);  //TAG_EMV_CVM_RESULTS
  tags.push_back(0x9F35);  //TAG_EMV_TERMINAL_TYPE
  tags.push_back(0x9F36);  //TAG_EMV_ATC
  tags.push_back(0x9F37);  //TAG_EMV_UNPREDICTABLE_NUMBER
  tags.push_back(0x9F41);  //TAG_EMV_TRANSACTION_SEQUENCE_COUNTER
  tags.push_back(0x9F5A);  //TAG_PAYWAVE_APPLICATION_PROGRAM_IDENTIFIER
  tags.push_back(0x9F6E);  //TAG_PAYWAVE_FORM_FACTOR_INDICATOR
  return tags;

}

std::vector<unsigned int> ContactlessWorkflow::ConstructTagsForPaypass() {
  std::vector<unsigned int> tags;

  tags.push_back(0x82);   //TAG_EMV_AIP
  tags.push_back(0x84);   //TAG_EMV_DF_NAME
  tags.push_back(0x95);   //TAG_EMV_TVR
  tags.push_back(0x9A);   //TAG_EMV_TRANSACTION_DATE
  tags.push_back(0x9C);   //TAG_EMV_TRANSACTION_TYPE
  tags.push_back(0x5F2A);  //TAG_EMV_TRANSACTION_CURRENCY_CODE
  tags.push_back(0x5F34);  //TAG_EMV_APPLI_PAN_SEQUENCE_NUMBER
  tags.push_back(0x9F02);  //TAG_EMV_AMOUNT_AUTH_NUM
  tags.push_back(0x9F03);  //TAG_EMV_AMOUNT_OTHER_NUM
  tags.push_back(0x9F09);  //TAG_EMV_APPLI_VERSION_NUMBER_TERM
  tags.push_back(0x9F10);  //TAG_EMV_ISSUER_APPLI_DATA
  tags.push_back(0x9F1A);  //TAG_EMV_TERMINAL_COUNTRY_CODE
  tags.push_back(0x9F1E);  //TAG_EMV_IFD_SERIAL_NUMBER
  tags.push_back(0x9F26);  //TAG_EMV_APPLICATION_CRYPTOGRAM
  tags.push_back(0x9F27);  //TAG_EMV_CRYPTOGRAM_INFO_DATA
  tags.push_back(0x9F33);  //TAG_EMV_TERMINAL_CAPABILITIES
  tags.push_back(0x9F34);  //TAG_EMV_CVM_RESULTS
  tags.push_back(0x9F35);  //TAG_EMV_TERMINAL_TYPE
  tags.push_back(0x9F36);  //TAG_EMV_ATC
  tags.push_back(0x9F37);  //TAG_EMV_UNPREDICTABLE_NUMBER
  tags.push_back(0x9F41);  //TAG_EMV_TRANSACTION_SEQUENCE_COUNTER
  tags.push_back(0x9F53);  //TAG_PAYPASS_TRANSACTION_CATEGORY_CODE
  tags.push_back(0x9F6E);  //TAG_PAYPASS_THIRD_PARTY_DATA
  return tags;

}
#ifndef __TELIUM3__
std::vector<unsigned int> ContactlessWorkflow::ConstructTagsForJSpeedy() {
  std::vector<unsigned int> tags;

  tags.push_back(0x82);   //TAG_EMV_AIP
  tags.push_back(0x84);//TAG_EMV_DF_NAME
  tags.push_back(0x95);//TAG_EMV_TVR
  tags.push_back(0x9A);//TAG_EMV_TRANSACTION_DATE
  tags.push_back(0x9C);//TAG_EMV_TRANSACTION_TYPE
  tags.push_back(0x5F2A);//TAG_EMV_TRANSACTION_CURRENCY_CODE
  tags.push_back(0x5F34);//TAG_EMV_APPLI_PAN_SEQUENCE_NUMBER
  tags.push_back(0x9F02);//TAG_EMV_AMOUNT_AUTH_NUM
  tags.push_back(0x9F03);//TAG_EMV_AMOUNT_OTHER_NUM
  tags.push_back(0x9F09);//TAG_EMV_APPLI_VERSION_NUMBER_TERM
  tags.push_back(0x9F10);//TAG_EMV_ISSUER_APPLI_DATA
  tags.push_back(0x9F1A);//TAG_EMV_TERMINAL_COUNTRY_CODE
  tags.push_back(0x9F1E);//TAG_EMV_IFD_SERIAL_NUMBER
  tags.push_back(0x9F26);//TAG_EMV_APPLICATION_CRYPTOGRAM
  tags.push_back(0x9F27);//TAG_EMV_CRYPTOGRAM_INFO_DATA
  tags.push_back(0x9F33);//TAG_EMV_TERMINAL_CAPABILITIES
  tags.push_back(0x9F34);//TAG_EMV_CVM_RESULTS
  tags.push_back(0x9F35);//TAG_EMV_TERMINAL_TYPE
  tags.push_back(0x9F36);//TAG_EMV_ATC
  tags.push_back(0x9F37);//TAG_EMV_UNPREDICTABLE_NUMBER
  tags.push_back(0x9F41);//TAG_EMV_TRANSACTION_SEQUENCE_COUNTER
  // may need TAG_EMV_APPLI_VERSION_NUMBER_CARD, TAG_EMV_POS_ENTRY_MODE, 0x9F6E
  return tags;
}
#endif
std::vector<unsigned int> ContactlessWorkflow::ConstructTagsForExpressPay() {
  std::vector<unsigned int> tags = { 0x9F26, 0x9F10, 0x9F37, 0x9F36, 0x95,
      0x9A, 0x9C, 0x9F02, 0x5F2A, 0x9F1A, 0x82, 0x9F03, 0x5F34, 0x9F27, 0x9F06,
      0x9F09, 0x9F34, 0x9F0E, 0x9F0F, 0x9F0D };

  return tags;
}

void ContactlessWorkflow::ReadPayPassApplicationData(Transaction& tx) {
  tx.pan = paypass_kernel_->GetPan();
  tx.aid = cless_explicit_->GetSelectedAid();

  auto application_preferred_name =
      paypass_kernel_->GetApplicationPreferredName();
  if (application_preferred_name)
    tx.application_prefered_name = application_preferred_name.value();

  if (tx.application_prefered_name.empty()) {
    auto application_label = paypass_kernel_->GetApplicationLabel();
    if (application_label)
      tx.application_prefered_name = application_label.value();
  }

  auto expiry_date = paypass_kernel_->GetCardExpiryDate();
  if (expiry_date) {
    tx.expiration_date = (*expiry_date).substr(0, 4);
  }

  tx.track2_equivalent_data = paypass_kernel_->GetTrack2EquivalentData();
  if (tx.track2_equivalent_data)
    logger::debug("ClessWorkflow - PAN from Track2: %s",
                  tx.track2_equivalent_data->GetPan().ToString().c_str());

  auto pan_sequence_number = paypass_kernel_->GetPanSequenceNumber();
  if (pan_sequence_number)
    tx.pan_sequence_number = utils::BcdToInt(*pan_sequence_number);

  auto card_holder_name = paypass_kernel_->GetCardholderName();
  if (card_holder_name)
    tx.card_holder_name = *card_holder_name;

  tx.signature_required = paypass_kernel_->IsSignatureRequested();

  auto transaction_certificate = paypass_kernel_->GetApplicationCryptogram();
  if (transaction_certificate) {
    tx.transaction_certificate = utils::BytesToHexString(
        *transaction_certificate);
  }

  is_online_pin_required_ = paypass_kernel_->IsOnlinePinRequested();

  tx.icc_data = paypass_kernel_->GetIccData(ConstructTagsForPaypass());

  tx.tx_datetime = *(cless_explicit_->GetTransactionDateTime());

  auto card_type = paypass_kernel_->GetCardType();

  if (card_type) {
    if (card_type == contactless::PayPassKernel::CardType::MSTRIPE)
      tx.pos_entry_mode = types::PosEntryMode::CONTACTLESS_MAGSTRIPE;
  }
}

void ContactlessWorkflow::ReadPayWaveApplicationData(Transaction& tx) {
  tx.track2_equivalent_data = paywave_kernel_->GetTrack2EquivalentData();
  if (tx.track2_equivalent_data)
    logger::debug("ClessWorkflow - PAN from Track2: %s",
                  tx.track2_equivalent_data->GetPan().ToString().c_str());

  tx.pan = paywave_kernel_->GetPan();
  if (tx.pan)
    logger::debug("ClessWorkflow - PAN from kernel: %s",
                  (*tx.pan).ToString().c_str());
  else {
    logger::debug("No PAN returned from kernel");
    tx.pan = tx.track2_equivalent_data->GetPan();
  }

  auto expiry_date = paywave_kernel_->GetCardExpiryDate();
  if (expiry_date) {
    tx.expiration_date = (*expiry_date).substr(0, 4);
  }
  else
    tx.expiration_date = tx.track2_equivalent_data->GetExpirationDate();

  tx.aid = cless_explicit_->GetSelectedAid();

  auto application_preferred_name =
      paywave_kernel_->GetApplicationPreferredName();
  if (application_preferred_name)
    tx.application_prefered_name = application_preferred_name.value();

  if (tx.application_prefered_name.empty()) {
    auto application_label = paywave_kernel_->GetApplicationLabel();
    if (application_label)
      tx.application_prefered_name = application_label.value();
  }

  auto pan_sequence_number = paywave_kernel_->GetPanSequenceNumber();
  if (pan_sequence_number)
    tx.pan_sequence_number = utils::BcdToInt(*pan_sequence_number);

  auto card_holder_name = paywave_kernel_->GetCardholderName();
  if (card_holder_name)
    tx.card_holder_name = *card_holder_name;

  tx.signature_required = paywave_kernel_->IsSignatureRequested();

  auto transaction_certificate = paywave_kernel_->GetApplicationCryptogram();
  if (transaction_certificate) {
    tx.transaction_certificate = utils::BytesToHexString(
        *transaction_certificate);
  }

  is_online_pin_required_ = paywave_kernel_->IsOnlinePinRequested();

  tx.icc_data = paywave_kernel_->GetIccData(ConstructTagsForPaywave());

  tx.tx_datetime = *(cless_explicit_->GetTransactionDateTime());
}
#ifndef __TELIUM3__
void ContactlessWorkflow::ReadJSpeedyApplicationData(Transaction& tx) {
  tx.track2_equivalent_data = jspeedy_kernel_->GetTrack2EquivalentData();
  if (tx.track2_equivalent_data)
  logger::debug("ClessWorkflow - PAN from Track2: %s",
      tx.track2_equivalent_data->GetPan().ToString().c_str());

  tx.pan = jspeedy_kernel_->GetPan();
  if (tx.pan)
  logger::debug("ClessWorkflow - PAN from kernel: %s",
      (*tx.pan).ToString().c_str());
  else {
    logger::debug("No PAN returned from kernel");
    tx.pan = tx.track2_equivalent_data->GetPan();
  }

  auto expiry_date = jspeedy_kernel_->GetCardExpiryDate();
  if (expiry_date) {
    tx.expiration_date = (*expiry_date).substr(0, 4);
  }
  else
  tx.expiration_date = tx.track2_equivalent_data->GetExpirationDate();

  tx.aid = cless_explicit_->GetSelectedAid();

  auto application_preferred_name =
  jspeedy_kernel_->GetApplicationPreferredName();
  if (application_preferred_name)
  tx.application_prefered_name = application_preferred_name.value();

  if (tx.application_prefered_name.empty()) {
    auto application_label = jspeedy_kernel_->GetApplicationLabel();
    if (application_label)
    tx.application_prefered_name = application_label.value();
  }

  auto pan_sequence_number = jspeedy_kernel_->GetPanSequenceNumber();
  if (pan_sequence_number)
  tx.pan_sequence_number = utils::BcdToInt(*pan_sequence_number);

  auto card_holder_name = jspeedy_kernel_->GetCardholderName();
  if (card_holder_name)
  tx.card_holder_name = *card_holder_name;

  tx.signature_required = jspeedy_kernel_->IsSignatureRequested();

  auto transaction_certificate = jspeedy_kernel_->GetApplicationCryptogram();
  if (transaction_certificate) {
    tx.transaction_certificate = utils::BytesToHexString(
        *transaction_certificate);
  }

  is_online_pin_required_ = jspeedy_kernel_->IsOnlinePinRequested();

  tx.icc_data = jspeedy_kernel_->GetIccData(ConstructTagsForJSpeedy());

  tx.tx_datetime = *(cless_explicit_->GetTransactionDateTime());

  auto card_type = jspeedy_kernel_->GetCardType();

  if (card_type) {
    if (card_type == contactless::JSpeedyKernel::CardType::MSTRIPE)
    tx.pos_entry_mode = types::PosEntryMode::CONTACTLESS_MAGSTRIPE;
  }
}
#endif
void ContactlessWorkflow::ReadExpressPayApplicationData(Transaction& tx) {
  tx.track2_equivalent_data = expresspay_kernel_->GetTrack2EquivalentData();
  if (tx.track2_equivalent_data)
    logger::debug("ClessWorkflow - PAN from Track2: %s",
                  tx.track2_equivalent_data->GetPan().ToString().c_str());

  tx.pan = expresspay_kernel_->GetPan();
  if (tx.pan)
    logger::debug("ClessWorkflow - PAN from kernel: %s",
                  (*tx.pan).ToString().c_str());
  else {
    logger::debug("No PAN returned from kernel");
    tx.pan = tx.track2_equivalent_data->GetPan();
  }

  auto expiry_date = expresspay_kernel_->GetCardExpiryDate();
  if (expiry_date) {
    tx.expiration_date = (*expiry_date).substr(0, 4);
  }
  else
    tx.expiration_date = tx.track2_equivalent_data->GetExpirationDate();

  tx.aid = cless_explicit_->GetSelectedAid();

  auto application_preferred_name =
      expresspay_kernel_->GetApplicationPreferredName();
  if (application_preferred_name)
    tx.application_prefered_name = application_preferred_name.value();

  if (tx.application_prefered_name.empty()) {
    auto application_label = expresspay_kernel_->GetApplicationLabel();
    if (application_label)
      tx.application_prefered_name = application_label.value();
  }

  auto pan_sequence_number = expresspay_kernel_->GetPanSequenceNumber();
  if (pan_sequence_number)
    tx.pan_sequence_number = utils::BcdToInt(*pan_sequence_number);

  auto card_holder_name = expresspay_kernel_->GetCardholderName();
  if (card_holder_name)
    tx.card_holder_name = *card_holder_name;

  tx.signature_required = expresspay_kernel_->IsSignatureRequested();

  auto transaction_certificate = expresspay_kernel_->GetApplicationCryptogram();
  if (transaction_certificate) {
    tx.transaction_certificate = utils::BytesToHexString(
        *transaction_certificate);
  }

  is_online_pin_required_ = expresspay_kernel_->IsOnlinePinRequested();

  tx.icc_data = expresspay_kernel_->GetIccData(ConstructTagsForExpressPay());

  tx.tx_datetime = *(cless_explicit_->GetTransactionDateTime());
}

bool ContactlessWorkflow::IsOnlinePinRequired() {
  return is_online_pin_required_;
}

WorkflowStatus ContactlessWorkflow::Restart(Transaction& tx) {
  cless_explicit_->DetectionPrepareForRestart(TRUE);

  if (!StartDetection())
    return WorkflowStatus::CLESS_CANCELLED;

  tpcore::TeliumEvent telium_event;
  tpcore::TeliumEvent::EventType event = telium_event.WaitForEvent(
      { tpcore::TeliumEvent::EVENT_CONTACTLESS, tpcore::TeliumEvent::EVENT_KEYBOARD },
      60000);

  if (event == tpcore::TeliumEvent::EVENT_CONTACTLESS) {
    return ActivateTransaction(tx);
  }

  return WorkflowStatus::CLESS_CANCELLED;
}

WorkflowStatus ContactlessWorkflow::RunPayPassFlow(Transaction& tx) {
  paypass_kernel_ = cless_explicit_->GetPaypassKernel();

  contactless::PayPassKernel::Status status = paypass_kernel_->DoTransaction();

  types::TagValues tags = paypass_kernel_->GetAllKernelTags();
  for (const auto& tag : tags) {
    logger::debug("PayPass After - TagValue:  %X  :  %s", tag.first,
                  utils::BytesToHexString(tag.second).c_str());
  }


  WorkflowStatus flow_status = TranslateStatus(status);

  if (status == contactless::PayPassKernel::Status::OFFLINE_APPROVED) {
    logger::debug("ClessWorkflow - TX Approved Offline");
    tx.SetTransactionStatus(TransactionStatus::TO_ADVISE);
    tx.online_authorization_required = false;
    ui_.MessageApproved("Y1");
  } else if (status == contactless::PayPassKernel::Status::ONLINE_AUTH_NEEDED) {
    logger::debug("ClessWorkflow - TX Online Auth Required.");
    tx.online_authorization_required = true;
  } else if (flow_status == WorkflowStatus::CLESS_DECLINED) {
    logger::debug("ClessWorkflow - TX Declined Offline");
    tx.SetTransactionStatus(TransactionStatus::DECLINED);
    tx.online_authorization_required = false;
    ui_.MessageDeclined();
    if(tx.transaction_type == TransactionType::REFUND)
      tx.online_authorization_required = true;
    else
      tx.online_authorization_required = false;
  } else if (flow_status == WorkflowStatus::CLESS_ERROR) {
    ui_.MessageClessError();
  } else if (flow_status == WorkflowStatus::CLESS_CANCELLED) {
    ui_.MessageProcessAborted();
  } else if (flow_status == WorkflowStatus::CLESS_FALLBACK_NEEDED) {
    ui_.MessagePleaseUseOtherInterfaces();
  }
  else if (flow_status == WorkflowStatus::CLESS_MOBILE) {
    paypass_kernel_ = nullptr;
    ui_.MessagePleaseSeePhone();
    return Restart(tx);
  }
  return flow_status;
}

WorkflowStatus ContactlessWorkflow::RunPayWaveFlow(Transaction& tx) {
  paywave_kernel_ = cless_explicit_->GetPaywaveKernel();
 
  contactless::PayWaveKernel::Status status = paywave_kernel_->DoTransaction();

  types::TagValues tags = paywave_kernel_->GetAllKernelTags();
  for (const auto& tag : tags) {
    logger::debug("PayWave After - TagValue:  %X  :  %s", tag.first,
                  utils::BytesToHexString(tag.second).c_str());
  }

  WorkflowStatus flow_status = TranslateStatus(status);

  if (status == contactless::PayWaveKernel::Status::OFFLINE_APPROVED) {
    logger::debug("ClessWorkflow - TX Approved Offline");
    tx.SetTransactionStatus(TransactionStatus::TO_ADVISE);
    tx.online_authorization_required = false;
    ui_.MessageApproved("Y1");
  } else if (status == contactless::PayWaveKernel::Status::ONLINE_AUTH_NEEDED) {
    logger::debug("ClessWorkflow - TX Online Auth Required.");
    tx.online_authorization_required = true;
  } else if (flow_status == WorkflowStatus::CLESS_DECLINED) {
    logger::debug("ClessWorkflow - TX Declined Offline");
    tx.SetTransactionStatus(TransactionStatus::DECLINED);
    tx.online_authorization_required = false;
    ui_.MessageDeclined();
    if(tx.transaction_type == TransactionType::REFUND)
      tx.online_authorization_required = true;
    else
      tx.online_authorization_required = false;
  } else if (flow_status == WorkflowStatus::CLESS_ERROR) {
    ui_.MessageClessError();
  } else if (flow_status == WorkflowStatus::CLESS_CANCELLED) {
    ui_.MessageProcessAborted();
  } else if (flow_status == WorkflowStatus::CLESS_FALLBACK_NEEDED) {
    tpcore::CamDeviceGuard cam_device_guard(tpcore::CamDeviceGuard::CAM_TERMINAL);
    if (cam_device_guard.IsCardInserted() != true)
      ui_.MessagePleaseUseOtherInterfaces();
  }
  else if (flow_status == WorkflowStatus::CLESS_MOBILE) {
     paywave_kernel_ = nullptr;
     ui_.MessagePleaseSeePhone();
     return Restart(tx);
   }
  return flow_status;
}
#ifndef __TELIUM3__
WorkflowStatus ContactlessWorkflow::RunJSpeedyFlow(Transaction& tx) {
  jspeedy_kernel_ = cless_explicit_->GetJSpeedyKernel();

  contactless::JSpeedyKernel::Status status = jspeedy_kernel_->DoTransaction();

  types::TagValues tags = jspeedy_kernel_->GetAllKernelTags();
  for (const auto& tag : tags) {
    logger::debug("JSpeedy - TagValue:  %X  :  %s", tag.first,
        utils::BytesToHexString(tag.second).c_str());
  }

  WorkflowStatus flow_status = TranslateStatus(status);

  if (status == contactless::JSpeedyKernel::Status::OFFLINE_APPROVED) {
    logger::debug("ClessWorkflow - TX Approved Offline");
    tx.SetTransactionStatus(TransactionStatus::TO_ADVISE);
    tx.online_authorization_required = false;
    ui_.MessageApproved("Y1");
  } else if (status == contactless::JSpeedyKernel::Status::ONLINE_AUTH_NEEDED) {
    logger::debug("ClessWorkflow - TX Online Auth Required.");
    tx.online_authorization_required = true;
  } else if (flow_status == WorkflowStatus::CLESS_DECLINED) {
    logger::debug("ClessWorkflow - TX Declined Offline");
    tx.SetTransactionStatus(TransactionStatus::DECLINED);
    tx.online_authorization_required = false;
    ui_.MessageDeclined();
    if(tx.transaction_type == TransactionType::REFUND)
      tx.online_authorization_required = true;
    else
      tx.online_authorization_required = false;
  } else if (flow_status == WorkflowStatus::CLESS_ERROR) {
    ui_.MessageClessError();
  } else if (flow_status == WorkflowStatus::CLESS_CANCELLED) {
    ui_.MessageProcessAborted();
  } else if (flow_status == WorkflowStatus::CLESS_FALLBACK_NEEDED) {
    ui_.MessagePleaseUseOtherInterfaces();
  }
  return flow_status;
}
#endif
WorkflowStatus ContactlessWorkflow::RunExpressPayFlow(Transaction& tx) {
  expresspay_kernel_ = cless_explicit_->GetExpressPayKernel();

  contactless::ExpressPayKernel::Status status = expresspay_kernel_
      ->DoTransaction();

  types::TagValues tags = expresspay_kernel_->GetAllKernelTags();
  for (const auto& tag : tags) {
    logger::debug("ExpressPay - TagValue:  %X  :  %s", tag.first,
                  utils::BytesToHexString(tag.second).c_str());
  }

  WorkflowStatus flow_status = TranslateStatus(status);

  if (status == contactless::ExpressPayKernel::Status::OFFLINE_APPROVED) {
    logger::debug("ClessWorkflow - TX Approved Offline");
    tx.SetTransactionStatus(TransactionStatus::TO_ADVISE);
    tx.online_authorization_required = false;
    ui_.MessageApproved("Y1");
  } else if (status
      == contactless::ExpressPayKernel::Status::ONLINE_AUTH_NEEDED) {
    logger::debug("ClessWorkflow - TX Online Auth Required.");
    tx.online_authorization_required = true;
  } else if (flow_status == WorkflowStatus::CLESS_DECLINED) {
    logger::debug("ClessWorkflow - TX Declined Offline");
    tx.SetTransactionStatus(TransactionStatus::DECLINED);
    tx.online_authorization_required = false;
    ui_.MessageDeclined();
    if(tx.transaction_type == TransactionType::REFUND)
      tx.online_authorization_required = true;
    else
      tx.online_authorization_required = false;
  } else if (flow_status == WorkflowStatus::CLESS_ERROR) {
    ui_.MessageClessError();
  } else if (flow_status == WorkflowStatus::CLESS_CANCELLED) {
    ui_.MessageProcessAborted();
  } else if (flow_status == WorkflowStatus::CLESS_FALLBACK_NEEDED) {
    ui_.MessagePleaseUseOtherInterfaces();
  }
  return flow_status;
}

WorkflowStatus ContactlessWorkflow::TranslateStatus(
    contactless::PayWaveKernel::Status status) const {
  switch (status) {
    case contactless::PayWaveKernel::Status::MOBILE:
         return WorkflowStatus::CLESS_MOBILE;

    case contactless::PayWaveKernel::Status::OFFLINE_APPROVED:
      case contactless::PayWaveKernel::Status::ONLINE_AUTH_NEEDED:
      return WorkflowStatus::CLESS_SUCCESS;

    case contactless::PayWaveKernel::Status::USE_CONTACT_INTERFACE:
      return WorkflowStatus::CLESS_FALLBACK_NEEDED;

    case contactless::PayWaveKernel::Status::CANCELLED:
      return WorkflowStatus::CLESS_CANCELLED;

    case contactless::PayWaveKernel::Status::OFFLINE_DECLINED:
      return WorkflowStatus::CLESS_DECLINED;

    case contactless::PayWaveKernel::Status::CARD_BLOCKED:
    case contactless::PayWaveKernel::Status::CARD_ERROR:
    case contactless::PayWaveKernel::Status::COMM_ERROR:
    case contactless::PayWaveKernel::Status::INTERNAL_ERROR:
    case contactless::PayWaveKernel::Status::USE_NEXT_AID:
    default:
      return WorkflowStatus::CLESS_ERROR;
  }
}

WorkflowStatus ContactlessWorkflow::TranslateStatus(
    contactless::PayPassKernel::Status status) const {
    switch (status) {
    case contactless::PayPassKernel::Status::MOBILE:
      return WorkflowStatus::CLESS_MOBILE;

      case contactless::PayPassKernel::Status::OFFLINE_APPROVED:
        case contactless::PayPassKernel::Status::ONLINE_AUTH_NEEDED:
        return WorkflowStatus::CLESS_SUCCESS;

      case contactless::PayPassKernel::Status::USE_CONTACT_INTERFACE:
        return WorkflowStatus::CLESS_FALLBACK_NEEDED;

      case contactless::PayPassKernel::Status::CANCELLED:
        return WorkflowStatus::CLESS_CANCELLED;

      case contactless::PayPassKernel::Status::OFFLINE_DECLINED:
        return WorkflowStatus::CLESS_DECLINED;

      case contactless::PayPassKernel::Status::CARD_BLOCKED:
      case contactless::PayPassKernel::Status::CARD_ERROR:
      case contactless::PayPassKernel::Status::COMM_ERROR:
      case contactless::PayPassKernel::Status::INTERNAL_ERROR:
      case contactless::PayPassKernel::Status::USE_NEXT_AID:
      default:
        return WorkflowStatus::CLESS_ERROR;
    }
}
#ifndef __TELIUM3__
WorkflowStatus ContactlessWorkflow::TranslateStatus(
    contactless::JSpeedyKernel::Status status) const {
    switch (status) {
      case contactless::JSpeedyKernel::Status::OFFLINE_APPROVED:
    case contactless::JSpeedyKernel::Status::ONLINE_AUTH_NEEDED:
    return WorkflowStatus::CLESS_SUCCESS;

      case contactless::JSpeedyKernel::Status::USE_CONTACT_INTERFACE:
    return WorkflowStatus::CLESS_FALLBACK_NEEDED;

      case contactless::JSpeedyKernel::Status::CANCELLED:
    return WorkflowStatus::CLESS_CANCELLED;

      case contactless::JSpeedyKernel::Status::OFFLINE_DECLINED:
    return WorkflowStatus::CLESS_DECLINED;

      case contactless::JSpeedyKernel::Status::CARD_BLOCKED:
    case contactless::JSpeedyKernel::Status::CARD_ERROR:
    case contactless::JSpeedyKernel::Status::COMM_ERROR:
    case contactless::JSpeedyKernel::Status::INTERNAL_ERROR:
    case contactless::JSpeedyKernel::Status::USE_NEXT_AID:
    default:
    return WorkflowStatus::CLESS_ERROR;
    }
}
#endif
WorkflowStatus ContactlessWorkflow::TranslateStatus(
    contactless::ExpressPayKernel::Status status) const {
    switch (status) {
      case contactless::ExpressPayKernel::Status::OFFLINE_APPROVED:
      case contactless::ExpressPayKernel::Status::ONLINE_AUTH_NEEDED:
      case contactless::ExpressPayKernel::Status::ONLINE_APPROVED:
        return WorkflowStatus::CLESS_SUCCESS;

      case contactless::ExpressPayKernel::Status::USE_CONTACT_INTERFACE:
        return WorkflowStatus::CLESS_FALLBACK_NEEDED;

      case contactless::ExpressPayKernel::Status::CANCELLED:
        return WorkflowStatus::CLESS_CANCELLED;

      case contactless::ExpressPayKernel::Status::OFFLINE_DECLINED:
        return WorkflowStatus::CLESS_DECLINED;

      case contactless::ExpressPayKernel::Status::CARD_BLOCKED:
      case contactless::ExpressPayKernel::Status::CARD_ERROR:
      case contactless::ExpressPayKernel::Status::COMM_ERROR:
      case contactless::ExpressPayKernel::Status::INTERNAL_ERROR:
      case contactless::ExpressPayKernel::Status::USE_NEXT_AID:
      default:
        return WorkflowStatus::CLESS_ERROR;
    }
  }

}

