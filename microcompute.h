#ifndef MICROCOMPUTE_H_INCLUDE_GUARD
#define MICROCOMPUTE_H_INCLUDE_GUARD

/** text
 * # `microcompute.h`
 *
 * This library contains utilities that can be used to easily run SPIR-V compute
 * shaders using vulkan.
 *
 * ## Usage
 *
 * Define `MICROCOMPUTE_IMPLEMENTATION` before including this file in one of
 * your C files to create the implementation. It should look something like:
 *
 * ```c
 * #include ...
 * #include ...
 *
 * #define MICROCOMPUTE_IMPLEMENTATION
 * #include "microcompute.h"
 * ```
 *
 * In other C files, just include this file as normal.
 *
 * See https://github.com/kal39/microcompute for more info.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/** text
 * ## Types
 */

/** code
 * The severity of a log message.
 */
typedef enum mc_LogLevel {
    MC_LOG_LEVEL_DEBUG,
    MC_LOG_LEVEL_INFO,
    MC_LOG_LEVEL_WARN,
    MC_LOG_LEVEL_ERROR,
    MC_LOG_LEVEL_UNKNOWN,
} mc_LogLevel_t;

/** code
 * The type of a mc_Device_t.
 */
typedef enum mc_DeviceType {
    MC_DEVICE_TYPE_DGPU, // discrete GPU
    MC_DEVICE_TYPE_IGPU, // integrated GPU
    MC_DEVICE_TYPE_VGPU, // virtual GPU
    MC_DEVICE_TYPE_CPU,
    MC_DEVICE_TYPE_OTHER,
} mc_DeviceType_t;

/** code
 * The log callback type passed to `mc_set_log_cb()`.
 *
 * - `lvl`: A `mc_LogLevel` indicating the severity of the message
 * - `src`: The message source (NULL terminated)
 * - `arg`: The value passed to `logArg` in `mc_instance_create()`
 * - `file`: The file the message originated from (NULL terminated)
 * - `line`: The line the message originated from
 * - `fmt`: The message formatting (same format as `printf()`)
 * - `...`: The arguments for the formatting
 */
typedef void(mc_log_cb)( //
    mc_LogLevel_t lvl,
    const char* src,
    void* arg,
    const char* file,
    int line,
    const char* fmt,
    ...
);

/** code
 * Core microcompute types.
 */
typedef struct mc_Instance mc_Instance_t;
typedef struct mc_Device mc_Device_t;
typedef struct mc_Buffer mc_Buffer_t;
typedef struct mc_Program mc_Program_t;

/** text
 * ## Functions
 */

/** code
 * Create a `mc_Instance_t` object.
 *
 * - `log_cb`: A function to call when a error occurs, set to `NULL` to ignore
 * - `logArg`: A value to pass to the log callback, set to `NULL` to ignore
 * - returns: A instance object on success, `NULL` on error
 */
mc_Instance_t* mc_instance_create(mc_log_cb* log_cb, void* logArg);

/** code
 * Destroy a `mc_Instance_t` object.
 *
 * - `self`: A reference to a `mc_Instance_t` object
 */
void mc_instance_destroy(mc_Instance_t* self);

/** code
 * Get the number of available `mc_Device_t` objects.
 *
 * - `self`: A reference to a `mc_Instance_t` object
 * - returns: The number of devices
 */
uint32_t mc_instance_get_device_count(mc_Instance_t* self);

/** code
 * Get a list of of available `mc_Device_t` objects.
 *
 * - `self`: A reference to a `mc_Instance_t` object
 * - returns: A list of devices
 */
mc_Device_t** mc_instance_get_devices(mc_Instance_t* self);

/** code
 * Get the type of the device (discrete gpu etc).
 *
 * - `self`: A reference to a `mc_Device_t` object
 * - returns: The type of the GPU
 */
mc_DeviceType_t mc_device_get_type(mc_Device_t* self);

/** code
 * Get the total video memory of a device.
 *
 * - `self`: A reference to a `mc_Device_t` object
 * - returns: The size of the video memory in bytes
 */
