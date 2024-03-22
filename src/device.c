#include <stdlib.h>
#include <string.h>

#include "device.h"
#include "log.h"

uint32_t defaultReturn[] = {0, 0, 0};

mc_Device* mc_device_create(
    mc_Instance* instance,
    VkPhysicalDevice physDev,
    uint32_t queueFamilyIdx
) {
    if (!instance) return NULL;

    mc_Device* device = malloc(sizeof *device);
    *device = (mc_Device){
        ._instance = instance,
        .physDev = physDev,
        .queueFamilyIdx = queueFamilyIdx,
        .dev = NULL,
        .type = MC_DEVICE_TYPE_OTHER,
        .maxWgSizeTotal = 0,
        .maxWgSizeShape = {0, 0, 0},
        .maxWgCount = {0, 0, 0},
        .devName = {0},
    };

    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo devQueueInfo = {0};
    devQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    devQueueInfo.queueFamilyIndex = device->queueFamilyIdx;
    devQueueInfo.queueCount = 1;
    devQueueInfo.pQueuePriorities = &queuePriority;

    VkDeviceCreateInfo devInfo = {0};
    devInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    devInfo.queueCreateInfoCount = 1;
    devInfo.pQueueCreateInfos = &devQueueInfo;

    if (vkCreateDevice(device->physDev, &devInfo, NULL, &device->dev)) {
        ERROR(device, "failed to create device");
        mc_device_destroy(device);
        return NULL;
    }

    VkPhysicalDeviceProperties devProps;
    vkGetPhysicalDeviceProperties(device->physDev, &devProps);

    switch (devProps.deviceType) {
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            device->type = MC_DEVICE_TYPE_IGPU;
            break;
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            device->type = MC_DEVICE_TYPE_DGPU;
            break;
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            device->type = MC_DEVICE_TYPE_VGPU;
            break;
        case VK_PHYSICAL_DEVICE_TYPE_CPU:
            device->type = MC_DEVICE_TYPE_CPU;
            break;
        default: //
            device->type = MC_DEVICE_TYPE_OTHER;
            break;
    }

    device->maxWgSizeTotal = devProps.limits.maxComputeWorkGroupInvocations;

    memcpy(
        device->maxWgSizeShape,
        devProps.limits.maxComputeWorkGroupSize,
        sizeof devProps.limits.maxComputeWorkGroupSize
    );

    memcpy(
        device->maxWgCount,
        devProps.limits.maxComputeWorkGroupCount,
        sizeof devProps.limits.maxComputeWorkGroupCount
    );

    memcpy(device->devName, devProps.deviceName, sizeof devProps.deviceName);

    return device;
}

void mc_device_destroy(mc_Device* device) {
    if (!device) return;
    ERROR(device, "destroying device");
    if (device->dev) vkDestroyDevice(device->dev, NULL);
    free(device);
}

mc_DeviceType mc_device_get_type(mc_Device* device) {
    return device ? device->type : MC_DEVICE_TYPE_OTHER;
}

uint32_t mc_device_get_max_workgroup_size_total(mc_Device* device) {
    return device ? device->maxWgSizeTotal : 0;
}

uint32_t* mc_device_get_max_workgroup_size_shape(mc_Device* device) {
    return device ? device->maxWgSizeShape : defaultReturn;
}

uint32_t* mc_device_get_max_workgroup_count(mc_Device* device) {
    return device ? device->maxWgCount : defaultReturn;
}

char* mc_device_get_name(mc_Device* device) {
    return device ? device->devName : NULL;
}