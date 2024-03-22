#ifndef MC_BUFFER_H
#define MC_BUFFER_H

#include <vulkan/vulkan.h>

#include "microcompute.h"

struct mc_Buffer {
    mc_Instance* _instance;
    mc_Device* device;
    mc_BufferType type;
    uint64_t size;
    void* map;
    VkBuffer buf;
    VkDeviceMemory mem;
};

#endif // MC_BUFFER_H