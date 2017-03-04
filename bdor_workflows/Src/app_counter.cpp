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
#include <app_counter.h>

#include <stdx/string>
#include <idalloc/idalloc.h>

namespace bdor {

const std::string keyIdAllocInvoiceNo = "InvoiceNo";
const std::string keyIdAllocStanNo = "StanNo";
const std::string keyIdPostfix = "Global";
const uint32_t fMinCounterValue = 1;
const uint32_t fMaxCounterValue = 999999;

int GetNextInvoiceNo() {
  if (idalloc::IsCounter(GetInvoiceKey()) == false)
    idalloc::InitCounter(GetInvoiceKey(), fMinCounterValue, fMaxCounterValue);

  return idalloc::NextValue(GetInvoiceKey());
}

std::string GetInvoiceKey() {
  return keyIdAllocInvoiceNo + keyIdPostfix;
}

int GetNextStanNo() {
  if (idalloc::IsCounter(GetStanKey()) == false)
    idalloc::InitCounter(GetStanKey(), fMinCounterValue, fMaxCounterValue);

  return idalloc::NextValue(GetStanKey());
}

std::string GetStanKey() {
  return keyIdAllocStanNo + keyIdPostfix;
}

void SetInvoiceNo(std::uint32_t invoce_num) {
  idalloc::SetValue(GetInvoiceKey(), invoce_num);
}

void SetStanNo(std::uint32_t stan_num){
  idalloc::SetValue(GetStanKey(), stan_num);
}

}
