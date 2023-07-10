#ifndef MICROCOMPUTE_H_INCLUDE_GUARD
#define MICROCOMPUTE_H_INCLUDE_GUARD

#define MICROCOMPUTE_IMPLEMENTATION // TODO: remove

#include <stdint.h>

typedef enum mc_DebugLevel {
    MC_DEBUG_LEVEL_INFO,
    MC_DEBUG_LEVEL_LOW,
    MC_DEBUG_LEVEL_MEDIUM,
    MC_DEBUG_LEVEL_HIGH,
    MC_DEBUG_LEVEL_UNKNOWN,
} mc_DebugLevel;

typedef void(mc_debug_cb)(
    mc_DebugLevel level,
    const char* source,
    const char* msg,
    void* arg
);

typedef struct mc_State mc_State_t;

typedef struct mc_Program mc_Program_t;

const char* mc_debug_level_to_str(mc_DebugLevel level);

mc_State_t* mc_state_create(mc_debug_cb* debug_cb, void* debugArg);

void mc_state_destroy(mc_State_t* self);

mc_Program_t* mc_program_create(
    mc_State_t* state,
    uint64_t shaderLength,
    uint32_t* shader,
    uint32_t bufferCount,
    uint64_t* bufferSizes
);

void mc_program_destroy(mc_Program_t* self);

uint32_t mc_program_get_buffer_count(mc_Program_t* self);

uint64_t mc_program_nth_buffer_get_size(mc_Program_t* self, uint32_t n);

uint64_t mc_program_nth_buffer_write(
    mc_Program_t* self,
    uint32_t n,
    uint64_t offset,
    uint64_t size,
    void* data
);

uint64_t mc_program_nth_buffer_read(
    mc_Program_t* self,
    uint32_t n,
    uint64_t offset,
    uint64_t size,
    void* data
);

void mc_program_dispatch(
    mc_Program_t* self,
    uint32_t x,
    uint32_t y,
    uint32_t z
);

/** text
 * ## Licence
 *
 * ```
 * MIT License
 * Copyright (c) 2023 Kai Kitagawa-Jones
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * ```
 */

#ifdef MICROCOMPUTE_IMPLEMENTATION

#include <stdbool.h>
#include <stdlib.h> // TODO: custom allocators
#include <string.h> // TODO: custom allocators
#include <vulkan/vulkan.h>

#define ENABLE_VALIDATION_LAYERS true // TODO: undef

struct mc_State {
    bool isInitialized;

    void* debugArg;
    mc_debug_cb* debug_cb;

    VkInstance instance;
    VkDebugUtilsMessengerEXT messenger;
    VkPhysicalDevice physicalDevice;
    uint32_t queueFamilyIndex;
    VkDevice device;

    PFN_vkCreateDebugUtilsMessengerEXT create_messenger;
    PFN_vkDestroyDebugUtilsMessengerEXT destroy_messenger;
};

struct mc_Buffer {
    bool isInitialized;
    mc_State_t* state;

    uint64_t size;
    void* mappedMemory;

    VkBuffer buffer;
    VkDeviceMemory memory;
};

struct mc_Program {
    bool isInitialized;
    mc_State_t* state;

    uint32_t bufferCount;
    struct mc_Buffer* buffers;

    VkShaderModule shaderModule;
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;
    VkDescriptorPool descriptorPool;
    VkCommandPool commandPool;
    VkDescriptorSet descriptorSet;
    VkCommandBuffer commandBuffer;
};

static void mc_state_emit_debug_msg(
    mc_State_t* self,
    mc_DebugLevel level,
    const char* source,
    const char* message
) {
    if (!self->debug_cb) return;
    self->debug_cb(level, source, message, self->debugArg);
}

