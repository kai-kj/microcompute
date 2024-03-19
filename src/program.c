#include <vulkan/vulkan.h>

#include "microcompute_internal.h"

static void mc_program_setup(mc_Program_t* self) {
    // destroy old objects if they exist
    if (self->cmdBuff)
        vkFreeCommandBuffers(self->dev->dev, self->cmdPool, 1, &self->cmdBuff);
    if (self->cmdPool)
        vkDestroyCommandPool(self->dev->dev, self->cmdPool, NULL);
    if (self->descSet)
        vkFreeDescriptorSets(self->dev->dev, self->descPool, 1, &self->descSet);
    if (self->descPool)
        vkDestroyDescriptorPool(self->dev->dev, self->descPool, NULL);
    if (self->pipeline) //
        vkDestroyPipeline(self->dev->dev, self->pipeline, NULL);
    if (self->pipelineLayout)
        vkDestroyPipelineLayout(self->dev->dev, self->pipelineLayout, NULL);
    if (self->descSetLayout)
        vkDestroyDescriptorSetLayout(self->dev->dev, self->descSetLayout, 0);

    MC_LOG_DEBUG(
        self->dev,
        "setting up program with %d buffer(s):",
        self->buffCount
    );

    VkDescriptorSetLayoutBinding* descBindings
        = malloc(sizeof *descBindings * self->buffCount);

    for (int32_t i = 0; i < self->buffCount; i++) {
        descBindings[i] = (VkDescriptorSetLayoutBinding){0};
        descBindings[i].binding = i;
        descBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descBindings[i].descriptorCount = 1;
        descBindings[i].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    }

    VkDescriptorSetLayoutCreateInfo descLayoutInfo = {0};
    descLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descLayoutInfo.bindingCount = self->buffCount;
    descLayoutInfo.pBindings = descBindings;

    if (vkCreateDescriptorSetLayout(
            self->dev->dev,
            &descLayoutInfo,
            NULL,
            &self->descSetLayout
        )) {
        MC_LOG_ERROR(self->dev, "failed to create descriptor set layout");
        free(descBindings);
        return;
    }

    free(descBindings);

    VkPipelineLayoutCreateInfo pipelineInfo = {0};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineInfo.setLayoutCount = 1;
    pipelineInfo.pSetLayouts = &self->descSetLayout;

    if (vkCreatePipelineLayout(
            self->dev->dev,
            &pipelineInfo,
            NULL,
            &self->pipelineLayout
        )) {
        MC_LOG_ERROR(self->dev, "failed to create pipeline layout");
        return;
    }

    VkPipelineShaderStageCreateInfo shaderStageInfo = {0};
    shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    shaderStageInfo.module = self->shaderModule;
    shaderStageInfo.pName = self->entryPoint;

    VkComputePipelineCreateInfo computePipelineInfo = {0};
    computePipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    computePipelineInfo.stage = shaderStageInfo;
    computePipelineInfo.layout = self->pipelineLayout;

    if (vkCreateComputePipelines(
            self->dev->dev,
            0,
            1,
            &computePipelineInfo,
            NULL,
            &self->pipeline
        )) {
        MC_LOG_ERROR(self->dev, "failed to create compute pipeline");
        return;
    }

    VkDescriptorPoolSize descPoolSize = {0};
    descPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descPoolSize.descriptorCount = self->buffCount;

    VkDescriptorPoolCreateInfo descPoolInfo = {0};
    descPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descPoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    descPoolInfo.maxSets = 1;
    descPoolInfo.poolSizeCount = 1;
    descPoolInfo.pPoolSizes = &descPoolSize;

    if (vkCreateDescriptorPool(
            self->dev->dev,
            &descPoolInfo,
            NULL,
            &self->descPool
        )) {
        MC_LOG_ERROR(self->dev, "failed to create descriptor pool");
        return;
    }

    VkDescriptorSetAllocateInfo descAllocInfo = {0};
    descAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descAllocInfo.descriptorPool = self->descPool;
    descAllocInfo.descriptorSetCount = 1;
    descAllocInfo.pSetLayouts = &self->descSetLayout;

    if (vkAllocateDescriptorSets(
            self->dev->dev,
            &descAllocInfo,
            &self->descSet
        )) {
        MC_LOG_ERROR(self->dev, "failed to allocate descriptor sets");
        return;
    }

    VkCommandPoolCreateInfo cmdPoolInfo = {0};
    cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    cmdPoolInfo.queueFamilyIndex = self->dev->queueFamilyIdx;

    if (vkCreateCommandPool(
            self->dev->dev,
            &cmdPoolInfo,
            NULL,
            &self->cmdPool
        )) {
        MC_LOG_ERROR(self->dev, "failed to create command pool");
        return;
    }

    VkDescriptorBufferInfo* descBuffInfo
        = malloc(sizeof *descBuffInfo * self->buffCount);
    VkWriteDescriptorSet* wrtDescSet
        = malloc(sizeof *wrtDescSet * self->buffCount);

    for (int32_t i = 0; i < self->buffCount; i++) {
        mc_Buffer_t* buffer = self->buffs[i];
        MC_LOG_DEBUG(self->dev, "- buffer %d: size=%ld", i, buffer->size);

        descBuffInfo[i] = (VkDescriptorBufferInfo){0};
        descBuffInfo[i].buffer = buffer->buf;
        descBuffInfo[i].range = VK_WHOLE_SIZE;

        wrtDescSet[i] = (VkWriteDescriptorSet){0};
        wrtDescSet[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        wrtDescSet[i].dstSet = self->descSet;
        wrtDescSet[i].dstBinding = i;
        wrtDescSet[i].descriptorCount = 1;
        wrtDescSet[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        wrtDescSet[i].pBufferInfo = &descBuffInfo[i];
    }

    vkUpdateDescriptorSets(
        self->dev->dev,
        self->buffCount,
        wrtDescSet,
        0,
        NULL
    );
    free(descBuffInfo);
    free(wrtDescSet);

    VkCommandBufferAllocateInfo cmdBuffAllocInfo = {0};
    cmdBuffAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBuffAllocInfo.commandPool = self->cmdPool;
    cmdBuffAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdBuffAllocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(
            self->dev->dev,
            &cmdBuffAllocInfo,
            &self->cmdBuff
        )) {
        MC_LOG_ERROR(self->dev, "failed to allocate command buffers");
        return;
    }
}

mc_Program_t* mc_program_create(
    mc_Device_t* device,
    size_t codeSize,
    const char* code,
    const char* entryPoint
) {
    if (!device) return NULL;
    MC_LOG_DEBUG(device, "initializing program, entry point: %s", entryPoint);

    mc_Program_t* self = malloc(sizeof *self);
    *self = (mc_Program_t){0};
    self->dev = device;
    self->entryPoint = entryPoint;
    self->buffCount = -1;

    VkShaderModuleCreateInfo moduleInfo = {0};
    moduleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleInfo.codeSize = codeSize;
    moduleInfo.pCode = (const uint32_t*)code;

    if (vkCreateShaderModule(
            self->dev->dev,
            &moduleInfo,
            NULL,
            &self->shaderModule
        )) {
        MC_LOG_ERROR(self->dev, "failed to create vulkan shader module");
        mc_program_destroy(self);
        return NULL;
    }

    return self;
}

mc_Program_t* mc_program_create_from_file(
    mc_Device_t* device,
    const char* fileName,
    const char* entryPoint
) {
    if (!device) return NULL;
    MC_LOG_DEBUG(device, "initializing program from file: %s", fileName);

    size_t len = mc_read_file(fileName, NULL);
    if (len == 0) {
        MC_LOG_ERROR(device, "failed to read file: %s", fileName);
        return NULL;
    }

    char* code = malloc(len);
    mc_read_file(fileName, code);

    mc_Program_t* self = mc_program_create(device, len, code, entryPoint);
    free(code);
    return self;
}

void mc_program_destroy(mc_Program_t* self) {
    if (!self) return;
    MC_LOG_DEBUG(self->dev, "destroying program");

    if (self->cmdBuff)
        vkFreeCommandBuffers(self->dev->dev, self->cmdPool, 1, &self->cmdBuff);
    if (self->cmdPool)
        vkDestroyCommandPool(self->dev->dev, self->cmdPool, NULL);
    if (self->descSet)
        vkFreeDescriptorSets(self->dev->dev, self->descPool, 1, &self->descSet);
    if (self->descPool)
        vkDestroyDescriptorPool(self->dev->dev, self->descPool, NULL);
    if (self->pipeline) //
        vkDestroyPipeline(self->dev->dev, self->pipeline, NULL);
    if (self->pipelineLayout)
        vkDestroyPipelineLayout(self->dev->dev, self->pipelineLayout, NULL);
    if (self->descSetLayout)
        vkDestroyDescriptorSetLayout(self->dev->dev, self->descSetLayout, 0);
    if (self->shaderModule)
        vkDestroyShaderModule(self->dev->dev, self->shaderModule, NULL);
    free(self);
}

double mc_program_run__(
    mc_Program_t* self,
    uint32_t dimX,
    uint32_t dimY,
    uint32_t dimZ,
    ...
) {
    if (!self) return -1.0;
    MC_LOG_DEBUG(self->dev, "running %dx%dx%d program", dimX, dimY, dimZ);

    if (dimX * dimY * dimZ == 0) {
        MC_LOG_ERROR(self->dev, "at least one dimension is 0");
        return -1.0;
    }

    // check if the buffers have been changed
    int32_t buffCount = 0;
    va_list args;
    va_start(args, dimZ);
    while (va_arg(args, mc_Buffer_t*)) buffCount++;
    va_end(args);

    bool buffersChanged = buffCount != self->buffCount;

    if (buffCount != self->buffCount) {
        self->buffCount = buffCount;
        self->buffs = realloc(self->buffs, sizeof *self->buffs * buffCount);
        memset(self->buffs, 0, sizeof *self->buffs * buffCount);
    }

    va_start(args, dimZ);
    for (int32_t i = 0; i < buffCount; i++) {
        mc_Buffer_t* buff = va_arg(args, mc_Buffer_t*);
        if (buff != self->buffs[i]) {
            self->buffs[i] = buff;
            buffersChanged = true;
        }
    }
    va_end(args);

    if (buffersChanged) mc_program_setup(self);

    VkCommandBufferBeginInfo cmdBuffBeginInfo = {0};
    cmdBuffBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBuffBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (vkBeginCommandBuffer(self->cmdBuff, &cmdBuffBeginInfo)) {
        MC_LOG_ERROR(self->dev, "failed to begin command buffer");
        return -1.0;
    }

    vkCmdBindPipeline(
        self->cmdBuff,
        VK_PIPELINE_BIND_POINT_COMPUTE,
        self->pipeline
    );

    vkCmdBindDescriptorSets(
        self->cmdBuff,
        VK_PIPELINE_BIND_POINT_COMPUTE,
        self->pipelineLayout,
        0,
        1,
        &self->descSet,
        0,
        NULL
    );

    vkCmdDispatch(self->cmdBuff, dimX, dimY, dimZ);
    if (vkEndCommandBuffer(self->cmdBuff)) {
        MC_LOG_ERROR(self->dev, "failed to end command buffer");
        return -1.0;
    }

    VkQueue queue;
    vkGetDeviceQueue(self->dev->dev, self->dev->queueFamilyIdx, 0, &queue);

    VkSubmitInfo submitInfo = {0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &self->cmdBuff;

    if (vkQueueSubmit(queue, 1, &submitInfo, 0)) {
        MC_LOG_ERROR(self->dev, "failed to submit queue");
        return -1.0;
    }

    double startTime = mc_get_time();
    if (vkQueueWaitIdle(queue)) {
        MC_LOG_ERROR(self->dev, "failed to wait for queue completion");
        return -1.0;
    }

    return mc_get_time() - startTime;
}