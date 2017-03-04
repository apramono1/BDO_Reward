#ifndef BDOR__TC_PROCESSING_H_
#define BDOR__TC_PROCESSING_H_

#include <bdor/application_settings.h>
#include <bdor/batch.h>
#include <bdor/host_switch.h>

namespace bdor {

bool ProcessAllPendingTcs(Batch& batch,
                          HostSwitch& host,
                          unsigned int host_index);

bool ProcessCurrentTc(Batch& batch,
                      HostSwitch& host,
                      unsigned int host_index);

}

#endif