static VkBool32 mc_vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    __attribute__((unused)) VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT* message,
    void* userData
) {
    switch (severity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            mc_state_emit_debug_msg(
                (mc_State_t*)userData,
                MC_DEBUG_LEVEL_MEDIUM,
                "Vulkan",
                message->pMessage
            );
            return VK_FALSE;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            mc_state_emit_debug_msg(
                (mc_State_t*)userData,
                MC_DEBUG_LEVEL_HIGH,
                "Vulkan",
                message->pMessage
            );
            return VK_FALSE;
        default: return VK_FALSE;
    }
}

static uint32_t mc_choose_physical_device_index(
    uint32_t physicalDeviceCount,
    VkPhysicalDevice* physicalDevices
) {
    uint32_t idx = physicalDeviceCount;

    // find discrete gpu
    for (uint32_t i = 0; i < physicalDeviceCount; i++) {
        VkPhysicalDeviceProperties deviceProps;
        vkGetPhysicalDeviceProperties(physicalDevices[i], &deviceProps);
        if (deviceProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            idx = i;
    }

    if (idx != physicalDeviceCount) return idx;

    // find integrated gpu
    for (uint32_t i = 0; i < physicalDeviceCount; i++) {
        VkPhysicalDeviceProperties deviceProps;
        vkGetPhysicalDeviceProperties(physicalDevices[i], &deviceProps);
        if (deviceProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
            idx = i;
    }

    return idx;
}

static uint32_t mc_choose_queue_family_index(
    uint32_t queueFamilyPropertiesCount,
    VkQueueFamilyProperties* queueFamilyProperties
) {
    uint32_t idx = queueFamilyPropertiesCount;

    // find dedicated compute queue
    for (uint32_t i = 0; i < queueFamilyPropertiesCount; i++) {
        bool g = VK_QUEUE_GRAPHICS_BIT & queueFamilyProperties[i].queueFlags;
        bool c = VK_QUEUE_COMPUTE_BIT & queueFamilyProperties[i].queueFlags;
        if (!g && c) idx = i;
    }

    if (idx != queueFamilyPropertiesCount) return idx;

    // find compute capable queue
    for (uint32_t i = 0; i < queueFamilyPropertiesCount; i++) {
        VkQueueFlags c
            = VK_QUEUE_COMPUTE_BIT & queueFamilyProperties[i].queueFlags;
        if (c) idx = i;
    }

    return idx;
}

static uint32_t mc_choose_memory_type(
    VkPhysicalDeviceMemoryProperties memoryProps,
    uint64_t size
) {
    for (uint32_t i = 0; i < memoryProps.memoryTypeCount; i++) {
        VkMemoryType type = memoryProps.memoryTypes[i];
        VkMemoryHeap heap = memoryProps.memoryHeaps[type.heapIndex];

        bool v = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT & type.propertyFlags;
        bool c = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT & type.propertyFlags;
        if (v && c && size < heap.size) return i;
    }

    return memoryProps.memoryTypeCount;
}

static void mc_buffer_init(
    struct mc_Buffer* self,
    mc_State_t* state,
    uint64_t size
) {
    VkResult res;
    *self = (struct mc_Buffer){
        .isInitialized = false,
        .size = size,
        .mappedMemory = NULL,
        .state = state,
        .buffer = NULL,
        .memory = NULL,
    };

    VkBufferCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .size = self->size,
        .usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 1,
        .pQueueFamilyIndices = &self->state->queueFamilyIndex,
    };

    res = vkCreateBuffer(self->state->device, &createInfo, NULL, &self->buffer);
    if (res != VK_SUCCESS) {
        mc_state_emit_debug_msg(
            self->state,
            MC_DEBUG_LEVEL_HIGH,
            "mc_Buffer",
            "vkCreateBuffer() failed"
        );
        return;
    }

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(
        self->state->device,
        self->buffer,
        &memoryRequirements
    );

    if (memoryRequirements.size > self->size)
        self->size = memoryRequirements.size;

    VkPhysicalDeviceMemoryProperties memoryProps;
    vkGetPhysicalDeviceMemoryProperties(
        self->state->physicalDevice,
        &memoryProps
    );

    uint32_t memoryTypeIdx = mc_choose_memory_type(memoryProps, self->size);
    if (memoryTypeIdx == memoryProps.memoryTypeCount) {
        mc_state_emit_debug_msg(
            self->state,
            MC_DEBUG_LEVEL_HIGH,
            "mc_Buffer",
            "no suitable memory type found"
        );
        return;
    }

    VkMemoryAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = NULL,
        .allocationSize = self->size,
        .memoryTypeIndex = memoryTypeIdx,
    };

    res = vkAllocateMemory(
        self->state->device,
        &allocInfo,
        NULL,
        &self->memory
    );

    if (res != VK_SUCCESS) {
        mc_state_emit_debug_msg(
            self->state,
            MC_DEBUG_LEVEL_HIGH,
            "mc_Buffer",
            "vkAllocateMemory() failed"
        );
        return;
    }

    vkMapMemory(
        self->state->device,
        self->memory,
        0,
        self->size,
        0,
        &self->mappedMemory
    );

    res = vkBindBufferMemory(
        self->state->device,
        self->buffer,
        self->memory,
        0
    );

    mc_state_emit_debug_msg(
        self->state,
        MC_DEBUG_LEVEL_INFO,
        "mc_Buffer",
        "mc_Buffer successfully initialized"
    );

    self->isInitialized = true;
}