uint64_t mc_device_get_total_memory_size(mc_Device_t* self);

/** code
 * Get the used video memory of a device.
 *
 * - `self`: A reference to a `mc_Device_t` object
 * - returns: The size of the used video memory in bytes
 */
uint64_t mc_device_get_used_memory_size(mc_Device_t* self);

/** code
 * Get the max workgroup size (the max for x*y*z) of a device.
 *
 * - `self`: A reference to a `mc_Device_t` object
 * - returns: The max workgroup size
 */
uint32_t mc_device_get_max_workgroup_size_total(mc_Device_t* self);

/** code
 * Get the max workgroup size (for each x, y, z) of a device.
 *
 * - `self`: A reference to a `mc_Device_t` object
 * - returns: The max workgroup size, as a 3 element array (dont free)
 */
uint32_t* mc_device_get_max_workgroup_size_shape(mc_Device_t* self);

/** code
 * Get the max workgroup count (for each x, y, z) of a device.
 *
 * - `self`: A reference to a `mc_Device_t` object
 * - returns: The max workgroup count, as a 3 element array
 */
uint32_t* mc_device_get_max_workgroup_count(mc_Device_t* self);

/** code
 * Get the name of a device.
 *
 * - `self`: A reference to a `mc_Device_t` object
 * - returns: A `NULL` terminated string containing the name of the device
 */
char* mc_device_get_name(mc_Device_t* self);

/** code
 * Create a `mc_Buffer_t` object.
 *
 * - `device`: A reference to a `mc_Device_t` object
 * - `size`: The size of the buffer
 * - returns: A reference to a `mc_Buffer_t` object
 */
mc_Buffer_t* mc_buffer_create(mc_Device_t* device, uint64_t size);

/** code
 * Create a `mc_Buffer_t` object and initializes with some data.
 *
 * - `device`: A reference to a `mc_Device_t` object
 * - `size`: The size of the buffer
 * - `data`: A reference to the data the initialize the buffer with
 * - returns: A buffer object on success, `NULL` on error
 */
mc_Buffer_t* mc_buffer_create_from(
    mc_Device_t* device,
    uint64_t size,
    void* data
);

/** code
 * Destroy a `mc_Buffer_t` object.
 *
 * - `self`: A reference to a `mc_Buffer_t` object
 */
void mc_buffer_destroy(mc_Buffer_t* self);

/** code
 * Get the size of a `mc_Buffer_t` object.
 *
 * - `self`: A reference to a `mc_Buffer_t` object
 * - returns: The size, in bytes
 */
uint64_t mc_buffer_get_size(mc_Buffer_t* self);

/** code
 * Write data to a `mc_Buffer_t` object.
 *
 * - `self`: A reference to a `mc_Buffer_t` object
 * - `offset`: The offset from witch to start writing the data, in bytes
 * - `size`: The size of the data to write, in bytes
 * - `data`: The data to write
 * - returns: The number of bytes written
 */
uint64_t mc_buffer_write(
    mc_Buffer_t* self,
    uint64_t offset,
    uint64_t size,
    void* data
);

/** code
 * Read data from a `mc_Buffer_t` object.
 *
 * - `self`: A reference to a `mc_Buffer_t` object
 * - `offset`: The offset from witch to start reading the data, in bytes
 * - `size`: The size of the data to read, in bytes
 * - `data`: Where to read the data to
 * - returns: The number of bytes read
 */
uint64_t mc_buffer_read(
    mc_Buffer_t* self,
    uint64_t offset,
    uint64_t size,
    void* data
);

/** code
 * Create a `mc_Program_t` object.
 *
 * - `device`: A reference to a `mc_Device_t` object
 * - `fileName`: The path to the shader code
 * - `entryPoint`: The entry point name, generally `"main"`
 * - returns: A program object on success, `NULL` on error
 */
mc_Program_t* mc_program_create(
    mc_Device_t* device,
    const char* fileName,
    const char* entryPoint
);

/** code
 * Destroy a `mc_Program_t` object.
 *
 * - `self`: A reference to a `mc_Program_t` object
 */
void mc_program_destroy(mc_Program_t* self);

