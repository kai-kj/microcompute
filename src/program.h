#ifndef MC_PROGRAM_H
#define MC_PROGRAM_H

#include <vulkan/vulkan.h>

#include "microcompute.h"

struct mc_Program {
    mc_Instance* _instance;
    const char* entryPoint;
    mc_Device* device;
    int32_t buffCount;
    mc_Buffer** buffs;
    VkShaderModule shaderModule;
    VkDescriptorSetLayout descSetLayout;
    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;
    VkDescriptorPool descPool;
    VkDescriptorSet descSet;
    VkCommandPool cmdPool;
    VkCommandBuffer cmdBuff;
};

#endif // MC_PROGRAM_H