static void mc_buffer_finalize(struct mc_Buffer self) {
    mc_state_emit_debug_msg(
        self.state,
        MC_DEBUG_LEVEL_INFO,
        "mc_Buffer",
        "destroying mc_Buffer"
    );

    if (self.buffer) {
        vkDestroyBuffer(self.state->device, self.buffer, NULL);
        self.buffer = NULL;
    }

    if (self.memory) {
        vkFreeMemory(self.state->device, self.memory, NULL);
        self.memory = NULL;
    }
}

static uint64_t mc_buffer_write(
    struct mc_Buffer self,
    uint64_t offset,
    uint64_t size,
    void* data
) {
    if (!self.isInitialized) {
        mc_state_emit_debug_msg(
            self.state,
            MC_DEBUG_LEVEL_MEDIUM,
            "mc_Buffer",
            "mc_Buffer not initialized"
        );
        return 0;
    }

    if (!self.isInitialized) {
        mc_state_emit_debug_msg(
            self.state,
            MC_DEBUG_LEVEL_MEDIUM,
            "mc_Buffer",
            "offset + size > mc_Buffer.size"
        );
        return 0;
    }

    memcpy((char*)self.mappedMemory + offset, data, size);
    return size;
}

static uint64_t mc_buffer_read(
    struct mc_Buffer self,
    uint64_t offset,
    uint64_t size,
    void* data
) {
    if (!self.isInitialized) {
        mc_state_emit_debug_msg(
            self.state,
            MC_DEBUG_LEVEL_MEDIUM,
            "mc_Buffer",
            "mc_Buffer not initialized"
        );
        return 0;
    }

    if (!self.isInitialized) {
        mc_state_emit_debug_msg(
            self.state,
            MC_DEBUG_LEVEL_MEDIUM,
            "mc_Buffer",
            "offset + size > mc_Buffer.size"
        );
        return 0;
    }

    memcpy(data, (char*)self.mappedMemory + offset, size);
    return size;
}

const char* mc_debug_level_to_str(mc_DebugLevel level) {
    switch (level) {
        case MC_DEBUG_LEVEL_INFO: return "MC_DEBUG_LEVEL_INFO";
        case MC_DEBUG_LEVEL_LOW: return "MC_DEBUG_LEVEL_LOW";
        case MC_DEBUG_LEVEL_MEDIUM: return "MC_DEBUG_LEVEL_MEDIUM";
        case MC_DEBUG_LEVEL_HIGH: return "MC_DEBUG_LEVEL_HIGH";
        default: return "MC_DEBUG_LEVEL_UNKNOWN";
    }
}

