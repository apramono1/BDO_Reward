/*
 ------------------------------------------------------------------------------
 INGENICO Technical Software Department
 ------------------------------------------------------------------------------
 Copyright (c) 2014 INGENICO S.A.
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
#include <bdor/goal_receipts.h>
#include <bdor/receipts.h>
#include <bdor/application_settings.h>
#include <bdor/cvm_type.h>
#include <bdor/copy_type.h>
#include <bdor/reprint_type.h>

receipt::PrintStatus TestPrintInstalmentSale(
    bdor::CopyType copy, bdor::Transaction& txn1, bdor::Receipts& receipts,
    bdor::ApplicationSettings& app_settings);
