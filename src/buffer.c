#include <stdlib.h>
#include <string.h>

#include "buffer.h"
#include "device.h"
#include "log.h"

mc_Buffer* mc_buffer_create(
    mc_Device* device,
    mc_BufferType type,
    uint64_t size
) {
    if (!device) return NULL;

    mc_Buffer* buffer = malloc(sizeof *buffer);
    *buffer = (mc_Buffer){
        ._instance = device->_instance,
        .device = device,
        .type = type,
        .size = size,
        .map = NULL,
        .buf = NULL,
        .mem = NULL,
    };

    DEBUG(buffer, "initializing buffer of size %ld", size);

    VkBufferCreateInfo bufferInfo = {0};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = buffer->size;
    bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferInfo.queueFamilyIndexCount = 1;
    bufferInfo.pQueueFamilyIndices = &buffer->device->queueFamilyIdx;

    if (vkCreateBuffer(buffer->device->dev, &bufferInfo, NULL, &buffer->buf)) {
        ERROR(buffer, "failed to create vulkan buffer");
        mc_buffer_destroy(buffer);
        return NULL;
    }

    // check the minimum memory size
    VkMemoryRequirements memReqs;
    vkGetBufferMemoryRequirements(buffer->device->dev, buffer->buf, &memReqs);
    if (memReqs.size > buffer->size) buffer->size = memReqs.size;

    VkPhysicalDeviceMemoryProperties memProps;
    vkGetPhysicalDeviceMemoryProperties(buffer->device->physDev, &memProps);

    uint32_t bestMemTypeIdx = memProps.memoryTypeCount;
    uint32_t bestMemTypeScore = 0;

    for (uint32_t i = 0; i < memProps.memoryTypeCount; i++) {
        VkMemoryType memType = memProps.memoryTypes[i];
        VkMemoryHeap heap = memProps.memoryHeaps[memType.heapIndex];
        bool v = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT & memType.propertyFlags;
        bool c = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT & memType.propertyFlags;
        bool d = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT & memType.propertyFlags;

        uint32_t score = 0;
        switch (type) {
            case MC_BUFFER_TYPE_CPU: score = v && c; break;
            case MC_BUFFER_TYPE_GPU: score = d + (d && v && c); break;
        }

        score *= heap.size;
        score *= memReqs.size <= heap.size;

        DEBUG(
            buffer,
            "- found mem type %d: %c%c%c, %ld",
            i,
            v ? 'V' : '_',
            c ? 'C' : '_',
            d ? 'D' : '_',
            heap.size
        );

        if (score > bestMemTypeScore) {
            bestMemTypeIdx = i;
            bestMemTypeScore = score;
        }
    }

    if (bestMemTypeIdx == memProps.memoryTypeCount) {
        ERROR(buffer, "no suitable memory type found");
        mc_buffer_destroy(buffer);
        return NULL;
    }

    VkMemoryAllocateInfo memAllocInfo = {0};
    memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAllocInfo.allocationSize = buffer->size;
    memAllocInfo.memoryTypeIndex = bestMemTypeIdx;

    if (vkAllocateMemory(
            buffer->device->dev,
            &memAllocInfo,
            NULL,
            &buffer->mem
        )) {
        ERROR(buffer, "failed to allocate vulkan memory");
        mc_buffer_destroy(buffer);
        return NULL;
    }

    if (vkMapMemory(
            buffer->device->dev,
            buffer->mem,
            0,
            buffer->size,
            0,
            &buffer->map
        )) {
        ERROR(buffer, "failed to map memory");
        mc_buffer_destroy(buffer);
        return NULL;
    }

    if (vkBindBufferMemory(buffer->device->dev, buffer->buf, buffer->mem, 0)) {
        ERROR(buffer, "failed to bind memory");
        mc_buffer_destroy(buffer);
        return NULL;
    }

    return buffer;
}

void mc_buffer_destroy(mc_Buffer* buffer) {
    if (!buffer) return;
    DEBUG(buffer, "destroying buffer");
    if (buffer->mem) vkFreeMemory(buffer->device->dev, buffer->mem, NULL);
    if (buffer->buf) vkDestroyBuffer(buffer->device->dev, buffer->buf, NULL);
    free(buffer);
}

uint64_t mc_buffer_get_size(mc_Buffer* self) {
    return self ? self->size : 0;
}

mc_Buffer* mc_buffer_realloc(mc_Buffer* buffer, uint64_t size) {
    if (!buffer) return NULL;
    DEBUG(buffer, "reallocating buffer: %ld -> %ld", buffer->size, size);

    mc_Buffer* new = mc_buffer_create(buffer->device, buffer->type, size);
    if (!new) return NULL;

    uint64_t minSize = size < buffer->size ? size : buffer->size;
    mc_buffer_write(new, 0, minSize, buffer->map);
    mc_buffer_destroy(buffer);
    return new;
}

uint64_t mc_buffer_write(
    mc_Buffer* buffer,
    uint64_t offset,
    uint64_t size,
    void* data
) {
    if (!buffer) return 0;
    DEBUG(buffer, "writing %ld bytes to buffer", size);

    if (offset + size > buffer->size) {
        ERROR(buffer, "offset + size > buffer size");
        return 0;
    }

    memcpy((char*)buffer->map + offset, data, size);
    return size;
}

uint64_t mc_buffer_read(
    mc_Buffer* buffer,
    uint64_t offset,
    uint64_t size,
    void* data
) {
    if (!buffer) return 0;
    DEBUG(buffer, "reading %ld bytes from buffer", size);

    if (offset + size > buffer->size) {
        ERROR(buffer, "offset + size > buffer size");
        return 0;
    }

    memcpy(data, (char*)buffer->map + offset, size);
    return size;
}