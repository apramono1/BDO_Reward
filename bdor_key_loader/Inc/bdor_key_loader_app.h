/*
 ------------------------------------------------------------------------------
 INGENICO Technical Software Department
 ------------------------------------------------------------------------------
 Copyright (c) 2015 INGENICO S.A.
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

#ifndef ATOS_KEY_LOADER_APP_H_
#define ATOS_KEY_LOADER_APP_H_

#include <tpcore/telium_app.h>
#include <tpcore/with_more_function.h>
#include <tpcore/with_after_reset.h>
#include <tpcore/goal_handle.h>

#include "menu.h"

class FdmsKeyLoaderApp : public tpcore::TeliumApp,
    public tpcore::WithMoreFunction, public tpcore::WithAfterReset {
 public:
  FdmsKeyLoaderApp(int app_no, int app_type);
  virtual ~FdmsKeyLoaderApp() {
  }

  // virtual variant of TeliumApp methods for event interfaces.
  virtual const char *AppName() const;
  virtual unsigned char AppNo() const;

  // event specializations
  virtual void State();
  virtual bool IsDeleteAllowed();
  virtual bool IsEvolAllowed();

  virtual void More();

  virtual void FirstReset(bool is_remote_download);
  virtual void NormalReset();

 private:
  tpcore::GoalHandle gl_;
  Menu menu_;
};

#endif
