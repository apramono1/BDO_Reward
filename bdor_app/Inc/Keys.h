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
#ifndef KEYS_CONFIG_H_
#define KEYS_CONFIG_H_

#include <map>
#include <types/capk.h>

namespace bdor {

std::map<unsigned int, types::Capk> GetVisaLiveCapks();
std::map<unsigned int, types::Capk> GetVisaTestCapks();

std::map<unsigned int, types::Capk> GetMasterCardLiveCapks();
std::map<unsigned int, types::Capk> GetMasterCardTestCapks();

std::map<unsigned int, types::Capk> GetJcbLiveCapks();
std::map<unsigned int, types::Capk> GetJcbTestCapks();

std::map<unsigned int, types::Capk> GetCupLiveCapks();
std::map<unsigned int, types::Capk> GetCupTestCapks();

std::map<unsigned int, types::Capk> GetAmexTestCapks();

}

#endif
