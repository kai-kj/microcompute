#ifndef MC_INSTANCE_H
#define MC_INSTANCE_H

#include "microcompute.h"

mc_log_fn* mc_instance_get_log_fn(mc_Instance* instance);

void* mc_instance_get_log_arg(mc_Instance* instance);

#endif // MC_INSTANCE_H
