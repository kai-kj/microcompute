#ifndef MC_DEVICE_H
#define MC_DEVICE_H

#include <vulkan/vulkan.h>

#include "microcompute.h"

struct mc_Device {
    mc_Instance* _instance;
    VkPhysicalDevice physDev;
    uint32_t queueFamilyIdx;
    VkDevice dev;
    mc_DeviceType type;
    uint32_t maxWgSizeTotal;
    uint32_t maxWgSizeShape[3];
    uint32_t maxWgCount[3];
    char devName[256];
};

mc_Device* mc_device_create(
    mc_Instance* instance,
    VkPhysicalDevice physDev,
    uint32_t queueFamilyIdx
);

void mc_device_destroy(mc_Device* device);

#endif // MC_DEVICE_H