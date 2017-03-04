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

#include <bdor/goal_ui.h>

#include <stdx/ctime>
#include <stdx/memory>
#include <stdx/string>
#include <utils/strings.h>
#include <tpcore/telium_keyboard_guard.h>
#include "pin_entry_ui.h"
#include "goal_res.h"

namespace bdor {

namespace {

const std::map<const std::string, const std::string> kResponseCodeTable = {
    { "00", "APPROVED" },
    { "01", "REFER TO CARD ISSUER" },
    { "02", "REFER TO CARD ISSUER, SPECIAL CONDITION" },
    { "03", "INVALID MERCHANT" },
    { "04", "PICK UP CARD" },
    { "05", "DO NOT HONOUR" },
    { "06", "ERROR" },
    { "07", "PICK UP CARD, SPECIAL CONDITION" },
    { "08", "HONOUR WITH IDENTIFICATION" },
    { "09", "REQUEST IN PROGRESS" },
    { "10", "APPROVED, PARTIAL" },
    { "11", "APPROVED, VIP" },
    { "12", "INVALID TRANSACTION" },
    { "13", "INVALID AMOUNT" },
    { "14", "INVALID CARD NUMBER" },
    { "15", "NO SUCH ISSUER" },

    { "19", "RE-ENTER TRANSACTION" },
    { "20", "INVALID RESPONSE" },
    { "21", "CARD NOT INITIALIZED" },
    { "22", "CARD IS NOT VALID FREE ACCESS DENIED CALL 631-8000" },
	{ "23", "FREE ACCESS EXCEEDED PROCEED TO PAY?" },

    { "25", "UNABLE TO LOCATE ORIGINAL TRANSACTION" },

    { "30", "FORMAT ERROR" },
    { "31", "BANK NOT SUPPORTED" },
    { "32", "EXPIRED CARD, PICK UP" },
    { "33", "SUSPECTED FRAUD, PICK UP" },
    { "34", "FRAUD" },

    { "36", "RESTRICTED CARD, PICK UP" },

    { "38", "PIN TRIES EXCEEDED" },
    { "39", "NO CREDIT ACCOUNT" },
    { "40", "FUNCTION NOT SUPPORT" },
    { "41", "LOST CARD" },
    { "42", "NO UNIVERSAL ACCOUNT" },
    { "43", "STOLEN CARD" },

    { "45", "FALLBACK NOT ALLOWED" },

    { "51", "NOT SUFFICIENT FUNDS" },
    { "52", "NO CHECK ACCOUNT" },
    { "53", "NO SAVINGS ACCOUNT" },
    { "54", "EXPIRED CARD" },
    { "55", "INCORRECT PIN" },
    { "56", "NO CARD RECORD" },
    { "57", "TRANSACTION NOT PERMITTED TO CARDHOLDER" },
    { "58", "TRANSACTION NOT PERMITTED ON TERMINAL" },
    { "59", "SUSPECTED FRAUD" },

    { "61", "EXCEEDS WITHDRAWAL LIMIT" },
    { "62", "RESTRICTED CARD" },
    { "63", "SECURITY VIOLATION" },
    { "64", "ORIGINAL AMOUNT INCORRECT" },
    { "65", "EXCEEDS WITHDRAWAL FREQUENCY" },

    { "68", "ISSUER REPONSE TIMED OUT" },

    { "75", "PIN TRIES EXCEEDED" },

    { "77", "INTERVENE, BANK APPROVAL REQUIRED" },
    { "78", "INTERVENE, BANK APPROVAL REQUIRED" },
    { "79", "BATCH OPEN # 79" },

    { "85", "NOT DECLINED" },

    { "90", "CUT OFF IN PROGRESS" },
    { "91", "ISSUER OR SWITCH INOPERATIVE" },
    { "92", "ROUTING ERROR" },

    { "94", "DUPLICATE TRANSMISSION" },
    { "95", "RECONCILE ERROR" },
    { "96", "SYSTEM ERROR" },

    { "99", "PIN BLOCK ERROR" },

    { "D1", "INCORRECT PIN" },
    { "D2", "DATE ERROR" },
    { "D8", "UNSUCCESSFUL" },

    { "Y1", "OFFLINE APPROVED 1" },
    { "Y3", "OFFLINE APPROVED 3" },
    { "Z1", "OFFLINE DECLINED 1" },
    { "Z3", "OFFLINE DECLINED 3" },
};

const std::string kUnknownRespondeCodeMessage("DECLINED - ");

std::string GetResponseCodeString(const std::string& code) {
  auto it = kResponseCodeTable.find(code);
  return
  (it != kResponseCodeTable.cend()) ?
                                      (code + " - " + it->second) :
                                      (kUnknownRespondeCodeMessage + code);
}

}

GoalUi::GoalUi(tpcore::GoalHandle& gl)
    : gl_(gl) {
}

unsigned long GoalUi::MenuSelectionIndex(const char* title,
                                         const std::vector<std::string> &menu) {

  // make sure the keyboard is active for the menu
  tpcore::TeliumKeyboardGuard keyboard_guard;

  // fill char buffer with shape required by GL_Dialog_Menu
  // - sequence of pointers to null terminated strings
  // - single null pointer to mark end of menu
  auto array = stdx::make_unique<const char *[]>(menu.size() + 1);
  auto ptr = array.get();
  for (auto menu_item : menu)
    *ptr++ = menu_item.c_str();
  *ptr = nullptr;

  return GL_Dialog_Menu(gl_.GetHandle(), title, array.get(), 0,
                        GL_BUTTON_CANCEL,
                        GL_KEY_1,
                        screen_timeout);
}

std::string GoalUi::MenuSelection(const char* title,
                                  const std::vector<std::string> &menu) {

  auto item_number = MenuSelectionIndex(title, menu);

  if (item_number == GL_KEY_CANCEL || item_number == GL_RESULT_INACTIVITY) {
    MessageProcessAborted();
    return std::string();
  }

  return menu[item_number];
}

bool GoalUi::RequestInvoice(uint32_t& invoice_no) {
  std::string inv_str;

  inv_str.resize(7);
  tpcore::TeliumKeyboardGuard keyboard_guard;
  ulong return_key;
  do {
    return_key = GL_Dialog_Text(gl_.GetHandle(), txn_type_title_,
                                "Enter Invoice:",
                                "/d/d/d/d/d/d",
                                (char*) inv_str.c_str(),
                                inv_str.size(),
                                screen_timeout);
    if (return_key != GL_KEY_VALID) {
      MessageProcessAborted();
      return false;
    }
  } while (strlen(inv_str.c_str()) == 0);

  inv_str.resize(strlen(inv_str.c_str()));

  invoice_no = utils::FromString<unsigned int>(inv_str);

  return true;
}

bool GoalUi::RequestTraceNumber(uint32_t& stan_no) {
  std::string stan_str;

  stan_str.resize(7);
  tpcore::TeliumKeyboardGuard keyboard_guard;
  ulong return_key;
  do {
    return_key = GL_Dialog_Text(gl_.GetHandle(), txn_type_title_,
                                "Enter Trace Number:",
                                "/d/d/d/d/d/d",
                                (char*) stan_str.c_str(),
                                stan_str.size(),
                                screen_timeout);

    if (return_key != GL_KEY_VALID) {
      MessageProcessAborted();
      return false;
    }
  } while (strlen(stan_str.c_str()) == 0);

  stan_str.resize(strlen(stan_str.c_str()));

  stan_no = utils::FromString<unsigned int>(stan_str);

  return true;
}

bool GoalUi::RequestTid(std::string &terminal_id) {
  terminal_id.resize(9);
  tpcore::TeliumKeyboardGuard keyboard_guard;

  for (;;) {
    ulong return_key = GL_Dialog_Text(gl_.GetHandle(), "TERMINAL ID",
                                      "Enter TID:",
                                      "/d/d/d/d/d/d/d/d",
                                      (char*) terminal_id.c_str(),
                                      terminal_id.size(), screen_timeout);
    if (return_key != GL_KEY_VALID)
      return false;

    if (strlen(terminal_id.c_str()) == 8)
      break;
    GL_Dialog_Message(gl_.GetHandle(), NULL,
                      "Please Enter 8 Digits Terminal ID.",
                      GL_ICON_WARNING,
                      GL_BUTTON_NONE,
                      msg_duration_);
  }

  terminal_id.resize(strlen(terminal_id.c_str()));
  return true;
}

bool GoalUi::RequestAmount(types::Amount& amount) {
  return RequestAmount(amount, "Enter Amount:", false);
}

bool GoalUi::RequestTipAmount(types::Amount& amount) {
  return RequestAmount(amount, "Tip Amount:", true);
}

bool GoalUi::RequestPreauthAmount(types::Amount& amount) {
  return RequestAmount(amount, "Enter Pre-auth Amount:", false);
}

bool GoalUi::RequestPreAuthCompletionAmount(types::Amount& amount) {
  return RequestAmount(amount, "Enter New Amount:", false);
}

bool GoalUi::RequestAmount(types::Amount& amount, const char* message,
                           bool is_zero_amount_allowed) {
  tpcore::TeliumKeyboardGuard keyboard_guard;
  char amount_buf[12 + 1] = { '\0' };
  std::string mask_str = "/d/d/d/d/d/d/d/d/d/d/d/d";

  int decimal_place = amount.GetDecimalShift();

  if (decimal_place != 0) {
    mask_str.erase(12 - (decimal_place + 1), 12);
    mask_str.append("/D.");
    for (int i = 0; i < decimal_place; i++)
      mask_str.append("/D");
  }

  do {
    if (GL_Dialog_Amount(gl_.GetHandle(), txn_type_title_, message,
                         mask_str.c_str(),
                         amount_buf, sizeof(amount_buf),
                         amount.GetCurrencyName().c_str(),
                         GL_ALIGN_LEFT,
                         screen_timeout) != GL_KEY_VALID) {
      MessageProcessAborted();
      return false;
    }

    if (strlen(amount_buf) == 0) {
      if (is_zero_amount_allowed)
        break;
      GL_Dialog_Message(gl_.GetHandle(), txn_type_title_,
                        "Please Enter Amount!",
                        GL_ICON_WARNING, GL_BUTTON_NONE,
                        msg_duration_);
    }
    else {
      amount.SetValue(utils::FromString<uint64_t>(std::string(amount_buf)));
      break;
    }
  } while (true);

  return true;
}

bool GoalUi::RequestHostName(std::string &host_ip) {
  tpcore::TeliumKeyboardGuard keyboard_guard;
  host_ip.resize(16);

  ulong return_key = GL_Dialog_Text(gl_.GetHandle(), "Host Name",
                                    "ENTER HOST NAME:",
                                    NULL,
                                    (char*) host_ip.c_str(),
                                    host_ip.size(),
                                    screen_timeout);

  if (return_key != GL_KEY_VALID)
    return false;

  host_ip.resize(strlen(host_ip.c_str()));
  return true;
}

bool GoalUi::RequestPort(std::string &port) {
  tpcore::TeliumKeyboardGuard keyboard_guard;
  port.resize(6);

  ulong return_key = GL_Dialog_Text(gl_.GetHandle(), NULL, "ENTER PORT NUMBER:",
                                    "/d/d/d/d/d",
                                    (char*) port.c_str(),
                                    port.size(),
                                    screen_timeout);
  if (return_key != GL_KEY_VALID)
    return false;
  port.resize(strlen(port.c_str()));

  return true;
}

bool GoalUi::RequestPan(std::vector<uint8_t>& pan_no) {
  tpcore::TeliumKeyboardGuard keyboard_guard;
  char buf[19 + 1] = { 0 };

  while (true) {
    memset(buf, 0, sizeof(buf));
    ulong return_key = GL_Dialog_Text(gl_.GetHandle(), txn_type_title_,
                                      "ENTER PAN:",
                                      "/d/d/d/d/d/d/d/d/d/d/d/d/d/d/d/d/d/d/d",
                                      buf,
                                      sizeof(buf), screen_timeout);
    if (return_key != GL_KEY_VALID) {
      MessageProcessAborted();
      return false;
    }

    if (strlen(buf) >= 11)
      break;

    GL_Dialog_Message(gl_.GetHandle(), txn_type_title_,
                      "Pan No Should Be Atleast 11 Digits!",
                      GL_ICON_WARNING,
                      GL_BUTTON_NONE,
                      msg_duration_);
  }

  int dataArraySize = strlen(buf) / sizeof(char);
  pan_no.reserve(pan_no.size() + dataArraySize);
  std::copy(&buf[0], &buf[dataArraySize], std::back_inserter(pan_no));

  return true;
}

bool GoalUi::RequestLast4DigitOfPan(std::string &last_4_digit) {
  tpcore::TeliumKeyboardGuard keyboard_guard;
  last_4_digit.resize(5);

  while (true) {
    ulong return_key = GL_Dialog_Text(gl_.GetHandle(), txn_type_title_,
                                      "ENTER LAST 4 DIGIT:",
                                      "/d/d/d/d",
                                      (char*) last_4_digit.c_str(),
                                      4,
                                      screen_timeout);
    if (return_key != GL_KEY_VALID)
      return false;

    if (strlen(last_4_digit.c_str()) == 4)
      break;

    GL_Dialog_Message(gl_.GetHandle(), txn_type_title_,
                      "Please Enter Last 4 Digits!",
                      GL_ICON_WARNING,
                      GL_BUTTON_NONE,
                      msg_duration_);
  }

  last_4_digit.resize(strlen(last_4_digit.c_str()));

  return true;
}

bool GoalUi::Request4DbcForAmexCard(std::string* get_4_dbc) {
  tpcore::TeliumKeyboardGuard keyboard_guard;
  get_4_dbc->resize(5);

  while (true) {
    ulong return_key = GL_Dialog_Text(gl_.GetHandle(), txn_type_title_,
                                      "ENTER 4DBC:",
                                      "/d/d/d/d",
                                      (char*) get_4_dbc->c_str(),
                                      5,
                                      screen_timeout);
    if (return_key != GL_KEY_VALID)
      return false;

    if (strlen(get_4_dbc->c_str()) == 4)
      break;

    GL_Dialog_Message(gl_.GetHandle(), txn_type_title_,
                      "Please Enter Atleast 4 Digits!",
                      GL_ICON_WARNING,
                      GL_BUTTON_NONE,
                      msg_duration_);
  }

  return true;
}

bool GoalUi::RequestAtnOrPnrNo(std::string &atn_pnr) {
  tpcore::TeliumKeyboardGuard keyboard_guard;
  atn_pnr.resize(14);

  ulong return_key = GL_Dialog_Text(gl_.GetHandle(), txn_type_title_,
                                    "ENTER ATN/PNR NO:",
                                    "/d/d/d/d/d/d/d/d/d/d/d/d/d",
                                    (char*) atn_pnr.c_str(),
                                    14,
                                    screen_timeout);
  if (return_key != GL_KEY_VALID)
    return false;

  atn_pnr.resize(strlen(atn_pnr.c_str()));

  return true;
}

bool GoalUi::RequestExpiryDate(std::string& expiry_date) {
  tpcore::TeliumKeyboardGuard keyboard_guard;

  while (true) {
    expiry_date.clear();
    expiry_date.resize(5);

    ulong return_key = GL_Dialog_Text(gl_.GetHandle(), txn_type_title_,
                                      "ENTER EXPIRY DATE (MM/YY):",
                                      "/d/d/d/d",
                                      (char*) expiry_date.c_str(),
                                      5, screen_timeout);
    if (return_key != GL_KEY_VALID) {
      MessageProcessAborted();
      return false;
    }

    if (strlen(expiry_date.c_str()) == 4) {
      std::string month = expiry_date.substr(0, 2);
      if (month.compare("01") >= 0 && month.compare("12") <= 0)
        break;
    }
  }

  expiry_date = expiry_date.substr(2, 2) + expiry_date.substr(0, 2);

  expiry_date.resize(strlen(expiry_date.c_str()));

  return true;
}

bool GoalUi::RequestCvv(std::string &cvv, bool bypass_allowed) {
  tpcore::TeliumKeyboardGuard keyboard_guard;
  const uint32_t fcvv_min_size = 3;
  const uint32_t fcvv_max_size = 4;

  while (true) {
    cvv.clear();
    cvv.resize(fcvv_max_size);

    ulong return_key = GL_Dialog_Text(gl_.GetHandle(), txn_type_title_,
                                      "ENTER CVV:",
                                      "/d/d/d/d",
                                      (char*) cvv.c_str(),
                                      fcvv_max_size + 1, screen_timeout);
    if (return_key != GL_KEY_VALID) {
      MessageProcessAborted();
      return false;
    }

    if (bypass_allowed) {
      if (strlen(cvv.c_str()) == 0)
        break;
    }

    if (strlen(cvv.c_str()) >= fcvv_min_size
        && strlen(cvv.c_str()) <= fcvv_max_size)
      break;

  }

  cvv.resize(strlen(cvv.c_str()));

  return true;
}

bool GoalUi::RequestAuthorizationCode(std::string &auth_code) {
  tpcore::TeliumKeyboardGuard keyboard_guard;
  const ulong kMaxLength = 6;
  auth_code.resize(kMaxLength);

  ulong return_key;
  do {
    return_key = GL_Dialog_Text(gl_.GetHandle(), txn_type_title_,
                                "ENTER AUTH CODE:",
                                "/a/a/a/a/a/a",
                                (char*) auth_code.c_str(),
                                kMaxLength + 1,
                                screen_timeout);
    if (return_key != GL_KEY_VALID) {
      MessageProcessAborted();
      return false;
    }
  } while (strlen(auth_code.c_str()) == 0);

  auth_code.resize(strlen(auth_code.c_str()));

  return true;
}

bool GoalUi::RequestPassword(std::string display_msg, std::string& password) {
  tpcore::TeliumKeyboardGuard keyboard_guard;
  const ulong max_length = 10;

  password.clear();
  password.resize(max_length);

  ulong return_key = GL_Dialog_Password(gl_.GetHandle(), txn_type_title_,
                                        display_msg.c_str(),
                                        "/d/d/d/d/d/d/d/d/d/d",
                                        (char*) password.c_str(),
                                        max_length,
                                        screen_timeout);
  if (return_key != GL_KEY_VALID) {
    MessageProcessAborted();
    return false;
  }

  password.resize(strlen(password.c_str()));

  return true;
}

bool GoalUi::SelectAccountType(types::AccountType& account_type) {

  std::vector<std::string> menu_content;
  menu_content.push_back("SAVINGS");
  menu_content.push_back("CURRENT");
  menu_content.push_back("DEFAULT");

  std::string item_selected = MenuSelection("Select Account", menu_content);
  if (item_selected.empty()) {
    return false;
  }

  account_type =
      (item_selected == "SAVINGS") ?
          types::AccountType::SAVING :
          ((item_selected == "CURRENT") ?
                                          types::AccountType::CURRENT :
                                          types::AccountType::DEFAULT);

  return true;
}

bool GoalUi::SelectRecordSearchOption(RecordSearchOption& search_option) {
  std::vector<std::string> menu_content;
  const std::string kTraceNumberMenuItem = "TRACE NUMBER";
  const std::string kPanMenuItem = "PAN";
  const std::string kInvoiceNumberMenuItem = "INVOICE NUMBER";
  const std::string kTxAmountMenuItem = "TRANSACTION AMOUNT";

  menu_content.push_back(kTraceNumberMenuItem);
  menu_content.push_back(kPanMenuItem);
  menu_content.push_back(kInvoiceNumberMenuItem);
  menu_content.push_back(kTxAmountMenuItem);

  std::string item_selected = MenuSelection("Search Record By:", menu_content);
  if (item_selected.empty()) {
    return false;
  }

  if (item_selected == kTraceNumberMenuItem) {
    search_option = SEARCH_BY_TRACE_NUMBER;
  } else if (item_selected == kPanMenuItem) {
    search_option = SEARCH_BY_ACCOUNT_PAN;
  } else if (item_selected == kInvoiceNumberMenuItem) {
    search_option = SEARCH_BY_INVOICE_NUMBER;
  } else {
    search_option = SEARCH_BY_TX_AMOUNT;
  }

  return true;
}

bool GoalUi::SelectTransaction(const std::vector<TxReviewDetail>& tx_list,
                               int* index) {
  tpcore::TeliumKeyboardGuard keyboard_guard;
  std::vector<std::string> menu_content;

  if (tx_list.empty()) {
    return false;
  }

  for (const auto& tx_rec : tx_list) {
    std::string item_str = tx_rec.tx_name + " "
        + tx_rec.amount.GetCurrencyName() + " "
        + tx_rec.amount.ToString();
    menu_content.push_back(item_str);
  }

  std::string item_selected = MenuSelection("Select Transaction:",
                                            menu_content);
  if (item_selected.empty()) {
    return false;
  }

  *index = -1;
  for (unsigned int i = 0; i < menu_content.size(); i++) {
    if (item_selected == menu_content[i]) {
      *index = i;
      break;
    }
  }

  return *index >= 0;
}

bool GoalUi::SelectSettlementOption(SettlementOption& settlement_option) {
  std::vector<std::string> menu_content;
  menu_content.push_back("SETTLE ALL HOSTS");
  menu_content.push_back("SETTLE BY HOST");

  std::string item_selected = MenuSelection("Select Settlement Option",
                                            menu_content);
  if (item_selected.empty()) {
    return false;
  }

  settlement_option =
      (item_selected == "SETTLE ALL HOSTS") ?
                                              SettlementOption::SETTLE_ALL :
                                              SettlementOption::SETTLE_BY_HOST;

  return true;
}

bool GoalUi::SelectReportOption(SettlementOption& report_option) {
  std::vector<std::string> menu_content;
  menu_content.push_back("ALL HOSTS");
  menu_content.push_back("BY HOST");

  std::string item_selected = MenuSelection("Select Report Option",
                                            menu_content);
  if (item_selected.empty()) {
    return false;
  }

  report_option =
      (item_selected == "ALL HOSTS") ?
                                       SettlementOption::SETTLE_ALL :
                                       SettlementOption::SETTLE_BY_HOST;

  return true;
}

bool GoalUi::SelectHost(const std::vector<std::string>& host_name_list,
                        unsigned int& pos) {
  std::string item_selected = MenuSelection("Select Host", host_name_list);
  if (item_selected.empty()) {
    return false;
  }

  pos = find(host_name_list.begin(), host_name_list.end(), item_selected)
      - host_name_list.begin();
  if (pos < host_name_list.size())
    return true;

  return false;
}
bool GoalUi::SelectInstalmentPlan(
    const std::vector<std::string>& instalment_name_list, unsigned int& pos) {
  std::string item_selected = MenuSelection("Select Installment Plan",
                                            instalment_name_list);
  if (item_selected.empty()) {
    return false;
  }

  pos = find(instalment_name_list.begin(), instalment_name_list.end(),
             item_selected) - instalment_name_list.begin();
  if (pos < instalment_name_list.size())
    return true;

  return false;
}
bool GoalUi::SelectInstalmentOption(InstalmentOption& instalment_option) {
  std::vector<std::string> menu_content;
  menu_content.push_back("SCB PLANS");
  menu_content.push_back("NON-SCB PLANS");

  std::string item_selected = MenuSelection("Select Instalment Option",
                                            menu_content);
  if (item_selected.empty()) {
    return false;
  }

  instalment_option =
      (item_selected == "SCB PLANS") ?
                                       InstalmentOption::SCB :
                                       InstalmentOption::NON_SCB;

  return true;
}
bool GoalUi::SelectAcquirer(unsigned int& host_index, bool show_all_acquirer) {
  tpcore::TeliumKeyboardGuard keyboard_guard;
  //const char * menu[] = { "CREDIT", "AMEX", 0 };
  const char * menu[] = { "VISA", "MC", "MC", "JCB", "UNIONPAY", 0 };
  const char * menu_all[] = { "CREDIT", "DEBIT", "AMEX", 0 };
  std::string selected_acquirer;
  ulong choice = 0;

  if (show_all_acquirer) {
    choice = GL_Dialog_Menu(gl_.GetHandle(), txn_type_title_, menu_all, choice,
                            GL_BUTTON_CANCEL,
                            GL_KEY_1,
                            screen_timeout);
    if (choice >= (sizeof(menu_all) / sizeof(menu_all[0])) - 1) {
      MessageProcessAborted();
      return false;
    }

    selected_acquirer = menu_all[choice];
  } else {
    choice = GL_Dialog_Menu(gl_.GetHandle(), txn_type_title_, menu, choice,
                            GL_BUTTON_CANCEL,
                            GL_KEY_1,
                            screen_timeout);
    if (choice >= (sizeof(menu) / sizeof(menu[0])) - 1) {
      MessageProcessAborted();
      return false;
    }

    selected_acquirer = menu[choice];
  }

  host_index = choice;  // TIDI: check if off by one

  return true;
}

bool GoalUi::DisplayTransactionCountAndTotal(const types::Currency& currency,
                                             unsigned int sale_count,
                                             uint64_t sale_total,
                                             unsigned int refund_count,
                                             uint64_t refund_total) {
  tpcore::TeliumKeyboardGuard keyboard_guard;

  types::Amount sale_amount(currency, sale_total);
  types::Amount refund_amount(currency, refund_total);

  std::string message;
  message += "SALES COUNT: " + stdx::to_string(sale_count) + "\n";
  message += "SALES TOTALS: " + currency.GetName() + " "
      + sale_amount.ToString() + "\n";
  message += "REFUNDS COUNT: " + stdx::to_string(refund_count) + "\n";
  message += "REFUNDS TOTALS: " + currency.GetName() + " "
      + refund_amount.ToString() + "\n";

  ulong return_key = GL_Dialog_Message(gl_.GetHandle(),
                                       "Settlement",
                                       message.c_str(),
                                       GL_ICON_NONE,
                                       GL_BUTTON_VALID_CANCEL,
                                       screen_timeout);

  if (return_key != GL_KEY_VALID) {
    MessageProcessAborted();
    return false;
  }
  return true;
}

void GoalUi::MessageNoLastSettlementRecord() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "NO SETTLEMENT RECORD",
                    GL_ICON_WARNING,
                    GL_BUTTON_NONE, msg_duration_);
}

