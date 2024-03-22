#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan.h>

#include "buffer.h"
#include "device.h"
#include "log.h"

struct mc_Program {
    mc_Instance* _instance;
    const char* entryPoint;
    mc_DeviceInfo devi;
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

static void mc_program_clear(mc_Program* program) {
    DEBUG(program, "clearing program");
    VkDevice dev = program->devi.dev;
    if (program->cmdBuff)
        vkFreeCommandBuffers(dev, program->cmdPool, 1, &program->cmdBuff);
    if (program->cmdPool) //
        vkDestroyCommandPool(dev, program->cmdPool, NULL);
    if (program->descSet)
        vkFreeDescriptorSets(dev, program->descPool, 1, &program->descSet);
    if (program->descPool) //
        vkDestroyDescriptorPool(dev, program->descPool, NULL);
    if (program->pipeline) //
        vkDestroyPipeline(dev, program->pipeline, NULL);
    if (program->pipelineLayout)
        vkDestroyPipelineLayout(dev, program->pipelineLayout, NULL);
    if (program->descSetLayout)
        vkDestroyDescriptorSetLayout(dev, program->descSetLayout, 0);
}

static void mc_program_setup(mc_Program* program) {
    DEBUG(program, "setting up program with %d buffer(s):", program->buffCount);

    mc_program_clear(program);

    VkDescriptorSetLayoutBinding* descBindings
        = malloc(sizeof *descBindings * program->buffCount);

    for (int32_t i = 0; i < program->buffCount; i++) {
        descBindings[i] = (VkDescriptorSetLayoutBinding){0};
        descBindings[i].binding = i;
        descBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descBindings[i].descriptorCount = 1;
        descBindings[i].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    }

    VkDescriptorSetLayoutCreateInfo descLayoutInfo = {0};
    descLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descLayoutInfo.bindingCount = program->buffCount;
    descLayoutInfo.pBindings = descBindings;

    if (vkCreateDescriptorSetLayout(
            program->devi.dev,
            &descLayoutInfo,
            NULL,
            &program->descSetLayout
        )) {
        ERROR(program, "failed to create descriptor set layout");
        free(descBindings);
        return;
    }

    free(descBindings);

    VkPipelineLayoutCreateInfo pipelineInfo = {0};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineInfo.setLayoutCount = 1;
    pipelineInfo.pSetLayouts = &program->descSetLayout;

    if (vkCreatePipelineLayout(
            program->devi.dev,
            &pipelineInfo,
            NULL,
            &program->pipelineLayout
        )) {
        ERROR(program, "failed to create pipeline layout");
        return;
    }

    VkPipelineShaderStageCreateInfo shaderStageInfo = {0};
    shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    shaderStageInfo.module = program->shaderModule;
    shaderStageInfo.pName = program->entryPoint;

    VkComputePipelineCreateInfo computePipelineInfo = {0};
    computePipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    computePipelineInfo.stage = shaderStageInfo;
    computePipelineInfo.layout = program->pipelineLayout;

    if (vkCreateComputePipelines(
            program->devi.dev,
            0,
            1,
            &computePipelineInfo,
            NULL,
            &program->pipeline
        )) {
        ERROR(program, "failed to create compute pipeline");
        return;
    }

    VkDescriptorPoolSize descPoolSize = {0};
    descPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descPoolSize.descriptorCount = program->buffCount;

    VkDescriptorPoolCreateInfo descPoolInfo = {0};
    descPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descPoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    descPoolInfo.maxSets = 1;
    descPoolInfo.poolSizeCount = 1;
    descPoolInfo.pPoolSizes = &descPoolSize;

    if (vkCreateDescriptorPool(
            program->devi.dev,
            &descPoolInfo,
            NULL,
            &program->descPool
        )) {
        ERROR(program, "failed to create descriptor pool");
        return;
    }

    VkDescriptorSetAllocateInfo descAllocInfo = {0};
    descAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descAllocInfo.descriptorPool = program->descPool;
    descAllocInfo.descriptorSetCount = 1;
    descAllocInfo.pSetLayouts = &program->descSetLayout;

    if (vkAllocateDescriptorSets(
            program->devi.dev,
            &descAllocInfo,
            &program->descSet
        )) {
        ERROR(program, "failed to allocate descriptor sets");
        return;
    }

    VkCommandPoolCreateInfo cmdPoolInfo = {0};
    cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    cmdPoolInfo.queueFamilyIndex = program->devi.idx;

    if (vkCreateCommandPool(
            program->devi.dev,
            &cmdPoolInfo,
            NULL,
            &program->cmdPool
        )) {
        ERROR(program, "failed to create command pool");
        return;
    }

    VkDescriptorBufferInfo* descBuffInfo
        = malloc(sizeof *descBuffInfo * program->buffCount);
    VkWriteDescriptorSet* wrtDescSet
        = malloc(sizeof *wrtDescSet * program->buffCount);

    for (int32_t i = 0; i < program->buffCount; i++) {
        mc_Buffer* buffer = program->buffs[i];
        DEBUG(program, "- buffer %d: size=%ld", i, mc_buffer_get_size(buffer));

        descBuffInfo[i] = (VkDescriptorBufferInfo){0};
        descBuffInfo[i].buffer = mc_buffer_get_vk_buffer(buffer);
        descBuffInfo[i].range = VK_WHOLE_SIZE;

        wrtDescSet[i] = (VkWriteDescriptorSet){0};
        wrtDescSet[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        wrtDescSet[i].dstSet = program->descSet;
        wrtDescSet[i].dstBinding = i;
        wrtDescSet[i].descriptorCount = 1;
        wrtDescSet[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        wrtDescSet[i].pBufferInfo = &descBuffInfo[i];
    }

    vkUpdateDescriptorSets(
        program->devi.dev,
        program->buffCount,
        wrtDescSet,
        0,
        NULL
    );
    free(descBuffInfo);
    free(wrtDescSet);

    VkCommandBufferAllocateInfo cmdBuffAllocInfo = {0};
    cmdBuffAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBuffAllocInfo.commandPool = program->cmdPool;
    cmdBuffAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdBuffAllocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(
            program->devi.dev,
            &cmdBuffAllocInfo,
            &program->cmdBuff
        )) {
        ERROR(program, "failed to allocate command buffers");
        return;
    }
}

mc_Program* mc_program_create(
    mc_Device* device,
    size_t codeSize,
    const char* code,
    const char* entryPoint
) {
    if (!device) return NULL;

    mc_Program* program = malloc(sizeof *program);
    *program = (mc_Program){0};
    program->_instance = mc_device_get_instance(device);
    program->devi = mc_device_get_info(device);
    program->entryPoint = entryPoint;
    program->buffCount = -1;

    DEBUG(program, "initializing program, entry point: %s", entryPoint);

    VkShaderModuleCreateInfo moduleInfo = {0};
    moduleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleInfo.codeSize = codeSize;
    moduleInfo.pCode = (const uint32_t*)code;

    if (vkCreateShaderModule(
            program->devi.dev,
            &moduleInfo,
            NULL,
            &program->shaderModule
        )) {
        ERROR(program, "failed to create vulkan shader module");
        mc_program_destroy(program);
        return NULL;
    }

    return program;
}

void mc_program_destroy(mc_Program* program) {
    if (!program) return;
    DEBUG(program, "destroying program");

    mc_program_clear(program);
    if (program->shaderModule)
        vkDestroyShaderModule(program->devi.dev, program->shaderModule, NULL);
    free(program);
}

double mc_program_run__(
    mc_Program* program,
    uint32_t dimX,
    uint32_t dimY,
    uint32_t dimZ,
    ...
) {
    if (!program) return -1.0;
    DEBUG(program, "running %dx%dx%d program", dimX, dimY, dimZ);

    if (dimX * dimY * dimZ == 0) {
        ERROR(program, "at least one dimension is 0");
        return -1.0;
    }

    // check if the buffers have been changed
    int32_t buffCount = 0;
    va_list args;
    va_start(args, dimZ);
    while (va_arg(args, mc_Buffer*)) buffCount++;
    va_end(args);

    bool buffersChanged = buffCount != program->buffCount;

    if (buffCount != program->buffCount) {
        program->buffCount = buffCount;
        program->buffs
            = realloc(program->buffs, sizeof *program->buffs * buffCount);
        memset(program->buffs, 0, sizeof *program->buffs * buffCount);
    }

    va_start(args, dimZ);
    for (int32_t i = 0; i < buffCount; i++) {
        mc_Buffer* buff = va_arg(args, mc_Buffer*);
        if (buff != program->buffs[i]) {
            program->buffs[i] = buff;
            buffersChanged = true;
        }
    }
    va_end(args);

    if (buffersChanged) mc_program_setup(program);

    VkCommandBufferBeginInfo cmdBuffBeginInfo = {0};
    cmdBuffBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBuffBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (vkBeginCommandBuffer(program->cmdBuff, &cmdBuffBeginInfo)) {
        ERROR(program, "failed to begin command buffer");
        return -1.0;
    }

    vkCmdBindPipeline(
        program->cmdBuff,
        VK_PIPELINE_BIND_POINT_COMPUTE,
        program->pipeline
    );

    vkCmdBindDescriptorSets(
        program->cmdBuff,
        VK_PIPELINE_BIND_POINT_COMPUTE,
        program->pipelineLayout,
        0,
        1,
        &program->descSet,
        0,
        NULL
    );

    vkCmdDispatch(program->cmdBuff, dimX, dimY, dimZ);
    if (vkEndCommandBuffer(program->cmdBuff)) {
        ERROR(program, "failed to end command buffer");
        return -1.0;
    }

    VkQueue queue;
    vkGetDeviceQueue(program->devi.dev, program->devi.idx, 0, &queue);

    VkSubmitInfo submitInfo = {0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &program->cmdBuff;

    if (vkQueueSubmit(queue, 1, &submitInfo, 0)) {
        ERROR(program, "failed to submit queue");
        return -1.0;
    }

    double startTime = mc_get_time();
    if (vkQueueWaitIdle(queue)) {
        ERROR(program, "failed to wait for queue completion");
        return -1.0;
    }

    return mc_get_time() - startTime;
}