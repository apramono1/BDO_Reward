#include <tpcore/telium_keyboard_guard.h>
#include <bdor/goal_ui.h>
#include "goal_res.h"

namespace bdor {

static bool OnTimeout(T_GL_HMESSAGE message) {
  GL_Message_SetResult(message, 0);
  GL_Timer_Stop(GL_Message_GetWidget(message));
  return true;
}

static bool OnKeyPressed(T_GL_HMESSAGE message) {
  T_GL_WCHAR ulKey;
  ulKey = GL_Message_GetKey(message);
  GL_Message_SetUserData(message, &ulKey, sizeof(ulKey));
  GL_Message_SetResult(message, ulKey);
  return true;
}

BatchTotalReturn GoalUi::DisplayBatchTotal(BatchTotalToDisplay batch_total) {
  tpcore::TeliumKeyboardGuard keyboard_guard;

  T_GL_HWIDGET batch_total_single = GL_GraphicLib_ReadResource(
      gl_.GetHandle(), GetDAL(RES_BATCH_TOTAL__SCREEN).c_str());

  T_GL_HWIDGET label_host_name = GL_Widget_SearchId(batch_total_single,
                                                    BATCH_TOTAL_HOST_NAME);

  T_GL_HWIDGET label_sales_count = GL_Widget_SearchId(batch_total_single,
                                                      BATCH_TOTAL_SALE_COUNT);

  T_GL_HWIDGET label_sales_total = GL_Widget_SearchId(batch_total_single,
                                                      BATCH_TOTAL_SALES_TOTAL);

  T_GL_HWIDGET label_refund_count = GL_Widget_SearchId(
      batch_total_single, BATCH_TOTAL_REFUND_COUNT);

  T_GL_HWIDGET label_refund_total = GL_Widget_SearchId(
      batch_total_single, BATCH_TOTAL_REFUND_TOTAL);

  T_GL_HWIDGET label_grand_count = GL_Widget_SearchId(batch_total_single,
                                                      BATCH_TOTAL_TOTAL_COUNT);

  T_GL_HWIDGET label_grand_total = GL_Widget_SearchId(batch_total_single,
                                                      BATCH_TOTAL_GRAND_TOTAL);

  GL_Widget_SetText(label_host_name, batch_total.host_name.c_str());

  types::Amount sales_total(batch_total.currency,
                            batch_total.sales_total.total);
  types::Amount refund_total(batch_total.currency,
                             batch_total.refunds_total.total);
  types::Amount grand_total(batch_total.currency, batch_total.total.total);

  GL_Widget_SetText(
                    label_sales_count,
                    (stdx::to_string(batch_total.sales_total.count).c_str()));
  GL_Widget_SetText(
      label_sales_total,
      (batch_total.currency + " " + sales_total.ToString()).c_str());
  GL_Widget_SetText(
                    label_refund_count,
                    (stdx::to_string(batch_total.refunds_total.count).c_str()));
  GL_Widget_SetText(
      label_refund_total,
      (batch_total.currency + " " + refund_total.ToString()).c_str());
  GL_Widget_SetText(
                    label_grand_count,
                    (stdx::to_string(batch_total.total.count).c_str()));
  GL_Widget_SetText(
      label_grand_total,
      (batch_total.currency + " " + grand_total.ToString()).c_str());

  T_GL_HWIDGET hTimer = GL_Widget_SearchId(batch_total_single,
                                           BATCH_TOTAL_TIMEOUT_TIMER);

  if (!batch_total_single || !label_host_name || !label_sales_count
      || !label_sales_total || !label_refund_count
      || !label_refund_total || !label_grand_count || !label_grand_total
      || !hTimer) {
    std::string resource_name(RES_BATCH_TOTAL__SCREEN);
    MessageDisplayResourceNotFound(resource_name);
    return BatchTotalReturn::RESOURCE_NOT_FOUND;
  }

  GL_Timer_SetInterval(hTimer, screen_timeout);
  GL_Widget_RegisterCallback(hTimer, GL_EVENT_TIMER_OUT, OnTimeout);
  GL_Widget_RegisterCallback(batch_total_single, GL_EVENT_KEY_DOWN,
                             OnKeyPressed);

  ulong result = GL_Window_MainLoop(batch_total_single);

  GL_Widget_Destroy(batch_total_single);

  if (result == GL_KEY_F)
    return BatchTotalReturn::PRINT_REPORT;

  if (result != GL_KEY_VALID)
    return BatchTotalReturn::OTHER_KEY_PRESSED;

  return BatchTotalReturn::ENTER_KEY_PRESSED;
}

}
