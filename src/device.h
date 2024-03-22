#ifndef MC_DEVICE_H
#define MC_DEVICE_H

#include <vulkan/vulkan.h>

#include "microcompute.h"

typedef struct mc_DeviceInfo {
    mc_Instance* _instance;
    VkPhysicalDevice pDev;
    uint32_t idx;
    VkDevice dev;
} mc_DeviceInfo;

mc_Device* mc_device_create(
    mc_Instance* instance,
    VkPhysicalDevice physDev,
    uint32_t queueFamilyIdx
);

void mc_device_destroy(mc_Device* device);

mc_Instance* mc_device_get_instance(mc_Device* device);

mc_DeviceInfo mc_device_get_info(mc_Device* device);

#endif // MC_DEVICE_H