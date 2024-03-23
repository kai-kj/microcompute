#ifndef BUFFER_COPIER_H
#define BUFFER_COPIER_H

#include <vulkan/vulkan.h>

#include "microcompute.h"

struct mc_BufferCopier {
    mc_Instance* _instance;
    mc_Device* device;
    VkCommandPool cmdPool;
};

#endif