/*
 ------------------------------------------------------------------------------
 INGENICO Technical Software Department
 ------------------------------------------------------------------------------
 Copyright (c) 2014-2016 INGENICO S.A.
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
#ifndef BDOR__RECEIPT_SETTLEMENT_DATA_TYPES_H_
#define BDOR__RECEIPT_SETTLEMENT_DATA_TYPES_H_

#include <bdor/batch_data_types.h>

namespace bdor {
enum class SettlementStatus{
  NONE,
  SUCCESS,
  FAILED
};
enum class HostReportType{
  SETTLEMENT_REPORT,
  SUMMARY_REPORT,
  NONE
};
struct SingleHostSettlementData {
  BatchTotalsReport report;
  time_t settlement_datetime = 0;
  bool batch_upload_performed = false;
  HostReportType report_type = HostReportType::SETTLEMENT_REPORT;
  SettlementStatus settlement_status = SettlementStatus::NONE;
};

struct AllHostsSettlementData {
  std::vector<SingleHostSettlementData> host_report_list;
  GrandBatchTotalsForReport global_totals;
};

}

#endif
