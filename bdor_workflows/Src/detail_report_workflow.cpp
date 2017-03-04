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
#include <bdor/detail_report_workflow.h>
#include <tpcore/telium_event.h>
#include <stdx/string>
#include <utils/logger.h>
#include <idalloc/idalloc.h>
#include <bdor/application_settings.h>
#include "workflow_activities.h"
#include "app_counter.h"

namespace bdor {

DetailReportWorkflow::DetailReportWorkflow(ApplicationSettings& app_setting,
                                       Batch& batch,
                                       Ui& ui,
                                       Receipts& receipts)
    : app_setting_(app_setting),
      batch_(batch),
      ui_(ui),
      receipts_(receipts) {
}

void DetailReportWorkflow::Start() {
	logger::debug("Starting Detail Report");

	auto host_list = app_setting_.managed_settings_->GetAllHostDefinitions();
	std::vector<std::string> host_name_list;

	for (const auto& i : host_list)
		host_name_list.push_back(i.host_name);

	unsigned int pos = 0;
	if (!ui_.SelectHost(host_name_list, pos)) {
		return;
	}
	auto& host_config = host_list.at(pos);

	DetailReportSingleHost detail_report = batch_.GetBatchDetailTransactions(host_config.index);

	if (detail_report.transaction_list.empty()) {
		ui_.MessageDisplayBatchEmpty(host_config.host_name);
		return;
	}

	auto receipt = receipts_.GetDetailReportReceipt(app_setting_, detail_report);
	PrintLoop(ui_, *receipt);

}

}