/** code
 * Bind buffers to a `mc_Program_t` object.
 *
 * - `self`: A reference to a `mc_Program_t` object
 * - `dimX`: The number of local workgroups in the x dimension
 * - `dimY`: The number of local workgroups in the y dimension
 * - `dimZ`: The number of local workgroups in the z dimension
 * - `...`: A list of buffers to bind to the program
 * - returns: Time taken to run the program, in seconds, or `-1.0` on error
 */
#define mc_program_run(self, dimX, dimY, dimZ, ...)                            \
    mc_program_run__(self, dimX, dimY, dimZ, ##__VA_ARGS__, NULL)

/** code
 * Get the current time.
 *
 * - returns: The current time, in seconds
 */
double mc_get_time();

/** code
 * Convert a `mc_LogLevel_t` enum to a human readable string.
 *
 * - `level`: A `mc_LogLevel_t` value
 * - returns: A human readable string (`NULL` terminated)
 */
const char* mc_log_level_to_str(mc_LogLevel_t level);

/** code
 * Convert a `mc_DeviceType_t` enum to a human readable string.
 *
 * - `type`: A `mc_DeviceType_t` value
 * - returns: A human readable string (`NULL` terminated)
 */
const char* mc_device_type_to_str(mc_DeviceType_t type);

/** code
 * Default callback function that can be passed to `mc_instance_create()`. Just
 * prints all messages to stdout. Use as a template for your own callback.
 *
 * See `mc_log_cb` for more info about the arguments.
 */
void mc_default_log_cb( //
    mc_LogLevel_t lvl,
    const char* src,
    void* arg,
    char const* file,
    int line,
    const char* fmt,
    ...
);

/** code
 * Wrapped functions. Don't use these directly, use their corresponding macros.
 */
double mc_program_run__(
    mc_Program_t* self,
    uint32_t dimX,
    uint32_t dimY,
    uint32_t dimZ,
    ...
);

#endif // MICROCOMPUTE_H_INCLUDE_GUARD

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

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <vulkan/vulkan.h>

#define MC_LOG(s, l, ...)                                                      \
    if ((s)->log_cb)                                                           \
        (s)->log_cb((l), "mc", (s)->logArg, __FILE__, __LINE__, __VA_ARGS__);

#define MC_LOG_DEBUG(s, ...) MC_LOG((s), MC_LOG_LEVEL_DEBUG, __VA_ARGS__)
#define MC_LOG_INFO(s, ...) MC_LOG((s), MC_LOG_LEVEL_INFO, __VA_ARGS__)
#define MC_LOG_WARN(s, ...) MC_LOG((s), MC_LOG_LEVEL_WARN, __VA_ARGS__)
#define MC_LOG_ERROR(s, ...) MC_LOG((s), MC_LOG_LEVEL_ERROR, __VA_ARGS__)

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

uint32_t defaultReturn[] = {0, 0, 0};

#ifdef MC_ENABLE_VALIDATION_LAYER
static VkBool32 mc_vk_log_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    __attribute__((unused)) VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT* msg,
    void* userData
) {
    mc_Instance_t* inst = (mc_Instance_t*)userData;
    if (!inst->log_cb) return VK_FALSE;

    enum mc_LogLevel lvl;
    switch (severity) {
        case 0x00000010: lvl = MC_LOG_LEVEL_DEBUG; break;
        case 0x00000100: lvl = MC_LOG_LEVEL_WARN; break;
        case 0x00001000: lvl = MC_LOG_LEVEL_ERROR; break;
        default: return VK_FALSE;
    }

    inst->log_cb(lvl, "vk", inst->logArg, "", 0, "%s", msg->pMessage);
    return VK_FALSE;
}
#endif // MC_ENABLE_VALIDATION_LAYER

static size_t mc_read_file(const char* path, char* contents) {
    FILE* fp = fopen(path, "rb");
    if (!fp) return 0;
    fseek(fp, 0, SEEK_END);
    size_t length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (contents) (void)!fread(contents, 1, length, fp);
    fclose(fp);
    return length;
}

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