mc_State_t* mc_state_create(mc_debug_cb* debug_cb, void* debugArg) {
    VkResult res;
    mc_State_t* self = malloc(sizeof *self);

    *self = (mc_State_t){
        .isInitialized = false,
        .debug_cb = debug_cb,
        .debugArg = debugArg,
        .instance = NULL,
        .messenger = NULL,
        .physicalDevice = NULL,
        .queueFamilyIndex = 0,
        .device = NULL,
    };

    VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                         | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
                         | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                         | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                     | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                     | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = mc_vk_debug_callback,
        .pUserData = self,
    };

    VkApplicationInfo applicationInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = NULL,
        .pApplicationName = "microcompute",
        .applicationVersion = 0,
        .pEngineName = NULL,
        .engineVersion = 0,
        .apiVersion = VK_MAKE_VERSION(1, 0, 0),
    };

    VkInstanceCreateInfo instanceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = &messengerCreateInfo,
        .flags = 0,
        .pApplicationInfo = &applicationInfo,
        .enabledLayerCount = ENABLE_VALIDATION_LAYERS ? 1 : 0,
        .ppEnabledLayerNames = (const char*[]){"VK_LAYER_KHRONOS_validation"},
        .enabledExtensionCount = ENABLE_VALIDATION_LAYERS ? 1 : 0,
        .ppEnabledExtensionNames = (const char*[]){"VK_EXT_debug_utils"},
    };

    res = vkCreateInstance(&instanceCreateInfo, NULL, &self->instance);
    if (res != VK_SUCCESS) {
        mc_state_emit_debug_msg(
            self,
            MC_DEBUG_LEVEL_HIGH,
            "mc_State_t",
            "vkCreateInstance() failed"
        );
        return self;
    }

    self->create_messenger = //
        (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            self->instance,
            "vkCreateDebugUtilsMessengerEXT"
        );

    self->destroy_messenger = //
        (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            self->instance,
            "vkDestroyDebugUtilsMessengerEXT"
        );

    if (self->create_messenger) {
        self->create_messenger(
            self->instance,
            &messengerCreateInfo,
            NULL,
            &self->messenger
        );
    }

    uint32_t physicalDeviceCount = 0;
    res = vkEnumeratePhysicalDevices(
        self->instance,
        &physicalDeviceCount,
        NULL
    );

    if (res != VK_SUCCESS) {
        mc_state_emit_debug_msg(
            self,
            MC_DEBUG_LEVEL_HIGH,
            "mc_State_t",
            "vkEnumeratePhysicalDevices() failed"
        );
        return self;
    }

    VkPhysicalDevice* physicalDevices
        = malloc(sizeof *physicalDevices * physicalDeviceCount);

    res = vkEnumeratePhysicalDevices(
        self->instance,
        &physicalDeviceCount,
        physicalDevices
    );

    if (res != VK_SUCCESS) {
        mc_state_emit_debug_msg(
            self,
            MC_DEBUG_LEVEL_HIGH,
            "mc_State_t",
            "vkEnumeratePhysicalDevices() failed"
        );
        return self;
    }

    uint32_t physicalDeviceIndex
        = mc_choose_physical_device_index(physicalDeviceCount, physicalDevices);

    if (physicalDeviceIndex == physicalDeviceCount) {
        mc_state_emit_debug_msg(
            self,
            MC_DEBUG_LEVEL_HIGH,
            "mc_State_t",
            "no suitable devices found"
        );
        return self;
    }

    self->physicalDevice = physicalDevices[physicalDeviceIndex];
    free(physicalDevices);

    uint32_t queueFamilyPropertiesCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(
        self->physicalDevice,
        &queueFamilyPropertiesCount,
        NULL
    );

    VkQueueFamilyProperties* queueFamilyProperties
        = malloc(sizeof *queueFamilyProperties * queueFamilyPropertiesCount);

    vkGetPhysicalDeviceQueueFamilyProperties(
        self->physicalDevice,
        &queueFamilyPropertiesCount,
        queueFamilyProperties
    );

    self->queueFamilyIndex = mc_choose_queue_family_index(
        queueFamilyPropertiesCount,
        queueFamilyProperties
    );

    free(queueFamilyProperties);

    if (self->queueFamilyIndex == queueFamilyPropertiesCount) {
        mc_state_emit_debug_msg(
            self,
            MC_DEBUG_LEVEL_HIGH,
            "mc_State_t",
            "no suitable queue found"
        );
        return self;
    }

    float queuePriority = 1.0;
    VkDeviceQueueCreateInfo deviceQueueCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .queueFamilyIndex = self->queueFamilyIndex,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority,
    };

    VkDeviceCreateInfo deviceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &deviceQueueCreateInfo,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = NULL,
        .enabledExtensionCount = 0,
        .ppEnabledExtensionNames = NULL,
        .pEnabledFeatures = NULL,
    };

    res = vkCreateDevice(
        self->physicalDevice,
        &deviceCreateInfo,
        0,
        &self->device
    );

    if (res != VK_SUCCESS) {
        mc_state_emit_debug_msg(
            self,
            MC_DEBUG_LEVEL_HIGH,
            "mc_State_t",
            "vkCreateDevice() failed"
        );
        return self;
    }

    mc_state_emit_debug_msg(
        self,
        MC_DEBUG_LEVEL_INFO,
        "mc_State_t",
        "mc_State_t successfully created"
    );

    self->isInitialized = true;
    return self;
}

