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
#ifndef BDOR__CARD_DEFINITION_H_
#define BDOR__CARD_DEFINITION_H_

#include <registry/record.h>

namespace bdor {

class CardDefinition : public registry::Record {
 public:

  typedef std::unique_ptr<CardDefinition> uptr;

  virtual std::vector<registry::Mapper::uptr> BuildDataMap();
  virtual registry::Record::uptr GetNewInstance() const;

  enum CvvMode {
    CVV_NOPROMPT = 0,
    CVV_PROMPT_AND_ALLOW_BYPASS = 1,
    CVV_PROMPT_AND_NO_BYPASS = 2
  };

  std::uint64_t index;

  std::string pan_low_range;
  std::string pan_high_range;
  std::uint64_t pan_min_length;
  std::uint64_t pan_max_length;

  std::uint64_t host_index;
  std::uint64_t moto_host_index;
  std::uint64_t issuer_index;

  std::uint64_t floor_limit;
  std::uint64_t moto_floor_limit;
  std::uint64_t qps_floor_limit;

  CvvMode cvv_manual_entry;
  CvvMode cvv_moto_sale;
  CvvMode cvv_magstripe_entry;

  bool tx_sale_enabled;
  bool tx_preauth_enabled;
  bool tx_offline_sale_enabled;
  bool tx_refund_enabled;
  bool tx_void_enabled;
  bool tx_void_preauth_enabled;
  bool tx_preauth_completion_online_enabled;
  bool tx_preauth_completion_offline_enabled;
  bool tx_cash_advance_enabled;
  bool tx_moto_sale_enabled;
  bool tx_moto_refund_enabled;
  bool tx_moto_preauth_enabled;
  bool tx_instalment_sale_enabled;
  bool manual_entry_enabled;
  bool luhn_check_required;
  bool dcc_enabled;
};

}

#endif