const char* mc_log_level_to_str(mc_LogLevel_t level) {
    switch (level) {
        case MC_LOG_LEVEL_DEBUG: return "MC_LOG_LEVEL_DEBUG";
        case MC_LOG_LEVEL_INFO: return "MC_LOG_LEVEL_INFO";
        case MC_LOG_LEVEL_WARN: return "MC_LOG_LEVEL_WARN";
        case MC_LOG_LEVEL_ERROR: return "MC_LOG_LEVEL_ERROR";
        default: return "MC_LOG_LEVEL_UNKNOWN"; // just incase
    }
}

const char* mc_device_type_to_str(mc_DeviceType_t type) {
    switch (type) {
        case MC_DEVICE_TYPE_DGPU: return "MC_DEVICE_TYPE_DGPU";
        case MC_DEVICE_TYPE_IGPU: return "MC_DEVICE_TYPE_IGPU";
        case MC_DEVICE_TYPE_VGPU: return "MC_DEVICE_TYPE_VGPU";
        case MC_DEVICE_TYPE_CPU: return "MC_DEVICE_TYPE_CPU";
        default: return "MC_DEVICE_TYPE_OTHER";
    }
}

mc_Instance_t* mc_instance_create(mc_log_cb* log_cb, void* logArg) {
    mc_Instance_t* self = malloc(sizeof *self);
    *self = (mc_Instance_t){0};
    self->log_cb = log_cb;
    self->logArg = logArg;

    MC_LOG_DEBUG(self, "initializing instance");

    VkApplicationInfo appInfo = {0};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "microcompute";
    appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);

#ifdef MC_ENABLE_VALIDATION_LAYER
    MC_LOG_DEBUG(self, "enabling vulkan validation layer");

    VkDebugUtilsMessengerCreateInfoEXT msgInfo = {0};
    msgInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    msgInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
                            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    msgInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                        | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                        | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    msgInfo.pfnUserCallback = mc_vk_log_callback;
    msgInfo.pUserData = self;

    VkInstanceCreateInfo instanceInfo = {0};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.pNext = &msgInfo;
    instanceInfo.enabledLayerCount = 1;
    instanceInfo.ppEnabledLayerNames = (const char*[]){
        "VK_LAYER_KHRONOS_validation",
    };
    instanceInfo.enabledExtensionCount = 1;
    instanceInfo.ppEnabledExtensionNames = (const char*[]){
        "VK_EXT_debug_utils",
    };
#else
    VkInstanceCreateInfo instanceInfo = {0};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo = &appInfo;
#endif // MC_ENABLE_VALIDATION_LAYER

    if (vkCreateInstance(&instanceInfo, NULL, &self->instance)) {
        MC_LOG_ERROR(self, "failed to create vulkan instance");
        mc_instance_destroy(self);
        return NULL;
    }

    uint32_t vkVersion;
    vkEnumerateInstanceVersion(&vkVersion);
    MC_LOG_DEBUG(
        self,
        "vulkan version: %d.%d.%d",
        VK_VERSION_MAJOR(vkVersion),
        VK_VERSION_MINOR(vkVersion),
        VK_VERSION_PATCH(vkVersion)
    );

#ifdef MC_ENABLE_VALIDATION_LAYER
    PFN_vkCreateDebugUtilsMessengerEXT msg_create
        = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            self->instance,
            "vkCreateDebugUtilsMessengerEXT"
        );
    // vkGetInstanceProcAddr() can fail, so just to be sure
    if (msg_create) msg_create(self->instance, &msgInfo, NULL, &self->msg);
