#include <tpcore/telium_keyboard_guard.h>
#include <stdx/string>
#include <bdor/goal_ui.h>
#include "goal_res.h"
#include <utils/logger.h>

namespace bdor {

static bool OnTimeout(T_GL_HMESSAGE message)
                      {
  GL_Message_SetResult(message, 0);
  GL_Timer_Stop(GL_Message_GetWidget(message));
  return true;
}

static bool OnKeyPressed(T_GL_HMESSAGE message)
                       {
  T_GL_WCHAR ulKey;
  ulKey = GL_Message_GetKey(message);
  GL_Message_SetUserData(message, &ulKey, sizeof(ulKey));
  GL_Message_SetResult(message, ulKey);
  return true;
}

bool GoalUi::DisplayTxReview(TxReviewDetail& tx_detail) {
  tpcore::TeliumKeyboardGuard keyboard_guard;

  T_GL_HWIDGET review_tx = GL_GraphicLib_ReadResource(
      gl_.GetHandle(), GetDAL(RES_TX_REVIEW_SCREEN).c_str());

  std::string resource_name(RES_TX_REVIEW_SCREEN);
  if (!review_tx) {
    MessageDisplayResourceNotFound(resource_name);
    return false;
  }

  T_GL_HWIDGET review_tx_type_id = GL_Widget_SearchId(review_tx,
                                                      REVIEW_TX_TYPE);

  T_GL_HWIDGET review_card_type_id = GL_Widget_SearchId(review_tx,
                                                        REVIEW_CARD_TYPE);

  T_GL_HWIDGET review_masked_pan_id = GL_Widget_SearchId(review_tx,
                                                         REVIEW_MASKED_PAN);

  T_GL_HWIDGET review_masked_expiry_date_id = GL_Widget_SearchId(
      review_tx, REVIEW_MASKED_EXPIRY_DATE);

  T_GL_HWIDGET review_tx_amount_id = GL_Widget_SearchId(review_tx,
                                                        REVIEW_TX_AMOUNT);

  T_GL_HWIDGET review_auth_code_id = GL_Widget_SearchId(review_tx,
                                                        REVIEW_AUTH_CODE);

  T_GL_HWIDGET review_tx_stan_id = GL_Widget_SearchId(review_tx,
                                                      REVIEW_TX_STAN);

  T_GL_HWIDGET timer = GL_Widget_SearchId(review_tx, REVIEW_TX_TIMEOUT_TIMER);

  if (!review_tx_type_id || !review_card_type_id || !review_masked_pan_id
      || !review_masked_expiry_date_id || !review_tx_amount_id
      || !review_auth_code_id || !review_tx_stan_id || !timer) {
    MessageDisplayResourceNotFound(resource_name);
    return false;
  }

  std::string amount = tx_detail.amount.GetCurrencyName() + " "
      + tx_detail.amount.ToString();

  GL_Widget_SetText(review_tx_type_id, tx_detail.tx_name.c_str());
  GL_Widget_SetText(review_card_type_id, tx_detail.card_name.c_str());
  GL_Widget_SetText(review_masked_pan_id, tx_detail.masked_pan.c_str());
  GL_Widget_SetText(review_masked_expiry_date_id,
                    tx_detail.masked_expiry_date.c_str());
  GL_Widget_SetText(review_tx_amount_id, amount.c_str());
  GL_Widget_SetText(review_auth_code_id, tx_detail.auth_code.c_str());
  GL_Widget_SetText(review_tx_stan_id, stdx::to_string(tx_detail.stan).c_str());

  GL_Timer_SetInterval(timer, screen_timeout);
  GL_Widget_RegisterCallback(timer, GL_EVENT_TIMER_OUT, OnTimeout);
  GL_Widget_RegisterCallback(review_tx, GL_EVENT_KEY_DOWN, OnKeyPressed);

  ulong result = GL_Window_MainLoop(review_tx);

  logger::debug("GL_Window_MainLoop result[%X]",result);
  GL_Widget_Destroy(review_tx);

  if(result != GL_KEY_VALID) {
    MessageProcessAborted();
    return false;
  }
  return true;
}

}
