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

#include <stdint.h>
#include <vector>
#include "idalloc_proto.h"
#include "idalloc/idalloc.h"
#include "idalloc_message.pb-c.h"

namespace idalloc {

IdAllocMsg* SendRequest(IdAllocMsg& request_msg, const char* name) {
  return nullptr;
}

void InitCounter(const char* name, uint64_t initial, uint64_t max) {
}

uint64_t NextValue(const char* name) {
  return 2;
}

void SetValue(const char* name, uint64_t value) {
}

bool IsCounter(const char* name) {
  return true;
}

void DeleteCounter(const char* name) {
}

}