bool GoalUi::DisplayTransactionInfo(const std::string &txn_type,
                                    const types::Amount& amount) {
  tpcore::TeliumKeyboardGuard keyboard_guard;
  std::string message = txn_type + "\nTotal Amount: " + amount.ToString();

  ulong return_key = GL_Dialog_Message(gl_.GetHandle(), txn_type_title_,
                                       message.c_str(),
                                       GL_ICON_INFORMATION,
                                       GL_BUTTON_VALID_CANCEL,
                                       screen_timeout);
  if (return_key != GL_KEY_VALID) {
    MessageProcessAborted();
    return false;
  }

  return true;
}

bool GoalUi::DisplayTransactionAndTipAmount(types::Amount& sale_amount,
                                            types::Amount& tip_amount) {

  tpcore::TeliumKeyboardGuard keyboard_guard;

  std::string sale_amount_msg = "Sale Amount: " + sale_amount.ToString();
  std::string tip_amount_msg = "Tip Amount: " + tip_amount.ToString();

  ulong return_key = GL_Dialog_Message(gl_.GetHandle(), sale_amount_msg.c_str(),
                                       tip_amount_msg.c_str(),
                                       GL_ICON_INFORMATION,
                                       GL_BUTTON_VALID_CANCEL,
                                       screen_timeout);
  if (return_key != GL_KEY_VALID) {
    MessageProcessAborted();
    return false;
  }

  return true;
}

