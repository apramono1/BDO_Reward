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
#include "lounge_usage.h"

#include <iostream>
#include <bdor/summary_report_workflow.h>
#include <bdor/ui.h>
#include <bdor/host_switch.h>
#include <bdor/acquirer.h>

namespace bdor {
LoungeUsage::LoungeUsage(Ui& ui, Acquirer& acquirer): ui_(ui), acquirer_(acquirer){};
bool LoungeUsage::OpenLoungeUsageMenu(Transaction& tx) {
  return ui_.DispatchMenu("LOUNGE USAGE", selection_menu1_);
  selection_menu1_.push_back(
         std::make_pair("YES",
                       [&]() {return ProcessValidCard(tx, ui_);}));
  selection_menu1_.push_back(
  		 std::make_pair("NO",
  				       [&]() {tx.SetTransactionStatus(TransactionStatus::CANCELLED); return true;}));
}

bool LoungeUsage::ProcessValidCard(Transaction& tx, Ui& ui_){
  ui_.MessageCountVisit(tx.field63_data);
  if(tx.response_code == "23"){
	  ui_.DisplayResponseMessage(tx.response_code);
	  ProceedToExcessVisit(tx, ui_);
  }
  ProceedToWithinLimitVisit(tx, ui_);
  return true;
}

void LoungeUsage::ProceedToExcessVisit(Transaction& tx, Ui& ui_){
	ui_.DispatchMenu("EXCESS VISIT", guest_number_menu_);
	guest_number_menu_.push_back(
	        std::make_pair("CARDHOLDER ONLY", [&]()
	        		{return ui_.DispatchMenu("PAYMENT OPTION", payment_option_menu_);}));
	guest_number_menu_.push_back(
		    std::make_pair("CARDHOLDER WITH 1 GUEST", [&]()
		        	{return ui_.DispatchMenu("PAYMENT OPTION", payment_option_menu_);}));
	guest_number_menu_.push_back(
			std::make_pair("CARDHOLDER WITH MORE THAN 1 GUEST", [&]()
			        {return ui_.DispatchMenu("PAYMENT OPTION", payment_option_menu_);}));

	payment_option_menu_ = {
	  	    { "CASH              ", [&]() {return acquirer_.StartSale();}},
	  	    { "DEBIT             ", [&]() {return acquirer_.StartSale();}},
	  	    { "CREDIT            ", [&]() {return acquirer_.StartSale();}},
	  	    { "REDEMPTION        ", [&]() {return acquirer_.StartSale();}}
	};

}

void LoungeUsage::ProceedToWithinLimitVisit(Transaction& tx, Ui& ui_){
	ui_.DispatchMenu("LOUNGE USAGE", guest_number_menu_);
	guest_number_menu_.push_back(
		    std::make_pair("CARDHOLDER ONLY", [&]()
		        		{tx.SetTransactionStatus(TransactionStatus::APPROVED); return true;}));
	guest_number_menu_.push_back(
			std::make_pair("CARDHOLDER WITH 1 GUEST", [&]()
			        	{tx.SetTransactionStatus(TransactionStatus::APPROVED); return true;}));
	guest_number_menu_.push_back(
		    std::make_pair("CARDHOLDER WITH MORE THAN 1 GUEST", [&]()
				        {ui_.MessageExcessGuest(); return ui_.DispatchMenu("SELECTION CHOICE", selection_menu2_);}));
	selection_menu2_ = {
			{ "YES               ", [&]() {return ui_.DispatchMenu("PAYMENT OPTION", payment_option_menu_);}},
			{ "NO                ", [&]() {tx.SetTransactionStatus(TransactionStatus::CANCELLED); return true;}}
	};

	payment_option_menu_ = {
	  	    { "CASH              ", [&]() {return acquirer_.StartSale();}},
	  	    { "DEBIT             ", [&]() {return acquirer_.StartSale();}},
	  	    { "CREDIT            ", [&]() {return acquirer_.StartSale();}},
	  	    { "REDEMPTION        ", [&]() {return acquirer_.StartSale();}}
	};

}

}
