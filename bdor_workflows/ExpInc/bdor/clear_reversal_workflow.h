/*
 * clear_reversal_workflow.h
 *
 *  Created on: Nov 3, 2016
 *      Author: RickyChan
 */

#ifndef EXPINC_FDMS_CLEAR_REVERSAL_WORKFLOW_H_
#define EXPINC_FDMS_CLEAR_REVERSAL_WORKFLOW_H_

#include <bdor/application_settings.h>
#include <bdor/batch.h>
#include <bdor/ui.h>
#include <bdor/host.h>

namespace bdor {

class ClearReversalWorkFlow {


	public:
		ClearReversalWorkFlow(ApplicationSettings& app_setting,
		           	   	      Batch& batch,
							  Ui& ui)
			:app_setting_(app_setting),
		     batch_(batch),
			 ui_(ui){
		}

		bool Start();

	private:
		ApplicationSettings& app_setting_;
		Batch& batch_;
		Ui& ui_;

};

}



#endif /* EXPINC_FDMS_CLEAR_REVERSAL_WORKFLOW_H_ */