void GoalUi::MessageTipAdjustNotAllowed() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "Tip Adjust Not Allowed!",
                    GL_ICON_WARNING,
                    GL_BUTTON_NONE, msg_duration_);
}

void GoalUi::MessageProcessing() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "PROCESSING",
                    GL_ICON_INFORMATION,
                    GL_BUTTON_NONE, 0);
}

bool GoalUi::DisplayBalance(const types::Amount& amount) {
  tpcore::TeliumKeyboardGuard keyboard_guard;
  std::string message = "Available Balance:\n Rs " + amount.ToString();

  ulong return_key = GL_Dialog_Message(gl_.GetHandle(), txn_type_title_,
                                       message.c_str(),
                                       GL_ICON_INFORMATION,
                                       GL_BUTTON_VALID_CANCEL,
                                       screen_timeout);
  if (return_key != GL_KEY_VALID) {
    return false;
  }

  return true;
}

bool GoalUi::DisplayPanAndExpiryDate(const std::string& masked_pan,
                                     const std::string& masked_expiry_date) {
  tpcore::TeliumKeyboardGuard keyboard_guard;
  std::string month = masked_expiry_date.substr(2, 2);
  std::string year = masked_expiry_date.substr(0, 2);

  std::string message = "Card Number\n" + masked_pan + "\nExpiry Date: " + month
      + "/" + year;

  ulong return_key = GL_Dialog_Message(gl_.GetHandle(), txn_type_title_,
                                       message.c_str(),
                                       GL_ICON_INFORMATION,
                                       GL_BUTTON_VALID_CANCEL,
                                       screen_timeout);
  if (return_key != GL_KEY_VALID) {
    MessageProcessAborted();
    return false;
  }

  return true;
}

