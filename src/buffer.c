#include "microcompute_internal.h"

mc_Buffer_t* mc_buffer_create(mc_Device_t* device, uint64_t size) {
    if (!device) return NULL;
    MC_LOG_DEBUG(device, "initializing buffer of size %ld", size);

    mc_Buffer_t* self = malloc(sizeof *self);
    *self = (mc_Buffer_t){0};
    self->size = size;
    self->dev = device;

    if (self->dev->memUse + size > self->dev->memTot) {
        uint32_t remaining = self->dev->memTot - self->dev->memUse;
        MC_LOG_ERROR(
            self->dev,
            "not enough memory (available: %ld)",
            remaining
        );
        mc_buffer_destroy(self);
        return NULL;
    }

    self->dev->memUse += size;

    VkBufferCreateInfo bufferInfo = {0};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = self->size;
    bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferInfo.queueFamilyIndexCount = 1;
    bufferInfo.pQueueFamilyIndices = &self->dev->queueFamilyIdx;

    if (vkCreateBuffer(self->dev->dev, &bufferInfo, NULL, &self->buf)) {
        MC_LOG_ERROR(self->dev, "failed to create vulkan buffer");
        mc_buffer_destroy(self);
        return NULL;
    }

    // check the minimum memory size
    VkMemoryRequirements memReqs;
    vkGetBufferMemoryRequirements(self->dev->dev, self->buf, &memReqs);
    if (memReqs.size > self->size) self->size = memReqs.size;

    VkPhysicalDeviceMemoryProperties memProps;
    vkGetPhysicalDeviceMemoryProperties(self->dev->physDev, &memProps);

    uint32_t memTypeIdx = memProps.memoryTypeCount;
    for (uint32_t i = 0; i < memProps.memoryTypeCount; i++) {
        VkMemoryType type = memProps.memoryTypes[i];
        VkMemoryHeap heap = memProps.memoryHeaps[type.heapIndex];
        bool v = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT & type.propertyFlags;
        bool c = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT & type.propertyFlags;
        if (v && c && self->size < heap.size) {
            memTypeIdx = i;
            break; // just get the  first valid memory
        }
    }

    if (memTypeIdx == memProps.memoryTypeCount) {
        MC_LOG_ERROR(self->dev, "no suitable memory type found");
        mc_buffer_destroy(self);
        return NULL;
    }

    VkMemoryAllocateInfo memAllocInfo = {0};
    memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAllocInfo.allocationSize = self->size;
    memAllocInfo.memoryTypeIndex = memTypeIdx;

    if (vkAllocateMemory(self->dev->dev, &memAllocInfo, NULL, &self->mem)) {
        MC_LOG_ERROR(self->dev, "failed to allocate vulkan memory");
        mc_buffer_destroy(self);
        return NULL;
    }

    if (vkMapMemory(self->dev->dev, self->mem, 0, self->size, 0, &self->map)) {
        MC_LOG_ERROR(self->dev, "failed to map memory");
        mc_buffer_destroy(self);
        return NULL;
    }

    if (vkBindBufferMemory(self->dev->dev, self->buf, self->mem, 0)) {
        MC_LOG_ERROR(self->dev, "failed to bind memory");
        mc_buffer_destroy(self);
        return NULL;
    }

    return self;
}

mc_Buffer_t* mc_buffer_create_from(
    mc_Device_t* device,
    uint64_t size,
    void* data
) {
    if (!device) return NULL;

    mc_Buffer_t* self = mc_buffer_create(device, size);
    if (!self) return NULL;

    mc_buffer_write(self, 0, size, data);
    return self;
}

void mc_buffer_destroy(mc_Buffer_t* self) {
    if (!self) return;
    MC_LOG_DEBUG(self->dev, "destroying buffer");
    self->dev->memUse -= self->size;
    if (self->mem) vkFreeMemory(self->dev->dev, self->mem, NULL);
    if (self->buf) vkDestroyBuffer(self->dev->dev, self->buf, NULL);
    free(self);
}

uint64_t mc_buffer_get_size(mc_Buffer_t* self) {
    if (!self) return 0;
    return self->size;
}

mc_Buffer_t* mc_buffer_realloc(mc_Buffer_t* self, uint64_t size) {
    if (!self) return NULL;
    MC_LOG_DEBUG(self->dev, "reallocating buffer to size %ld", size);

    mc_Buffer_t* new = mc_buffer_create(self->dev, size);
    if (!new) return NULL;

    uint64_t minSize = size < self->size ? size : self->size;
    mc_buffer_write(new, 0, minSize, self->map);
    mc_buffer_destroy(self);
    return new;
}

uint64_t mc_buffer_write(
    mc_Buffer_t* self,
    uint64_t offset,
    uint64_t size,
    void* data
) {
    if (!self) return 0;
    MC_LOG_DEBUG(self->dev, "writing %ld bytes to buffer", size);

    if (offset + size > self->size) {
        MC_LOG_ERROR(
            self->dev,
            "offset + size > buffer size (%ld > %ld)",
            offset + size,
            self->size
        );
        return 0;
    }

    // since the buffer has been mapped, this is all that's needed
    memcpy((char*)self->map + offset, data, size);
    return size;
}

uint64_t mc_buffer_read(
    mc_Buffer_t* self,
    uint64_t offset,
    uint64_t size,
    void* data
) {
    if (!self) return 0;
    MC_LOG_DEBUG(self->dev, "reading %ld bytes from buffer", size);

    if (offset + size > self->size) {
        MC_LOG_ERROR(
            self->dev,
            "offset + size > buffer size (%ld > %ld)",
            offset + size,
            self->size
        );
        return 0;
    }

    // since the buffer has been mapped, this is all thats needed
    memcpy(data, (char*)self->map + offset, size);
    return size;
}