void mc_state_destroy(mc_State_t* self) {
    mc_state_emit_debug_msg(
        self,
        MC_DEBUG_LEVEL_INFO,
        "mc_State_t",
        "destroying mc_State_t"
    );

    if (self->device) {
        vkDestroyDevice(self->device, NULL);
        self->device = NULL;
    }

    if (self->messenger && self->destroy_messenger) {
        self->destroy_messenger(self->instance, self->messenger, NULL);
        self->messenger = NULL;
    }

    if (self->instance) {
        vkDestroyInstance(self->instance, NULL);
        self->instance = NULL;
    }

    free(self);
}

mc_Program_t* mc_program_create(
    mc_State_t* state,
    uint64_t shaderLength,
    uint32_t* shader,
    uint32_t bufferCount,
    uint64_t* bufferSizes
) {
    VkResult res;
    mc_Program_t* self = malloc(sizeof *self);
    *self = (mc_Program_t){
        .isInitialized = false,
        .state = state,
        .bufferCount = bufferCount,
        .buffers = NULL,
        .shaderModule = NULL,
        .descriptorSetLayout = NULL,
        .pipelineLayout = NULL,
        .pipeline = NULL,
        .descriptorPool = NULL,
        .commandPool = NULL,
    };

    self->buffers = malloc(sizeof *self->buffers * bufferCount);

    for (uint32_t i = 0; i < self->bufferCount; i++) {
        mc_buffer_init(&self->buffers[i], self->state, bufferSizes[i]);
        if (!self->buffers[i].isInitialized) {
            mc_state_emit_debug_msg(
                self->state,
                MC_DEBUG_LEVEL_HIGH,
                "mc_Program",
                "failed to initialize buffer"
            );
            return self;
        }
    }

    VkShaderModuleCreateInfo shaderModuleCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .codeSize = shaderLength,
        .pCode = shader,
    };

    res = vkCreateShaderModule(
        self->state->device,
        &shaderModuleCreateInfo,
        NULL,
        &self->shaderModule
    );

    if (res != VK_SUCCESS) {
        mc_state_emit_debug_msg(
            self->state,
            MC_DEBUG_LEVEL_HIGH,
            "mc_Program_t",
            "vkCreateShaderModule() failed"
        );
        return self;
    }

    VkDescriptorSetLayoutBinding* descriptorSetLayoutBindings
        = malloc(sizeof *descriptorSetLayoutBindings * bufferCount);

    for (uint32_t i = 0; i < bufferCount; i++) {
        descriptorSetLayoutBindings[i] = (VkDescriptorSetLayoutBinding){
            .binding = i,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
            .pImmutableSamplers = NULL,
        };
    }

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .bindingCount = bufferCount,
        .pBindings = descriptorSetLayoutBindings,
    };

    res = vkCreateDescriptorSetLayout(
        self->state->device,
        &descriptorSetLayoutCreateInfo,
        NULL,
        &self->descriptorSetLayout
    );

    if (res != VK_SUCCESS) {
        mc_state_emit_debug_msg(
            self->state,
            MC_DEBUG_LEVEL_HIGH,
            "mc_Program_t",
            "vkCreateDescriptorSetLayout() failed"
        );
        return self;
    }

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .setLayoutCount = 1,
        .pSetLayouts = &self->descriptorSetLayout,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = NULL,
    };

    res = vkCreatePipelineLayout(
        self->state->device,
        &pipelineLayoutCreateInfo,
        NULL,
        &self->pipelineLayout
    );

    if (res != VK_SUCCESS) {
        mc_state_emit_debug_msg(
            self->state,
            MC_DEBUG_LEVEL_HIGH,
            "mc_Program_t",
            "vkCreatePipelineLayout() failed"
        );
        return self;
    }

    VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .stage = VK_SHADER_STAGE_COMPUTE_BIT,
        .module = self->shaderModule,
        .pName = "main",
        .pSpecializationInfo = NULL,
    };

    VkComputePipelineCreateInfo computePipelineCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .stage = pipelineShaderStageCreateInfo,
        .layout = self->pipelineLayout,
        .basePipelineHandle = 0,
        .basePipelineIndex = 0,
    };

    res = vkCreateComputePipelines(
        self->state->device,
        0,
        1,
        &computePipelineCreateInfo,
        NULL,
        &self->pipeline
    );

    if (res != VK_SUCCESS) {
        mc_state_emit_debug_msg(
            self->state,
            MC_DEBUG_LEVEL_HIGH,
            "mc_Program_t",
            "vkCreateComputePipelines() failed"
        );
        return self;
    }

    VkDescriptorPoolSize descriptorPoolSize = {
        .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        .descriptorCount = bufferCount,
    };

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .maxSets = 1,
        .poolSizeCount = 1,
        .pPoolSizes = &descriptorPoolSize,
    };

    res = vkCreateDescriptorPool(
        self->state->device,
        &descriptorPoolCreateInfo,
        NULL,
        &self->descriptorPool
    );

    if (res != VK_SUCCESS) {
        mc_state_emit_debug_msg(
            self->state,
            MC_DEBUG_LEVEL_HIGH,
            "mc_Program_t",
            "vkCreateDescriptorPool() failed"
        );
        return self;
    }

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = NULL,
        .descriptorPool = self->descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &self->descriptorSetLayout,
    };

    res = vkAllocateDescriptorSets(
        self->state->device,
        &descriptorSetAllocateInfo,
        &self->descriptorSet
    );

    if (res != VK_SUCCESS) {
        mc_state_emit_debug_msg(
            self->state,
            MC_DEBUG_LEVEL_HIGH,
            "mc_Program_t",
            "vkAllocateDescriptorSets() failed"
        );
        return self;
    }

    VkDescriptorBufferInfo* descriptorBufferInfo
        = malloc(sizeof *descriptorBufferInfo * bufferCount);

    VkWriteDescriptorSet* writeDescriptorSet
        = malloc(sizeof *writeDescriptorSet * bufferCount);

    for (uint32_t i = 0; i < bufferCount; i++) {
        descriptorBufferInfo[i] = (VkDescriptorBufferInfo){
            .buffer = self->buffers[i].buffer,
            .offset = 0,
            .range = VK_WHOLE_SIZE,
        };

        writeDescriptorSet[i] = (VkWriteDescriptorSet){
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = 0,
            .dstSet = self->descriptorSet,
            .dstBinding = i,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .pImageInfo = NULL,
            .pBufferInfo = &descriptorBufferInfo[i],
            .pTexelBufferView = NULL,
        };
    }

    vkUpdateDescriptorSets(
        self->state->device,
        bufferCount,
        writeDescriptorSet,
        0,
        NULL
    );

    VkCommandPoolCreateInfo commandPoolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = self->state->queueFamilyIndex,
    };

    res = vkCreateCommandPool(
        self->state->device,
        &commandPoolCreateInfo,
        NULL,
        &self->commandPool
    );

    if (res != VK_SUCCESS) {
        mc_state_emit_debug_msg(
            self->state,
            MC_DEBUG_LEVEL_HIGH,
            "mc_Program_t",
            "vkCreateCommandPool() failed"
        );
        return self;
    }

    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = NULL,
        .commandPool = self->commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    res = vkAllocateCommandBuffers(
        self->state->device,
        &commandBufferAllocateInfo,
        &self->commandBuffer
    );

    if (res != VK_SUCCESS) {
        mc_state_emit_debug_msg(
            self->state,
            MC_DEBUG_LEVEL_HIGH,
            "mc_Program_t",
            "vkAllocateCommandBuffers() failed"
        );
        return self;
    }

    VkCommandBufferBeginInfo commandBufferBeginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = NULL,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = NULL,
    };

    res = vkBeginCommandBuffer(self->commandBuffer, &commandBufferBeginInfo);
    if (res != VK_SUCCESS) {
        mc_state_emit_debug_msg(
            self->state,
            MC_DEBUG_LEVEL_HIGH,
            "mc_Program_t",
            "vkBeginCommandBuffer() failed"
        );
        return self;
    }

    vkCmdBindPipeline(
        self->commandBuffer,
        VK_PIPELINE_BIND_POINT_COMPUTE,
        self->pipeline
    );

    vkCmdBindDescriptorSets(
        self->commandBuffer,
        VK_PIPELINE_BIND_POINT_COMPUTE,
        self->pipelineLayout,
        0,
        1,
        &self->descriptorSet,
        0,
        NULL
    );

    mc_state_emit_debug_msg(
        self->state,
        MC_DEBUG_LEVEL_INFO,
        "mc_Program_t",
        "mc_Program_t successfully created"
    );

    self->isInitialized = true;
    return self;
}