bool GoalUi::AskToProceedWithFallback() {
  tpcore::TeliumKeyboardGuard keyboard_guard;

  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "FALLBACK!",
                    GL_ICON_WARNING,
                    GL_BUTTON_NONE, msg_duration_);

  ulong return_key = GL_Dialog_Message(
      gl_.GetHandle(), txn_type_title_,
      "Please Use Mag Card\nRemove SMC And\nCancel To Stop\nOk To Proceed",
      GL_ICON_NONE, GL_BUTTON_VALID_CANCEL, screen_timeout);
  if (return_key != GL_KEY_VALID) {
    MessageProcessAborted();
    return false;
  }

  return true;
}

bool GoalUi::AskToPrintCustomerCopy() {
  tpcore::TeliumKeyboardGuard keyboard_guard;
  ulong return_key = GL_Dialog_Message(gl_.GetHandle(), txn_type_title_,
                                       "Print Customer Copy?",
                                       GL_ICON_QUESTION,
                                       GL_BUTTON_VALID_CANCEL,
                                       screen_timeout);
  if (return_key != GL_KEY_VALID)
    return false;

  return true;
}

bool GoalUi::AskToPrintDetailReport() {
  tpcore::TeliumKeyboardGuard keyboard_guard;
  ulong return_key = GL_Dialog_Message(gl_.GetHandle(), NULL,
                                       "Print Detail Report?",
                                       GL_ICON_QUESTION,
                                       GL_BUTTON_VALID_CANCEL,
                                       screen_timeout);

  if (return_key != GL_KEY_VALID)
    return false;

  return true;
}

