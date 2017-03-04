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
#include <utils/strings.h>

#include "goal_res.h"

namespace bdor {

void GoalUi::DisplayHostTestingProgress(int current_host_num,
                                        int total_host_number,
                                        int error_number) {
  T_GL_HWIDGET window = GL_GraphicLib_ReadResource(
      gl_.GetHandle(), GetDAL(RES_TEST_HOST__SCREEN).c_str());

  T_GL_HWIDGET label_progress = GL_Widget_SearchId(window,
                                                   TEST_HOST__PROGRESS_MESSAGE);
  T_GL_HWIDGET label_nb_error = GL_Widget_SearchId(window,
                                                   TEST_HOST__NB_ERROR_MESSAGE);

  std::string progress_msg = "Testing host: ";  // TODO: handle locale
  progress_msg += utils::ToString(current_host_num);
  progress_msg += "/";
  progress_msg += utils::ToString(total_host_number);

  std::string error_msg = "Errors: ";  // TODO: handle locale
  error_msg += utils::ToString(error_number);

  GL_Widget_SetText(label_progress, progress_msg.c_str());
  GL_Widget_SetText(label_nb_error, error_msg.c_str());

  GL_Window_Dispatch(window, 10);
  GL_Widget_Destroy(window);

}

void GoalUi::DisplayHostTestingResult(int total_host_number,
                                      int error_number) {
  T_GL_DIALOG_ICON icon = GL_ICON_INFORMATION;
  std::string message;
  std::string title = "Test Host Result";  // TODO: handle locale

  if (error_number > 0) {
    icon = GL_ICON_ERROR;
    message = utils::ToString(error_number) + " host error(s)";
  } else if (total_host_number == 0 ) {
    icon = GL_ICON_WARNING;
    message = "No host configured";
  } else {
    message = "All Hosts OK";
  }

  GL_Dialog_Message(gl_.GetHandle(), title.c_str(),
                    message.c_str(),
                    icon,
                    GL_BUTTON_NONE,
                    msg_duration_);
}

}
