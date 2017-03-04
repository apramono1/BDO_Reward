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
#ifndef BDOR__BATCH_DATA_TYPES_H_
#define BDOR__BATCH_DATA_TYPES_H_

#include <types/currency.h>
#include <bdor/transaction.h>

namespace bdor {

struct BatchTotal {
  unsigned int count = 0;
  std::uint64_t total = 0;

  BatchTotal(unsigned int count_, std::uint64_t total_)
      : count(count_),
        total(total_) {
  }

  BatchTotal()
      : count(0),
        total(0) {
  }

  BatchTotal& operator+=(const BatchTotal& rhs) {
    count += rhs.count;
    total += rhs.total;
    return *this;
  }

};

struct BatchTotalsForHost {
  types::Currency currency;
  BatchTotal sales_total;
  BatchTotal refunds_total;
};

struct BatchTotalsForReport {
  std::string card_type;
  types::Currency currency;
  BatchTotal sales_total;
  BatchTotal tips_total;
  BatchTotal refunds_total;
  BatchTotal offline_sales_total;
  BatchTotal void_sales_total;
  BatchTotal void_refunds_total;
  BatchTotal card_total;
};

struct GrandBatchTotalsForReport {
  types::Currency currency;
  BatchTotal sales_total;
  BatchTotal tips_total;
  BatchTotal refunds_total;
  BatchTotal offline_sales_total;
  BatchTotal void_sales_total;
  BatchTotal void_refunds_total;
  BatchTotal total;
};

struct BatchTotalsReport {
  unsigned int host_index;
  int batch_number;
  std::vector<BatchTotalsForReport> batch_totals_list;
  GrandBatchTotalsForReport global_batch_totals;
};

struct AllHostBatchTotalsReport {
  std::vector<BatchTotalsReport> batch_totals_report_list;
  GrandBatchTotalsForReport global_batch_totals;
};

struct DetailReportSingleHost {
	unsigned int host_index;
	int batch_number;
	int transaction_count;
	std::vector<Transaction> transaction_list;
	std::vector<BatchTotalsForReport> issuer_totals_list;
	BatchTotalsForReport host_total;
};

}

#endif