bool GoalUi::AskToDeleteOldPreAuth() {
  tpcore::TeliumKeyboardGuard keyboard_guard;
  ulong return_key = GL_Dialog_Message(gl_.GetHandle(), NULL,
                                       "Delete old PreAuth ?",
                                       GL_ICON_QUESTION,
                                       GL_BUTTON_VALID_CANCEL,
                                       screen_timeout);

  if (return_key != GL_KEY_VALID) {
    MessageProcessAborted();
    return false;
  }

  return true;
}

void GoalUi::MessageEMVProcessing() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "EMV PROCESSING",
                    GL_ICON_INFORMATION,
                    GL_BUTTON_NONE, 0);
}

void GoalUi::MessageEMVProcessingError() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "EMV PROCESSING ERROR",
                    GL_ICON_ERROR,
                    GL_BUTTON_NONE, msg_duration_);
}

void GoalUi::MessageSelectionInProgress() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "SELECTION IN PROGRESS",
                    GL_ICON_INFORMATION,
                    GL_BUTTON_NONE, 0);
}

void GoalUi::MessageInsertPaperRoll() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "INSERT PAPER ROLL",
                    GL_ICON_WARNING,
                    GL_BUTTON_NONE, msg_duration_);
}

void GoalUi::MessageProcessingCard() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "Processing Card...",
                    GL_ICON_INFORMATION,
                    GL_BUTTON_NONE, 0);
}

void GoalUi::MessageInsertOrSwipeCard() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "Insert/Swipe Card",
                    GL_ICON_INFORMATION,
                    GL_BUTTON_NONE, 0);
}

void GoalUi::MessageEnterPanOrSwipeCard() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "Enter Pan/Swipe Card",
                    GL_ICON_INFORMATION,
                    GL_BUTTON_NONE, 0);
}

void GoalUi::MessageManualInputCard() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_,
                    "Please Enter Card Number",
                    GL_ICON_INFORMATION,
                    GL_BUTTON_NONE,
                    0);
}

void GoalUi::MessagePleaseUseOtherInterfaces() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_,
                    "Please Use Other Interfaces",
                    GL_ICON_INFORMATION,
                    GL_BUTTON_NONE,
                    msg_duration_);
}

void GoalUi::MessageSwipeOrInsertOrTapCard() {
  T_GL_HWIDGET window = GL_GraphicLib_ReadResource(
        gl_.GetHandle(), GetDAL(RES_CLESS_LOGO__SCREEN).c_str());
  GL_Window_Dispatch(window, 0);
  GL_Widget_Destroy(window);
}

void GoalUi::MessagePleaseInsertCard() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "Please Insert Card!!!",
                    GL_ICON_WARNING,
                    GL_BUTTON_NONE, msg_duration_);
}

void GoalUi::MessagePleaseRemoveCard() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "Please Remove Card!!!",
                    GL_ICON_WARNING,
                    GL_BUTTON_NONE, 0);
}

void GoalUi::MessageUnsupportedCard() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "Unsupported Card!!!",
                    GL_ICON_ERROR,
                    GL_BUTTON_NONE, msg_duration_);
}

void GoalUi::MessageInvalidPan() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "Invalid Pan!!!",
                    GL_ICON_ERROR,
                    GL_BUTTON_NONE, msg_duration_);
}

void GoalUi::MessageLast4DigitMismatch() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_,
                    "Last 4 Digits Mismatch!!!",
                    GL_ICON_ERROR, GL_BUTTON_NONE,
                    msg_duration_);
}

void GoalUi::MessageInvalidTrack2() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "Invalid Track2!!!",
                    GL_ICON_ERROR,
                    GL_BUTTON_NONE, msg_duration_);
}

void GoalUi::MessageCardRemoved() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "Card Removed!!!",
                    GL_ICON_ERROR,
                    GL_BUTTON_NONE, msg_duration_);
}

void GoalUi::MessageCardBlocked() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "Card Blocked!!!",
                    GL_ICON_ERROR,
                    GL_BUTTON_NONE, msg_duration_);
}

void GoalUi::MessageCardExpired() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "Card Expired!!!",
                    GL_ICON_ERROR,
                    GL_BUTTON_NONE, msg_duration_);
}

void GoalUi::MessageCardError() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "Card Error!!!",
                    GL_ICON_ERROR,
                    GL_BUTTON_NONE, msg_duration_);
}

void GoalUi::MessageHostError() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "Host Error!!!",
                    GL_ICON_ERROR,
                    GL_BUTTON_NONE, msg_duration_);
}

void GoalUi::MessagePinOk() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "PIN OK",
                    GL_ICON_INFORMATION,
                    GL_BUTTON_NONE, msg_duration_);
}

void GoalUi::MessagePinBlocked() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "PIN BLOCKED",
                    GL_ICON_INFORMATION,
                    GL_BUTTON_NONE, msg_duration_);
}

void GoalUi::MessageIncorrectPin() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "Incorrect PIN",
                    GL_ICON_INFORMATION,
                    GL_BUTTON_NONE, msg_duration_);
}

void GoalUi::MessageInvalidPassword() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_,
                    "Incorrect Password \n Please try again!",
                    GL_ICON_ERROR,
                    GL_BUTTON_NONE,
                    msg_duration_);
}