#endif // MC_ENABLE_VALIDATION_LAYER

    uint32_t physDevCount = 0;
    if (vkEnumeratePhysicalDevices(self->instance, &physDevCount, NULL)) {
        MC_LOG_ERROR(self, "failed to get vulkan devices");
        mc_instance_destroy(self);
        return NULL;
    }

    MC_LOG_DEBUG(self, "found %d vulkan device(s):", physDevCount);

    VkPhysicalDevice* physDevs = malloc(sizeof *physDevs * physDevCount);
    if (vkEnumeratePhysicalDevices(self->instance, &physDevCount, physDevs)) {
        MC_LOG_ERROR(self, "failed to get vulkan devices");
        mc_instance_destroy(self);
        return NULL;
    }

    self->devs = malloc(sizeof *self->devs * physDevCount);

    uint32_t physIdx = 0, idx = 0;
    while (physIdx < physDevCount) {
        VkPhysicalDevice physDev = physDevs[physIdx];

        uint32_t queuePropsCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(
            physDev,
            &queuePropsCount,
            NULL
        );

        VkQueueFamilyProperties* queueProps
            = malloc(sizeof *queueProps * queuePropsCount);
        vkGetPhysicalDeviceQueueFamilyProperties(
            physDev,
            &queuePropsCount,
            queueProps
        );

        uint32_t queueFamilyIdx = queuePropsCount;
        for (uint32_t i = 0; i < queuePropsCount; i++) {
            if (VK_QUEUE_COMPUTE_BIT & queueProps[i].queueFlags)
                queueFamilyIdx = i;
        }

        free(queueProps);

        if (queueFamilyIdx == queuePropsCount) {
            physIdx++;
            continue;
        }

        float queuePriority = 1.0;
        VkDeviceQueueCreateInfo devQueueInfo = {0};
        devQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        devQueueInfo.queueFamilyIndex = queueFamilyIdx;
        devQueueInfo.queueCount = 1;
        devQueueInfo.pQueuePriorities = &queuePriority;

        VkDeviceCreateInfo devInfo = {0};
        devInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        devInfo.queueCreateInfoCount = 1;
        devInfo.pQueueCreateInfos = &devQueueInfo;

        VkDevice device;
        if (vkCreateDevice(physDev, &devInfo, NULL, &device)) {
            physIdx++;
            continue;
        }

        mc_Device_t* dev = (self->devs[idx] = malloc(sizeof *self->devs[idx]));

        *dev = (mc_Device_t){0};
        dev->logArg = logArg;
        dev->log_cb = log_cb;
        dev->physDev = physDev;
        dev->queueFamilyIdx = queueFamilyIdx;
        dev->dev = device;

        VkPhysicalDeviceMemoryProperties memProps;
        vkGetPhysicalDeviceMemoryProperties(dev->physDev, &memProps);
        for (uint32_t i = 0; i < memProps.memoryHeapCount; i++) {
            if (memProps.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
                dev->memTot = memProps.memoryHeaps[i].size;
        }

        VkPhysicalDeviceProperties devProps;
        vkGetPhysicalDeviceProperties(dev->physDev, &devProps);

        switch (devProps.deviceType) {
            case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                dev->type = MC_DEVICE_TYPE_IGPU;
                break;
            case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                dev->type = MC_DEVICE_TYPE_DGPU;
                break;
            case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
                dev->type = MC_DEVICE_TYPE_VGPU;
                break;
            case VK_PHYSICAL_DEVICE_TYPE_CPU:
                dev->type = MC_DEVICE_TYPE_CPU;
                break;
            default: //
                dev->type = MC_DEVICE_TYPE_OTHER;
                break;
        }

        dev->maxWorkgroupSizeTotal
            = devProps.limits.maxComputeWorkGroupInvocations;

        memcpy(
            dev->maxWorkgroupSizeShape,
            devProps.limits.maxComputeWorkGroupSize,
            sizeof devProps.limits.maxComputeWorkGroupSize
        );

        memcpy(
            dev->maxWorkgroupCount,
            devProps.limits.maxComputeWorkGroupCount,
            sizeof devProps.limits.maxComputeWorkGroupCount
        );

        memcpy(dev->devName, devProps.deviceName, sizeof devProps.deviceName);

        MC_LOG_DEBUG(self, "- found device %s", dev->devName);

        physIdx++;
        idx++;
    }

    free(physDevs);
    self->devCount = idx;

    return self;
}