void mc_program_destroy(mc_Program_t* self) {
    mc_state_emit_debug_msg(
        self->state,
        MC_DEBUG_LEVEL_INFO,
        "mc_Program_t",
        "destroying mc_Program_t"
    );

    if (self->buffers) {
        for (uint32_t i = 0; i < self->bufferCount; i++)
            mc_buffer_finalize(self->buffers[i]);
        free(self->buffers);

        self->bufferCount = 0;
        self->buffers = NULL;
    }

    if (self->shaderModule) {
        vkDestroyShaderModule(self->state->device, self->shaderModule, NULL);
        self->shaderModule = NULL;
    }

    if (self->descriptorSetLayout) {
        vkDestroyDescriptorSetLayout(
            self->state->device,
            self->descriptorSetLayout,
            NULL
        );
        self->descriptorSetLayout = NULL;
    }

    if (self->pipelineLayout) {
        vkDestroyPipelineLayout(
            self->state->device,
            self->pipelineLayout,
            NULL
        );
        self->pipelineLayout = NULL;
    }

    if (self->pipeline) {
        vkDestroyPipeline(self->state->device, self->pipeline, NULL);
        self->pipeline = NULL;
    }

    if (self->descriptorPool) {
        vkDestroyDescriptorPool(
            self->state->device,
            self->descriptorPool,
            NULL
        );
        self->descriptorPool = NULL;
    }

    if (self->commandPool) {
        vkDestroyCommandPool(self->state->device, self->commandPool, NULL);
        self->commandPool = NULL;
    }

    free(self);
}