void GoalUi::MessageSwipeNotAllowedForThisCard() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_,
                    "Swipe Not Allowed For This Card!",
                    GL_ICON_WARNING,
                    GL_BUTTON_NONE,
                    msg_duration_);
}

void GoalUi::MessageManualNotAllowedForThisCard() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_,
                    "Manual Not Allowed For This Card!",
                    GL_ICON_WARNING,
                    GL_BUTTON_NONE,
                    msg_duration_);
}

void GoalUi::MessagePleaseSeePhone() {
  std::string message = "SEE PHONE FOR\nINSTRUCTIONS";
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, message.c_str(),
                    GL_ICON_ERROR,
                    GL_BUTTON_NONE, msg_duration_);
}

void GoalUi::MessageUnknownError() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "Unknown Error",
                    GL_ICON_ERROR,
                    GL_BUTTON_NONE, msg_duration_);
}
void GoalUi::MessageParameterNotAvailable() {
  std::string message = "Parameter Not Available\n For Selected Plan";
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, message.c_str(),
                    GL_ICON_ERROR,
                    GL_BUTTON_NONE, msg_duration_);
}
void GoalUi::MessageSaleAlreadyCompleted() {
  GL_Dialog_Message(gl_.GetHandle(), NULL, "ALREADY COMPLETED!",
                    GL_ICON_WARNING,
                    GL_BUTTON_NONE, msg_duration_);
}

void GoalUi::MessagePleaseTryAgain() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "Please Try Again!",
                    GL_ICON_INFORMATION,
                    GL_BUTTON_NONE, msg_duration_);
}

void GoalUi::MessagePrintingMerchantCopy() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "Printing Merchant Copy",
                    GL_ICON_INFORMATION,
                    GL_BUTTON_NONE, 0);
}

void GoalUi::MessagePrintingCustomerCopy() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "Printing Customer Copy",
                    GL_ICON_INFORMATION,
                    GL_BUTTON_NONE, 0);
}

void GoalUi::MessagePrintingReversalReceipt() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_,
                    "Printing Reversal Receipt",
                    GL_ICON_INFORMATION,
                    GL_BUTTON_NONE,
                    0);
}

void GoalUi::MessageCommunicatingHost() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "Communicating Host",
                    GL_ICON_INFORMATION,
                    GL_BUTTON_NONE, 0);
}

void GoalUi::MessageCommunicationError() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "Communication Error!",
                    GL_ICON_ERROR,
                    GL_BUTTON_NONE, msg_duration_);
}

void GoalUi::MessageRebootingSystem() {
  GL_Dialog_Message(gl_.GetHandle(), "REV DOWNLOAD",
                    "Rebooting System\nPlease Wait!!!",
                    GL_ICON_INFORMATION,
                    GL_BUTTON_NONE,
                    msg_duration_);
}

void GoalUi::MessageMasterKeyNotPresent() {
  GL_Dialog_Message(gl_.GetHandle(), "KEY EXCHANGE",
                    "Master Key Not Present!!!",
                    GL_ICON_ERROR, GL_BUTTON_NONE,
                    msg_duration_);
}

void GoalUi::MessageLogonFailure() {
  GL_Dialog_Message(gl_.GetHandle(), "KEY EXCHANGE", "LOGON FAILURE",
                    GL_ICON_ERROR,
                    GL_BUTTON_NONE, msg_duration_);
}

void GoalUi::MessageLogonSuccessful() {
  GL_Dialog_Message(gl_.GetHandle(), "KEY EXCHANGE", "LOGON SUCCESSFUL",
                    GL_ICON_INFORMATION,
                    GL_BUTTON_NONE, msg_duration_);
}

void GoalUi::MessageApproved(const std::string& approval_code) {
  std::string messsage = "APPROVED\n" + approval_code;
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, messsage.c_str(),
                    GL_ICON_INFORMATION,
                    GL_BUTTON_NONE, msg_duration_);
}

void GoalUi::MessageDeclined() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "DECLINED", GL_ICON_ERROR,
                    GL_BUTTON_NONE,
                    msg_duration_);
}

void GoalUi::MessageClessError() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "CLESS ERROR", GL_ICON_ERROR,
                    GL_BUTTON_NONE,
                    msg_duration_);
}

void GoalUi::MessageDeclinedByCard() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "DECLINED BY CARD!",
                    GL_ICON_ERROR,
                    GL_BUTTON_NONE, msg_duration_);
}

void GoalUi::MessageProcessAborted() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "PROCESS ABORTED",
                    GL_ICON_INFORMATION,
                    GL_BUTTON_NONE, msg_duration_);
}

void GoalUi::MessageTransactionNotAllowed() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_,
                    "Transaction Not Allowed!!!",
                    GL_ICON_WARNING,
                    GL_BUTTON_NONE,
                    msg_duration_);
}

void GoalUi::MessageTransactionAlreadyVoided() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_,
                    "Transaction Already Voided!",
                    GL_ICON_WARNING,
                    GL_BUTTON_NONE,
                    msg_duration_);
}

void GoalUi::MessageTransactionAlreadyAdjusted() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_,
                    "Transaction Already Adjusted!",
                    GL_ICON_WARNING,
                    GL_BUTTON_NONE,
                    msg_duration_);
}

void GoalUi::MessageCashAmountMultipleOf100() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_,
                    "Cash Amount Should Be In Multiple Of 100!",
                    GL_ICON_WARNING, GL_BUTTON_NONE, msg_duration_);
}

bool GoalUi::MessageTipPercentageExceeded() {
  tpcore::TeliumKeyboardGuard keyboard_guard;
  ulong return_key = GL_Dialog_Message(
      gl_.GetHandle(), txn_type_title_,
      "Tip Percentage Exceeded!\nPlease enter again!",
      GL_ICON_INFORMATION,
      GL_BUTTON_VALID_CANCEL,
      screen_timeout);

  if (return_key != GL_KEY_VALID) {
    MessageProcessAborted();
    return false;
  }

  return true;
}

void GoalUi::MessageTransactionAlreadyPreAuthCompleted() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_,
                    "Already preauth completed!",
                    GL_ICON_INFORMATION,
                    GL_BUTTON_NONE,
                    msg_duration_);
}

void GoalUi::MessageSearchingInLogs() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "Searching In Logs",
                    GL_ICON_INFORMATION,
                    GL_BUTTON_NONE, 0);
}

void GoalUi::MessageTransactionNotFound() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "Transaction Not Found!",
                    GL_ICON_WARNING,
                    GL_BUTTON_NONE, msg_duration_);
}

void GoalUi::MessageReversalNotFound() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "No Reversal Found!",
                    GL_ICON_WARNING,
                    GL_BUTTON_NONE, msg_duration_);
}

void GoalUi::MessagePleaseSettleBatch() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "Please Settle Batch!",
                    GL_ICON_WARNING,
                    GL_BUTTON_NONE, msg_duration_);
}

void GoalUi::MessagePleaseCloseBatch() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "Please Close Batch!",
                    GL_ICON_WARNING,
                    GL_BUTTON_NONE, msg_duration_);
}

void GoalUi::MessageSettlementSuccessful() {
  GL_Dialog_Message(gl_.GetHandle(), NULL, "SETTLEMENT SUCCESSFUL",
                    GL_ICON_INFORMATION,
                    GL_BUTTON_NONE, msg_duration_);
}

