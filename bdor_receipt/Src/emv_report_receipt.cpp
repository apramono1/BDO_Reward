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
#include "emv_report_receipt.h"

#include <sstream>
#include <iomanip>
#include <stdx/memory>
#include "receipt_header.h"
#include "receipt_formatter.h"

namespace bdor {

EmvReportReceipt::EmvReportReceipt(tpcore::GoalHandle& gl,
                                   unsigned printer_timeout)
    : GoalCompositeReceipt(gl, printer_timeout) {
  Add(stdx::make_unique<ReceiptHeader>(gl_, printer_timeout));
  Add(stdx::make_unique<Body>(gl_, printer_timeout));
}

EmvReportReceipt::Body::Body(tpcore::GoalHandle& gl, unsigned printer_timeout)
    : GoalReceipt(gl, printer_timeout) {
}

void EmvReportReceipt::Body::Expand(const Receipt::Map& dict) {

  T_GL_HWIDGET layout = GetGoalRootLayout();

  T_GL_DIM line = 0;
  std::string text;

  //DATE and TIME
  text = "DATE: " + GetValue(dict, "DATE");
  SetPrintData(layout, text.c_str(), line, GL_ALIGN_LEFT, 25, 25);
  text = "TIME: " + GetValue(dict, "TIME");
  SetPrintData(layout, text.c_str(), line++, GL_ALIGN_RIGHT, 25, 25);

  //TID and MID
  text = "MID: " + GetValue(dict, "MID");
  SetPrintData(layout, text.c_str(), line, GL_ALIGN_LEFT, 25, 25);
  text = "TID: " + GetValue(dict, "TID");
  SetPrintData(layout, text.c_str(), line++, GL_ALIGN_RIGHT, 25, 25);

  SetPrintData(layout, "EMV DIAGNOSTIC REPORT", line++, GL_ALIGN_CENTER, 20,
               20);

  T_GL_DIM newLine = line;

  //Application Interchange Profile
  newLine = PrintTagLengthValue(dict, layout, newLine, "APPLI_INTCHG_PROF",
                                "82 ");

  //Issuer Script Command
  newLine = PrintTagLengthValue(dict, layout, newLine, "ISSUER_SCRIPT_CMD",
                                "86 ");

  //Auth Response Code
  newLine = PrintTagLengthValue(dict, layout, newLine, "AUTH_RESP_CODE", "8A ");

  //CVM List
  newLine = PrintTagLengthValue(dict, layout, newLine, "CVM_LIST", "8E ");

  //CA Public Key Index
  newLine = PrintTagLengthValue(dict, layout, newLine, "CA_PK_INDEX(ICC)",
                                "8F ");

  //Issuer Authentication Data
  newLine = PrintTagLengthValue(dict, layout, newLine, "ISS_AUTH_DATA", "91 ");

  //Application File Locator
  newLine = PrintTagLengthValue(dict, layout, newLine, "AFL", "94 ");

  //Terminal Verification Results
  newLine = PrintTagLengthValue(dict, layout, newLine, "TVR", "95 ");

  //Transaction Status Information
  newLine = PrintTagLengthValue(dict, layout, newLine, "TSI", "9B ");

  //Issuer Application Data
  newLine = PrintTagLengthValue(dict, layout, newLine, "ISSUER_APP_DATA",
                                "9F10 ");

  //Transaction Type
  newLine = PrintTagLengthValue(dict, layout, newLine, "TRANSACTION_TYPE",
                                "9C ");

  //Application Transaction Counter
  newLine = PrintTagLengthValue(dict, layout, newLine, "ATC", "9F36 ");

  //Cryptogram Information Data
  newLine = PrintTagLengthValue(dict, layout, newLine, "CRYPT_INFO_DATA",
                                "9F27 ");

  //CVM Results
  newLine = PrintTagLengthValue(dict, layout, newLine, "CVM_RESULTS", "9F34 ");

  //Issuer TC
  newLine = PrintTagLengthValue(dict, layout, newLine, "ISSUER TC", "9F26 ");

  //Unpredictable Number
  newLine = PrintTagLengthValue(dict, layout, newLine, "UNPREDICTABLE_NUMBER",
                                "9F37 ");

  //Transaction Date
  newLine = PrintTagLengthValue(dict, layout, newLine, "TRANSACTION DATE",
                                "9A ");

  //Amount, Authorized
  newLine = PrintTagLengthValue(dict, layout, newLine, "AMOUNT, AUTHORIZED",
                                "9F02 ");

  //Transaction Currency Code
  newLine = PrintTagLengthValue(dict, layout, newLine,
                                "TRANSACTION_CURRENCY CODE",
                                "5F2A ");

  //PAN - Truncated
  newLine = PrintTagLengthValue(dict, layout, newLine, "PAN - TRUNCATED",
                                "5A ");

  //Terminal Country Code
  newLine = PrintTagLengthValue(dict, layout, newLine, "TERMINAL COUNTRY CODE",
                                "9F1A ");

  //Amount, Other
  newLine = PrintTagLengthValue(dict, layout, newLine, "AMOUNT, OTHER",
                                "9F03 ");

  //PAN Sequence Number
  newLine = PrintTagLengthValue(dict, layout, newLine, "PAN_SEQUENCE_NUMBER",
                                "5F34 ");

  //TTQ
  newLine = PrintTagLengthValue(dict, layout, newLine, "TTQ", "9F66 ");

  //CTQ
  newLine = PrintTagLengthValue(dict, layout, newLine, "CTQ", "9F60 ");

  //AOSA
  newLine = PrintTagLengthValue(dict, layout, newLine, "AOSA", "9F5D ");

  SetPrintData(layout, "END OF REPORT\n\n\n", newLine++, GL_ALIGN_CENTER, 20,
               20);

  //end of receipt
}

int EmvReportReceipt::Body::PrintTagLengthValue(
    const receipt::Receipt::Map& dict,
    T_GL_HWIDGET layout,
    T_GL_DIM newLine,
    const char* name,
    const char* tag) {

  std::string data;
  std::string taglength;
  std::string value_string;
  std::string value;
  std::string length;
  uint8_t i = 0;

  SetPrintData(layout, name, newLine, GL_ALIGN_LEFT, 20, 20);
  data = GetValue(dict, name);
  length = data.substr(0, 2);
  taglength = tag + length;
  SetPrintData(layout, taglength.c_str(), newLine++, GL_ALIGN_RIGHT, 20, 20);

  if (!length.size()) {
    SetPrintData(layout, "No Data", newLine++, GL_ALIGN_LEFT, 20, 20);
    return newLine;
  }

  value_string = data.substr(2, 2 * (data.size() - length.size()));
  value = FormatTagValue(value_string);

  while (i < value.size()) {
    std::string temp;
    temp = value.substr(i, kMaxLineLength);
    SetPrintData(layout, temp.c_str(), newLine++, GL_ALIGN_LEFT, 20, 20);
    i += kMaxLineLength;
  }

  return newLine;
}

std::string EmvReportReceipt::Body::FormatTagValue(const std::string& data) {
  std::ostringstream oss;

  if (data.size() > 0) {
    for (size_t i = 0; i < data.size(); i = i + 2)
      oss << data[i] << data[i + 1] << std::setfill(' ') << std::setw(2);
  }
  return oss.str();
}

}
