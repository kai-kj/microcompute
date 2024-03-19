#ifndef MC_INTERNAL_H
#define MC_INTERNAL_H

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <vulkan/vulkan.h>

#include "microcompute.h"

#define MC_LOG(s, l, ...)                                                      \
    if ((s)->log_cb)                                                           \
        (s)->log_cb((l), "mc", (s)->logArg, __FILE__, __LINE__, __VA_ARGS__);

#define MC_LOG_DEBUG(s, ...) MC_LOG((s), MC_LOG_LEVEL_DEBUG, __VA_ARGS__)
#define MC_LOG_INFO(s, ...) MC_LOG((s), MC_LOG_LEVEL_INFO, __VA_ARGS__)
#define MC_LOG_WARN(s, ...) MC_LOG((s), MC_LOG_LEVEL_WARN, __VA_ARGS__)
#define MC_LOG_ERROR(s, ...) MC_LOG((s), MC_LOG_LEVEL_ERROR, __VA_ARGS__)

struct mc_Instance {
    void* logArg;
    mc_log_cb* log_cb;
    VkInstance instance;
    uint32_t devCount;
    mc_Device_t** devs;
#ifdef MC_ENABLE_VALIDATION_LAYER
    VkDebugUtilsMessengerEXT msg;
#endif // MC_ENABLE_VALIDATION_LAYER
};

struct mc_Device {
    void* logArg;
    mc_log_cb* log_cb;
    VkPhysicalDevice physDev;
    uint32_t queueFamilyIdx;
    VkDevice dev;
    mc_DeviceType_t type;
    uint32_t maxWorkgroupSizeTotal;
    uint32_t maxWorkgroupSizeShape[3];
    uint32_t maxWorkgroupCount[3];
    uint64_t memTot;
    uint64_t memUse;
    char devName[256];
};

struct mc_Buffer {
    mc_Device_t* dev;
    uint64_t size;
    void* map;
    VkBuffer buf;
    VkDeviceMemory mem;
};

struct mc_Program {
    const char* entryPoint;
    mc_Device_t* dev;
    int32_t buffCount;
    mc_Buffer_t** buffs;
    VkShaderModule shaderModule;
    VkDescriptorSetLayout descSetLayout;
    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;
    VkDescriptorPool descPool;
    VkDescriptorSet descSet;
    VkCommandPool cmdPool;
    VkCommandBuffer cmdBuff;
};

#endif