void GoalUi::MessageSettlementFailed() {
  GL_Dialog_Message(gl_.GetHandle(), NULL, "SETTLEMENT FAILED",
                    GL_ICON_ERROR,
                    GL_BUTTON_NONE, msg_duration_);
}

void GoalUi::MessageRetrySettlement() {
  GL_Dialog_Message(gl_.GetHandle(), NULL, "RETRY SETTLEMENT",
                    GL_ICON_INFORMATION,
                    GL_BUTTON_NONE, msg_duration_);
}

void GoalUi::MessageSettlementInProgress(std::string host_name) {
  std::string message;

  if (!host_name.empty())
    message = host_name + " Settlement\nIn Progress";
  else
    message = "Settlement\nIn Progress";

  GL_Dialog_Message(gl_.GetHandle(), NULL, message.c_str(),
                    GL_ICON_WARNING,
                    GL_BUTTON_NONE, 0);
}

void GoalUi::MessageBatchEmpty(const std::string& batch_type) {
  std::string message;

  if (!batch_type.empty())
    message = batch_type + " BATCH EMPTY!";
  else
    message = "BATCH EMPTY!";

  GL_Dialog_Message(gl_.GetHandle(), NULL, message.c_str(), GL_ICON_WARNING,
                    GL_BUTTON_NONE,
                    msg_duration_);
}

void GoalUi::MessageBatchTotalZero(const std::string& batch_type) {
  std::string message;

  if (!batch_type.empty())
    message = batch_type + " NET TOTALS ZERO";
  else
    message = "NET TOTALS ZERO";

  GL_Dialog_Message(gl_.GetHandle(), NULL, message.c_str(), GL_ICON_WARNING,
                    GL_BUTTON_NONE,
                    msg_duration_);
}

void GoalUi::MessageBatchFull() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "BATCH FULL!",
                    GL_ICON_WARNING,
                    GL_BUTTON_NONE, msg_duration_);
}

void GoalUi::MessageBatchLocked() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_, "BATCH LOCKED!",
                    GL_ICON_WARNING,
                    GL_BUTTON_NONE, msg_duration_);
}

void GoalUi::MessageBatchUploadInProgress(std::string host_name) {
  std::string message;

  if (!host_name.empty())
    message = host_name + " Batch Upload\nIn Progress";
  else
    message = "Batch Upload\nIn Progress";

  GL_Dialog_Message(gl_.GetHandle(), NULL, message.c_str(),
                    GL_ICON_WARNING,
                    GL_BUTTON_NONE, msg_duration_);
}

void GoalUi::MessageBatchMaxLimitExceeded() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_,
                    "Batch Max Limit Exceeded!",
                    GL_ICON_WARNING,
                    GL_BUTTON_NONE,
                    msg_duration_);
}

void GoalUi::MessagePleaseInitializeTerminal() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_,
                    "Please Initialize Terminal!",
                    GL_ICON_WARNING,
                    GL_BUTTON_NONE,
                    msg_duration_);
}

void GoalUi::MessageTerminalAlreadyInitialized() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_,
                    "Terminal Already Initialized!",
                    GL_ICON_WARNING,
                    GL_BUTTON_NONE,
                    msg_duration_);
}

void GoalUi::MessageDownloadAndInitializeParameterFile() {
  GL_Dialog_Message(gl_.GetHandle(), "REV DOWNLOAD",
                    "Downloading And Initializing Parameter File!",
                    GL_ICON_INFORMATION, GL_BUTTON_NONE, 0);
}

void GoalUi::MessageParameterFileNotFoundOnServer() {
  GL_Dialog_Message(gl_.GetHandle(), "REV DOWNLOAD",
                    "Parameter File Not Found On Server!",
                    GL_ICON_ERROR,
                    GL_BUTTON_NONE,
                    msg_duration_);
}

void GoalUi::MessageAppVersionNotAllowedToDownload() {
  GL_Dialog_Message(gl_.GetHandle(), "REV DOWNLOAD",
                    "App Version Not Allowed To Download!",
                    GL_ICON_ERROR,
                    GL_BUTTON_NONE,
                    msg_duration_);
}

void GoalUi::MessageParameterVersionNotFoundOnServer() {
  GL_Dialog_Message(gl_.GetHandle(), "REV DOWNLOAD",
                    "Parameter Version Not Found On Server!",
                    GL_ICON_ERROR,
                    GL_BUTTON_NONE,
                    msg_duration_);
}

void GoalUi::MessageInvalidParameterData() {
  GL_Dialog_Message(gl_.GetHandle(), "REV DOWNLOAD", "Invalid Parameter Data!",
                    GL_ICON_ERROR,
                    GL_BUTTON_NONE, msg_duration_);
}

void GoalUi::MessageInitializationSuccessful() {
  GL_Dialog_Message(gl_.GetHandle(), "REV DOWNLOAD",
                    "Initialization Successful!",
                    GL_ICON_INFORMATION,
                    GL_BUTTON_NONE,
                    msg_duration_);
}

void GoalUi::MessageInitializationFailed() {
  GL_Dialog_Message(gl_.GetHandle(), "REV DOWNLOAD", "Initialization Failed!",
                    GL_ICON_ERROR,
                    GL_BUTTON_NONE, msg_duration_);
}

bool GoalUi::ConfirmDeleteBatch() {
  tpcore::TeliumKeyboardGuard keyboard_guard;
  ulong return_key = GL_Dialog_Message(
      gl_.GetHandle(), txn_type_title_,
      "Please Confirm To \nClear Batch\nCancel To Stop\nOk To Proceed",
      GL_ICON_NONE, GL_BUTTON_VALID_CANCEL, screen_timeout);

  if (return_key != GL_KEY_VALID) {
    MessageProcessAborted();
    return false;
  }

  return true;
}

void GoalUi::MessageDisplayBatchEmpty(const std::string& host_name) {
  std::string message;

  if (!host_name.empty())
    message = host_name + "\n BATCH EMPTY!";
  else
    message = "BATCH EMPTY!";

  GL_Dialog_Message(gl_.GetHandle(), NULL, message.c_str(), GL_ICON_WARNING,
                    GL_BUTTON_NONE,
                    msg_duration_);
}

void GoalUi::MessageDeleteBatch(unsigned int host_index) {

  std::string message;

  std::string batch_name;
  batch_name = GetBatchName(host_index);

  message = batch_name + " BATCH ";

  GL_Dialog_Message(gl_.GetHandle(), message.c_str(), "DELETED SUCCESSFULLY",
                    GL_ICON_INFORMATION,
                    GL_BUTTON_NONE, msg_duration_);
}
tpcore::PinEntryUI::uptr GoalUi::GetPinEntryUI(
                                               bdor::TransactionType tx_type,
                                               types::Amount amount) {
  return stdx::make_unique<PinEntryUI>(gl_, GetTxTypeString(tx_type), amount);
}

void GoalUi::DisplayResponseMessage(const std::string& response_code) {

  auto message = GetResponseCodeString(response_code);
  GL_Dialog_Message(gl_.GetHandle(),
                    txn_type_title_,
                    message.c_str(),
                    GL_ICON_INFORMATION,
                    GL_BUTTON_NONE, msg_duration_);
}

