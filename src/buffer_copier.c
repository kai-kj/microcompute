#include <stdlib.h>

#include "buffer.h"
#include "buffer_copier.h"
#include "device.h"
#include "log.h"

mc_BufferCopier* mc_buffer_copier_create(mc_Device* device) {
    if (!device) return NULL;

    mc_BufferCopier* copier = malloc(sizeof(mc_BufferCopier));
    *copier = (mc_BufferCopier){
        ._instance = device->_instance,
        .device = device,
        .cmdPool = NULL,
    };

    VkCommandPoolCreateInfo cmdPoolInfo = {0};
    cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    cmdPoolInfo.queueFamilyIndex = copier->device->queueFamilyIdx;

    if (vkCreateCommandPool(
            copier->device->dev,
            &cmdPoolInfo,
            NULL,
            &copier->cmdPool
        )) {
        ERROR(copier, "failed to create command pool");
        mc_buffer_copier_destroy(copier);
        return NULL;
    }

    return copier;
}

void mc_buffer_copier_destroy(mc_BufferCopier* copier) {
    if (!copier) return;
    DEBUG(copier, "destroying buffer copier");

    if (copier->cmdPool)
        vkDestroyCommandPool(copier->device->dev, copier->cmdPool, NULL);
    free(copier);
}

uint64_t mc_buffer_copier_copy(
    mc_BufferCopier* copier,
    mc_Buffer* src,
    mc_Buffer* dst,
    uint64_t srcOffset,
    uint64_t dstOffset,
    uint64_t size
) {
    if (!copier || !src || !dst) return 0;
    DEBUG(copier, "copying %ld bytes", size);

    if (srcOffset + size > src->size || dstOffset + size > dst->size) {
        ERROR(copier, "offset + size > buffer size");
        return 0;
    }

    VkCommandBufferAllocateInfo cmdBufAllocI = {0};
    cmdBufAllocI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBufAllocI.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdBufAllocI.commandPool = copier->cmdPool;
    cmdBufAllocI.commandBufferCount = 1;

    VkCommandBuffer cmdBuf;
    if (vkAllocateCommandBuffers(copier->device->dev, &cmdBufAllocI, &cmdBuf)) {
        ERROR(copier, "failed to allocate command buffer");
        return 0;
    }

    VkCommandBufferBeginInfo cmdBufBeginI = {0};
    cmdBufBeginI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBufBeginI.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (vkBeginCommandBuffer(cmdBuf, &cmdBufBeginI)) {
        ERROR(copier, "failed to begin command buffer");
        return 0;
    }

    VkBufferCopy copyRegion = {0};
    copyRegion.srcOffset = srcOffset;
    copyRegion.dstOffset = dstOffset;
    copyRegion.size = size;
    vkCmdCopyBuffer(cmdBuf, src->buf, dst->buf, 1, &copyRegion);

    if (vkEndCommandBuffer(cmdBuf)) {
        ERROR(copier, "failed to end command buffer");
        return 0;
    }

    VkQueue queue;
    vkGetDeviceQueue(
        copier->device->dev,
        copier->device->queueFamilyIdx,
        0,
        &queue
    );

    VkSubmitInfo submitI = {0};
    submitI.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitI.commandBufferCount = 1;
    submitI.pCommandBuffers = &cmdBuf;

    if (vkQueueSubmit(queue, 1, &submitI, VK_NULL_HANDLE)) {
        ERROR(copier, "failed to submit queue");
        return 0;
    }

    if (vkQueueWaitIdle(queue)) {
        ERROR(copier, "failed to wait for queue");
        return 0;
    }

    vkFreeCommandBuffers(copier->device->dev, copier->cmdPool, 1, &cmdBuf);

    return size;
}