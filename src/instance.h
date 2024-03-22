#ifndef MC_INSTANCE_H
#define MC_INSTANCE_H

#include <vulkan/vulkan.h>

#include "microcompute.h"

struct mc_Instance {
    mc_Instance* _instance;
    void* logArg;
    mc_log_fn* log_fn;
    VkInstance instance;
    uint32_t devCount;
    mc_Device** devs;
    VkDebugUtilsMessengerEXT msg;
};

#endif // MC_INSTANCE_H