void GoalUi::MessageEMVCandidateListEmpty() {
  GL_Dialog_Message(gl_.GetHandle(), "EMV ERROR",
                    "CANDIDATE LIST EMPTY!",
                    GL_ICON_ERROR,
                    GL_BUTTON_NONE,
                    msg_duration_);
}

void GoalUi::MessageEMVReadCardError() {
  GL_Dialog_Message(gl_.GetHandle(), "EMV ERROR",
                    "READ CARD FAILED!",
                    GL_ICON_ERROR,
                    GL_BUTTON_NONE,
                    msg_duration_);
}

void GoalUi::MessageAmountOutOfRange() {
  GL_Dialog_Message(gl_.GetHandle(), "AMOUNT",
                    "OUT OF RANGE!",
                    GL_ICON_ERROR,
                    GL_BUTTON_NONE,
                    msg_duration_);
}

void GoalUi::MessageTipAmountOutOfRange() {
  GL_Dialog_Message(gl_.GetHandle(), "TIP AMOUNT",
                    "OUT OF RANGE!",
                    GL_ICON_ERROR,
                    GL_BUTTON_NONE,
                    msg_duration_);
}

bool GoalUi::ConfirmClearReversal(uint32_t& invoice_no,
                                  const stdx::optional<types::Amount>& amount) {
  tpcore::TeliumKeyboardGuard keyboard_guard;
  std::string invoice = utils::ToString(invoice_no);

  std::string message = "Inv Num:" + invoice;
  if (amount) {
    message += "\nAmount: " + amount->ToString();
  }

  ulong return_key = GL_Dialog_Message(gl_.GetHandle(), txn_type_title_,
                                       message.c_str(),
                                       GL_ICON_INFORMATION,
                                       GL_BUTTON_VALID_CANCEL,
                                       screen_timeout);
  if (return_key != GL_KEY_VALID) {
    MessageProcessAborted();
    return false;
  }

  return true;
}

void GoalUi::ReversalCleared() {
  GL_Dialog_Message(gl_.GetHandle(), "REVERSAL TRANSACTION",
                    "DELETED SUCCESSFULLY",
                    GL_ICON_ERROR, GL_BUTTON_NONE,
                    msg_duration_);
}

bool GoalUi::RequestSignature(const types::Amount& amount,
                              std::string& signature_file_path) {
  tpcore::TeliumKeyboardGuard keyboard_guard;

  if (GL_GraphicLib_IsTouchPresent(gl_.GetHandle())) {
    signature_file_path = "file://flash/HOST/SIGNATURE.SIG";

    if (!GL_File_Exists(signature_file_path.c_str()))
      GL_File_Delete(signature_file_path.c_str());

    std::string amount_text = "Transaction Amount: " +
        amount.GetCurrencyName() + amount.ToString();

    ulong return_key = GL_Dialog_Signature(gl_.GetHandle(), "SIGNATURE CAPTURE",
                                           amount_text.c_str(),
                                           signature_file_path.c_str(),
                                           screen_timeout);

    long file_size = GL_File_GetSizeFilename(signature_file_path.c_str());

    if (return_key == GL_KEY_VALID && file_size > 0)
      return true;
    else
      signature_file_path = "";
  }
  return false;
}

void GoalUi::MessageDisplayResourceNotFound(std::string resource_name) {
  std::string msg = "Resource " + resource_name + " Not Found";
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_,
                    msg.c_str(),
                    GL_ICON_ERROR, GL_BUTTON_NONE,
                    msg_duration_);
}

bool GoalUi::SelectDccOption(const types::Amount& local_amount,
                             const types::Amount& dcc_amount,
                             DccOption& option) {

  std::vector<std::string> menu_content;
  std::string local_amount_display = local_amount.ToString() + " "
      + local_amount.GetCurrencyName();
  std::string dcc_amount_display = dcc_amount.ToString() + " "
      + dcc_amount.GetCurrencyName();

  menu_content.push_back(dcc_amount_display);
  menu_content.push_back(local_amount_display);

  std::string item_selected = MenuSelection("Select Payment Option",
                                            menu_content);
  if (item_selected.empty()) {
    return false;
  }

  option =
      (item_selected == dcc_amount_display) ?
                                              DccOption::OPT_IN :
                                              DccOption::OPT_OUT;

  return true;
}

bool GoalUi::SelectDccMultiOptions(const types::Amount& local_amount,
                           const std::vector<types::Amount>& dcc_amount_list,
                           uint32_t& index_selected) {
  std::vector<std::string> menu_content;

  for (auto i = 0u; i < dcc_amount_list.size(); i++) {
    std::string dcc_amount_display = dcc_amount_list[i].ToString() + " "
        + dcc_amount_list[i].GetCurrencyName();

    menu_content.push_back(dcc_amount_display);
  }

  std::string local_amount_display = local_amount.ToString() + " "
      + local_amount.GetCurrencyName();

  menu_content.push_back(local_amount_display);

  auto item_number = MenuSelectionIndex("Select Payment Option",
                                        menu_content);

  if (item_number == GL_KEY_CANCEL || item_number == GL_RESULT_INACTIVITY) {
    MessageProcessAborted();
    return false;
  }

  index_selected = item_number;
  return true;
}

void GoalUi::MessageTransactionCancelled() {
  GL_Dialog_Message(gl_.GetHandle(), txn_type_title_,
                    "TRANSACTION CANCELLED", GL_ICON_ERROR,
                    GL_BUTTON_NONE,
                    msg_duration_);
}

void GoalUi::DisplayEMVProcessingError(TransactionStatus txn_status){
  switch(txn_status){
    case TransactionStatus::CANCELLED:
      MessageTransactionCancelled();
      break;
    case TransactionStatus::CARD_BLOCKED:
      MessageCardBlocked();
      break;
    case TransactionStatus::CARD_ERROR:
      MessageCardError();
      break;
    case TransactionStatus::CARD_REMOVED_FROM_READER:
      MessageCardRemoved();
      break;
    case TransactionStatus::DECLINED:
      MessageDeclined();
      break;
    case TransactionStatus::NOT_ALLOWED:
      MessageTransactionNotAllowed();
      break;
    case TransactionStatus::TERMINAL_ERROR:
    case TransactionStatus::UNKNOWN_ERROR:
    default:
      MessageEMVProcessingError();
      break;
  }

}

void GoalUi::MessageCountVisit(std::string& field63_data) {
  std::string s = field63_data;
  std::string delimiter = ","; size_t pos = 0; std::string token;

  std::vector<std::string> count_visit;
  while((pos = s.find(delimiter))!=std::string::npos){
	  token = s.substr(0,pos);
	  count_visit.push_back(token);
	  s.erase(0, pos+delimiter.length());
  }
  count_visit.push_back(s);

  std::string message = "VALID CARD." + count_visit.at(1) + "OF" + count_visit.at(2);
  GL_Dialog_Message(gl_.GetHandle(),
                    txn_type_title_,
                    message.c_str(),
                    GL_ICON_INFORMATION,
                    GL_BUTTON_NONE, msg_duration_);
}

void GoalUi::MessageExcessGuest() {
  GL_Dialog_Message(gl_.GetHandle(), NULL, "1 FREE GUEST ONLY\tPAY FOR EXCESS GUEST?",
                    GL_ICON_ERROR,
                    GL_BUTTON_NONE, msg_duration_);
}

}
