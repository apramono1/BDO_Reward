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
#ifndef BDOR__GOAL_RES_H_
#define BDOR__GOAL_RES_H_

#include <string>

#include "GL_GraphicLib.h"

namespace bdor {

const char* const RES_CLESS_LOGO__SCREEN = "ClessLogoWithMessage";

const char* const RES_APPLI_TYPE = "CF59";

const char* const RES_PIN_ENTRY__SCREEN = "pinEntryScreen";
const T_GL_ID PIN_ENTRY__TX_TYPE = 6;
const T_GL_ID PIN_ENTRY__AMOUNT = 7;
const T_GL_ID PIN_ENTRY__MAIN_MESSAGE = 4;
const T_GL_ID PIN_ENTRY__MASK = 5;
const T_GL_ID PIN_ENTRY__HELP_MESSAGE = 8;

const char* const RES_TEST_HOST__SCREEN = "testHostScreen";
const T_GL_ID TEST_HOST__PROGRESS_MESSAGE = 9;
const T_GL_ID TEST_HOST__NB_ERROR_MESSAGE = 10;

const char* const RES_TX_REVIEW_SCREEN = "ReviewTx";
const T_GL_ID REVIEW_TX_TYPE = 11;
const T_GL_ID REVIEW_CARD_TYPE = 12;
const T_GL_ID REVIEW_MASKED_PAN = 13;
const T_GL_ID REVIEW_MASKED_EXPIRY_DATE = 14;
const T_GL_ID REVIEW_TX_AMOUNT = 15;
const T_GL_ID REVIEW_AUTH_CODE = 16;
const T_GL_ID REVIEW_TX_STAN = 17;
const T_GL_ID REVIEW_TX_TIMEOUT_TIMER = 18;

const char* const RES_BATCH_TOTAL__SCREEN = "batchTotal";
const T_GL_ID BATCH_TOTAL_HOST_NAME = 19;
const T_GL_ID BATCH_TOTAL_SALES_TOTAL = 20;
const T_GL_ID BATCH_TOTAL_REFUND_TOTAL = 21;
const T_GL_ID BATCH_TOTAL_GRAND_TOTAL = 22;
const T_GL_ID BATCH_TOTAL_SINGLE_LAYOUT = 23;
const T_GL_ID BATCH_TOTAL_TIMEOUT_TIMER = 24;
const T_GL_ID BATCH_TOTAL_SALE_COUNT = 25;
const T_GL_ID BATCH_TOTAL_REFUND_COUNT = 26;
const T_GL_ID BATCH_TOTAL_TOTAL_COUNT = 27;

std::string GetDAL(const char* resource);

}

#endif