uint32_t mc_program_get_buffer_count(mc_Program_t* self) {
    return self->bufferCount;
}

uint64_t mc_program_nth_buffer_get_size(mc_Program_t* self, uint32_t n) {
    if (n >= self->bufferCount) {
        mc_state_emit_debug_msg(
            self->state,
            MC_DEBUG_LEVEL_MEDIUM,
            "mc_Program_t",
            "n >= mc_Program_t.bufferCount"
        );
        return 0;
    }
    return self->buffers[n].size;
}

uint64_t mc_program_nth_buffer_write(
    mc_Program_t* self,
    uint32_t n,
    uint64_t offset,
    uint64_t size,
    void* data
) {
    if (n >= self->bufferCount) {
        mc_state_emit_debug_msg(
            self->state,
            MC_DEBUG_LEVEL_MEDIUM,
            "mc_Program_t",
            "n >= mc_Program_t.bufferCount"
        );
        return 0;
    }
    return mc_buffer_write(self->buffers[n], offset, size, data);
}

uint64_t mc_program_nth_buffer_read(
    mc_Program_t* self,
    uint32_t n,
    uint64_t offset,
    uint64_t size,
    void* data
) {
    if (n >= self->bufferCount) {
        mc_state_emit_debug_msg(
            self->state,
            MC_DEBUG_LEVEL_MEDIUM,
            "mc_Program_t",
            "n >= mc_Program_t.bufferCount"
        );
        return 0;
    }
    return mc_buffer_read(self->buffers[n], offset, size, data);
}