void mc_instance_destroy(mc_Instance_t* self) {
    if (!self) return;

    MC_LOG_DEBUG(self, "destroying instance");

    for (uint32_t i = 0; i < self->devCount; i++) {
        if (self->devs[i]->dev) vkDestroyDevice(self->devs[i]->dev, NULL);
    }

#ifdef MC_ENABLE_VALIDATION_LAYER
    PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessenger
        = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            self->instance,
            "vkDestroyDebugUtilsMessengerEXT"
        );
    // vkGetInstanceProcAddr() can fail, so just to be sure
    if (vkDestroyDebugUtilsMessenger)
        vkDestroyDebugUtilsMessenger(self->instance, self->msg, NULL);
#endif // MC_ENABLE_VALIDATION_LAYER

    if (self->instance) vkDestroyInstance(self->instance, NULL);
    free(self);
}

uint32_t mc_instance_get_device_count(mc_Instance_t* self) {
    return self ? self->devCount : 0;
}

mc_Device_t** mc_instance_get_devices(mc_Instance_t* self) {
    return self ? self->devs : NULL;
}

mc_DeviceType_t mc_device_get_type(mc_Device_t* self) {
    return self ? self->type : MC_DEVICE_TYPE_OTHER;
}

uint64_t mc_device_get_total_memory_size(mc_Device_t* self) {
    return self ? self->memTot : 0;
}

uint64_t mc_device_get_used_memory_size(mc_Device_t* self) {
    return self ? self->memUse : 0;
}

uint32_t mc_device_get_max_workgroup_size_total(mc_Device_t* self) {
    return self ? self->maxWorkgroupSizeTotal : 0;
}

uint32_t* mc_device_get_max_workgroup_size_shape(mc_Device_t* self) {
    return self ? self->maxWorkgroupSizeShape : defaultReturn;
}

uint32_t* mc_device_get_max_workgroup_count(mc_Device_t* self) {
    return self ? self->maxWorkgroupCount : defaultReturn;
}

char* mc_device_get_name(mc_Device_t* self) {
    return self ? self->devName : NULL;
}

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

    // since the buffer has been mapped, this is all thats needed
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

mc_Program_t* mc_program_create(
    mc_Device_t* device,
    const char* fileName,
    const char* entryPoint
) {
    if (!device) return NULL;
    MC_LOG_DEBUG(
        device,
        "initializing program from %s:%s",
        fileName,
        entryPoint
    );

    mc_Program_t* self = malloc(sizeof *self);
    *self = (mc_Program_t){0};
    self->dev = device;
    self->entryPoint = entryPoint;
    self->buffCount = -1;

    size_t shaderSize = mc_read_file(fileName, NULL);
    if (shaderSize == 0) {
        MC_LOG_ERROR(self->dev, "failed to open %s", fileName);
        mc_program_destroy(self);
        return NULL;
    }

    char* shaderCode = malloc(shaderSize);
    mc_read_file(fileName, shaderCode);

    VkShaderModuleCreateInfo moduleInfo = {0};
    moduleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleInfo.codeSize = shaderSize;
    moduleInfo.pCode = (const uint32_t*)shaderCode;

    if (vkCreateShaderModule(
            self->dev->dev,
            &moduleInfo,
            NULL,
            &self->shaderModule
        )) {
        MC_LOG_ERROR(self->dev, "failed to create vulkan shader module");
        free(shaderCode);
        mc_program_destroy(self);
        return NULL;
    }

    free(shaderCode);

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

double mc_get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)(1000000 * tv.tv_sec + tv.tv_usec) / 1000000.0;
}

void mc_default_log_cb( //
    mc_LogLevel_t lvl,
    const char* src,
    void* arg,
    const char* file,
    int line,
    const char* fmt,
    ...
) {
    va_list args;
    va_start(args, fmt);
    int message_len = vsnprintf(NULL, 0, fmt, args);
    va_start(args, fmt);
    char* message = malloc(message_len + 1);
    vsnprintf(message, message_len + 1, fmt, args);
    va_end(args);

    printf("%s | %s | %s", mc_log_level_to_str(lvl), src, message);

    if (strlen(file) > 0) printf(" (%s:%d)\n", file, line);
    else printf("\n");
}

#endif // MICROCOMPUTE_IMPLEMENTATION