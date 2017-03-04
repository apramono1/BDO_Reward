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
#ifndef BDOR__RECEIPT_UTIL_H_
#define BDOR__RECEIPT_UTIL_H_

#include <receipt/goal_receipt.h>
#include "receipt_formatter.h"
#include "receipt/receipt.h"
#include <bdor/application_settings.h>

namespace bdor {

void SetPrintDataLineFeed(T_GL_HWIDGET layout,
                          T_GL_DIM& line);

void SetPrintDataTxnTypeDebit(const receipt::Receipt::Map& dict,
                              T_GL_HWIDGET layout,
                              T_GL_DIM& line);

void SetPrintDataTxnTypeCredit(const receipt::Receipt::Map& dict,
                               T_GL_HWIDGET layout,
                               T_GL_DIM& line);

void SetPrintDataTransactionDetails(const receipt::Receipt::Map& dict,
                                    T_GL_HWIDGET layout,
                                    T_GL_DIM& line);

void SetPrintDataDccInformation(const receipt::Receipt::Map& dict,
                                T_GL_HWIDGET layout,
                                T_GL_DIM& line);

void SetPrintDataAmountsWithTip(
    const receipt::Receipt::Map& dict,
    TerminalConfiguration::TipProcessingMode tip_mode,
    T_GL_HWIDGET layout,
    T_GL_DIM& line);

void SetPrintDataTotalAmountOnly(const receipt::Receipt::Map& dict,
                                 T_GL_HWIDGET layout,
                                 T_GL_DIM& line);

void SetPrintDataAmountsForVoid(
    const receipt::Receipt::Map& dict,
    TerminalConfiguration::TipProcessingMode tip_mode,
    T_GL_HWIDGET layout,
    T_GL_DIM& line);

void SetPrintDataAmountsForTipAdjust(
                                     const receipt::Receipt::Map& dict,
                                     T_GL_HWIDGET layout,
                                     T_GL_DIM& line);

void SetPrintDataTenureInformation(const receipt::Receipt::Map& dict,
                                   T_GL_HWIDGET layout,
                                   T_GL_DIM& line);

void SetPrintDataHostResponseText(const receipt::Receipt::Map& dict,
                                  T_GL_HWIDGET layout,
                                  T_GL_DIM& line);
}

#endif