void mc_program_dispatch(
    mc_Program_t* self,
    uint32_t x,
    uint32_t y,
    uint32_t z
) {
    VkResult res;
    if (x * y * z == 0) {
        mc_state_emit_debug_msg(
            self->state,
            MC_DEBUG_LEVEL_MEDIUM,
            "mc_Program_t",
            "at least one dimension is 0"
        );
        return;
    }

    VkCommandBufferBeginInfo commandBufferBeginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = NULL,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = NULL,
    };

    res = vkBeginCommandBuffer(self->commandBuffer, &commandBufferBeginInfo);
    if (res != VK_SUCCESS) {
        mc_state_emit_debug_msg(
            self->state,
            MC_DEBUG_LEVEL_HIGH,
            "mc_Program_t",
            "vkBeginCommandBuffer() failed"
        );
        return;
    }

    vkCmdBindPipeline(
        self->commandBuffer,
        VK_PIPELINE_BIND_POINT_COMPUTE,
        self->pipeline
    );

    vkCmdBindDescriptorSets(
        self->commandBuffer,
        VK_PIPELINE_BIND_POINT_COMPUTE,
        self->pipelineLayout,
        0,
        1,
        &self->descriptorSet,
        0,
        NULL
    );

    vkCmdDispatch(self->commandBuffer, x, y, z);
    res = vkEndCommandBuffer(self->commandBuffer);
    if (res != VK_SUCCESS) {
        mc_state_emit_debug_msg(
            self->state,
            MC_DEBUG_LEVEL_HIGH,
            "mc_Program_t",
            "vkEndCommandBuffer() failed"
        );
        return;
    }

    VkQueue queue;
    vkGetDeviceQueue(
        self->state->device,
        self->state->queueFamilyIndex,
        0,
        &queue
    );

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = NULL,
        .waitSemaphoreCount = 0,
        .pWaitSemaphores = NULL,
        .pWaitDstStageMask = NULL,
        .commandBufferCount = 1,
        .pCommandBuffers = &self->commandBuffer,
        .signalSemaphoreCount = 0,
        .pSignalSemaphores = NULL,
    };

    res = vkQueueSubmit(queue, 1, &submitInfo, 0);
    if (res != VK_SUCCESS) {
        mc_state_emit_debug_msg(
            self->state,
            MC_DEBUG_LEVEL_HIGH,
            "mc_Program_t",
            "vkQueueSubmit() failed"
        );
        return;
    }

    res = vkQueueWaitIdle(queue);
    if (res != VK_SUCCESS) {
        mc_state_emit_debug_msg(
            self->state,
            MC_DEBUG_LEVEL_HIGH,
            "mc_Program_t",
            "vkQueueWaitIdle() failed"
        );
        return;
    }
}

#endif // MICROCOMPUTE_IMPLEMENTATION
#endif // MICROCOMPUTE_H_INCLUDE_GUARD