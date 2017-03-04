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
#ifndef BDOR__TEST_TRANSACTION_H_
#define BDOR__TEST_TRANSACTION_H_

#include <string>
#include <ctime>

namespace bdor {

struct TestTransaction {
  std::string processing_code;
  time_t host_datetime;
  std::string tpdu;
  uint32_t nii;
  std::string tid;
  std::string mid;
};

}

#endif
