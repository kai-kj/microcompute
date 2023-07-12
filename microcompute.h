#ifndef MICROCOMPUTE_H_INCLUDE_GUARD
#define MICROCOMPUTE_H_INCLUDE_GUARD

/** text
 * # `microcompute.h`
 *
 * This library contains utilities that can be used to easily run compute
 * SPIR-V shaders using vulkan. The library is built to be used with GLSL
 * shaders, although it should also work with other shader types.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/** text
 * ## Types
 */

/** code
 * The severity of a debug message.
 */
typedef enum mc_DebugLevel {
    MC_DEBUG_LEVEL_INFO,
    MC_DEBUG_LEVEL_LOW,
    MC_DEBUG_LEVEL_MEDIUM,
    MC_DEBUG_LEVEL_HIGH,
    MC_DEBUG_LEVEL_UNKNOWN,
} mc_DebugLevel_t;

/** code
 * The debug callback type passed to `mc_set_debug_cb()`.
 *
 * - `level`: A `mc_DebugLevel` indicating the severity of the message
 * - `source`: The message source (NULL terminated)
 * - `msg`: The message contents (NULL terminated)
 * - `arg`: The value passed to `debugArg` in `mc_instance_create()`
 */
typedef void(mc_debug_cb)(
    mc_DebugLevel_t level,
    const char* source,
    const char* msg,
    void* arg
);

/** code
 * Basic scalar data types that can be used in GLSL.
 */
typedef float mc_float_t;
typedef int32_t mc_int_t;
typedef uint32_t mc_uint_t;

/** code
 * The basic vector types that can be used in GLSL.
 */
typedef struct mc_vec2 {
    mc_float_t x, y;
} mc_vec2_t;

typedef struct mc_vec3 {
    mc_float_t x, y, z;
} mc_vec3_t;

typedef struct mc_vec4 {
    mc_float_t x, y, z, w;
} mc_vec4_t;

typedef struct mc_ivec2 {
    mc_int_t x, y;
} mc_ivec2_t;

typedef struct mc_ivec3 {
    mc_int_t x, y, z;
} mc_ivec3_t;

typedef struct mc_ivec4 {
    mc_int_t x, y, z, w;
} mc_ivec4_t;

typedef struct mc_uvec2 {
    mc_uint_t x, y;
} mc_uvec2_t;

typedef struct mc_uvec3 {
    mc_uint_t x, y, z;
} mc_uvec3_t;

typedef struct mc_uvec4 {
    mc_uint_t x, y, z, w;
} mc_uvec4_t;

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
 * Convert a `mc_DebugLevel_t` enum to a human readable string.
 *
 * - `level`: A `mc_DebugLevel_t` value
 * - returns: A human readable string (`NULL` terminated)
 */
const char* mc_debug_level_to_str(mc_DebugLevel_t level);

/** code
 * Create a `mc_Instance_t` object.
 *
 * - `debug_cb`: A function to call when a error occurs, set to `NULL` to ignore
 * - `debugArg`: A value to pass to the debug callback, set to `NULL` to ignore
 * - returns: A reference to a `mc_Instance_t` object
 */
mc_Instance_t* mc_instance_create(mc_debug_cb* debug_cb, void* debugArg);

/** code
 * Destroy a `mc_Instance_t` object.
 *
 * - `self`: A reference to a `mc_Instance_t` object
 */
void mc_instance_destroy(mc_Instance_t* self);

/** code
 * Checks whether a `mc_Instance_t` object has been successfully initialized.
 *
 * - `self`: A reference to a `mc_Instance_t` object
 * - returns: `true` if successful, `false` otherwise
 */
bool mc_instance_is_initialized(mc_Instance_t* self);

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
 * Check if a `mc_Device_t` object is a discrete GPU.
 *
 * - `self`: A reference to a `mc_Device_t` object
 * - returns: `true` if it is, `false` otherwise
 */
bool mc_device_is_discrete_gpu(mc_Device_t* self);

/** code
 * Check if a `mc_Device_t` object is a integrated GPU.
 *
 * - `self`: A reference to a `mc_Device_t` object
 * - returns: `true` if it is, `false` otherwise
 */
bool mc_device_is_integrated_gpu(mc_Device_t* self);

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
 * - returns: A reference to a `mc_Buffer_t` object
 */
mc_Buffer_t* mc_buffer_from(mc_Device_t* device, uint64_t size, void* data);

/** code
 * Destroy a `mc_Buffer_t` object.
 *
 * - `self`: A reference to a `mc_Buffer_t` object
 */
void mc_buffer_destroy(mc_Buffer_t* self);

/** code
 * Checks whether a `mc_Buffer_t` object been successfully initialized.
 *
 * - `self`: A reference to a `mc_Buffer_t` object
 */
bool mc_buffer_is_initialized(mc_Buffer_t* self);

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
 * - `bufferCount`: The number of buffers to bind
 * - returns: A reference to a `mc_Program_t` object
 */
mc_Program_t* mc_program_create(
    mc_Device_t* device,
    const char* fileName,
    const char* entryPoint,
    uint32_t bufferCount
);

/** code
 * Destroy a `mc_Program_t` object.
 *
 * - `self`: A reference to a `mc_Program_t` object
 */
void mc_program_destroy(mc_Program_t* self);

/** code
 * Checks whether a `mc_Program_t` object has been successfully initialized.
 *
 * - `self`: A reference to a `mc_Program_t` object
 * - returns: `true` if successful, `false` otherwise
 */
bool mc_program_is_initialized(mc_Program_t* self);

/** code
 * Run a `mc_Program_t` object.
 *
 * - `self`: A reference to a `mc_Program_t` object
 * - `size`: The number of work groups to dispatch in each dimension
 * - `...`: A list of buffers to bind to the program, must be of the same length
 *          as the `bufferCount` value passed to `mc_program_create()`
 * - returns: The time taken waiting for the compute operation tio finish, in
 *            seconds, -1.0 on fail
 */
double mc_program_run(mc_Program_t* self, mc_uvec3_t dims, ...);

/** code
 * Get the current time.
 *
 * - returns: The current time, in seconds
 */
double mc_get_time();

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
#include <string.h>
#include <sys/time.h>
#include <vulkan/vulkan.h>

#define MC_MESSAGE_(self, level, message)                                      \
    do {                                                                       \
        if ((self)->debug_cb)                                                  \
            (self)->debug_cb((level), "mc", (message), (self)->debugArg);      \
    } while (0)

#define MC_MESSAGE_INFO(self, msg)                                             \
    MC_MESSAGE_((self), MC_DEBUG_LEVEL_INFO, (msg));

#define MC_MESSAGE_LOW(self, msg)                                              \
    MC_MESSAGE_((self), MC_DEBUG_LEVEL_LOW, (msg));

#define MC_MESSAGE_MEDIUM(self, msg)                                           \
    MC_MESSAGE_((self), MC_DEBUG_LEVEL_MEDIUM, (msg));

#define MC_MESSAGE_HIGH(self, msg)                                             \
    MC_MESSAGE_((self), MC_DEBUG_LEVEL_HIGH, (msg));

struct mc_Device {
    void* debugArg;
    mc_debug_cb* debug_cb;
    VkPhysicalDevice physDev;
    uint32_t queueFamilyIdx;
    VkDevice device;
};

struct mc_Instance {
    bool isInitialized;
    void* debugArg;
    mc_debug_cb* debug_cb;
    VkInstance instance;
    uint32_t deviceCount;
    mc_Device_t** devices;

#ifdef MC_ENABLE_VALIDATION_LAYER
    VkDebugUtilsMessengerEXT msg;
#endif // MC_ENABLE_VALIDATION_LAYER
};

struct mc_Buffer {
    bool isInitialized;
    mc_Device_t* device;
    uint64_t size;
    void* mappedMemory;
    VkBuffer buffer;
    VkDeviceMemory memory;
};

struct mc_Program {
    bool isInitialized;
    mc_Device_t* device;
    VkShaderModule shaderModule;
    VkDescriptorSetLayout descSetLayout;
    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;
    VkDescriptorPool descPool;
    VkDescriptorSet descSet;
    VkCommandPool cmdPool;
    VkCommandBuffer cmdBuff;
    uint32_t bufferCount;
    VkDescriptorBufferInfo* descBuffInfo;
    VkWriteDescriptorSet* wrtDescSet;
};

#ifdef MC_ENABLE_VALIDATION_LAYER
static VkBool32 mc_vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    __attribute__((unused)) VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT* message,
    void* userData
) {
    mc_Instance_t* instance = (mc_Instance_t*)userData;
    enum mc_DebugLevel level;
    switch (severity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            level = MC_DEBUG_LEVEL_MEDIUM;
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            level = MC_DEBUG_LEVEL_HIGH;
            break;
        default: return VK_FALSE; // dont bother with info or verbose
    }

    if (instance->debug_cb)
        instance->debug_cb(level, "vk", message->pMessage, instance->debugArg);

    return VK_FALSE;
}
#endif // MC_ENABLE_VALIDATION_LAYER

static uint32_t mc_choose_queue_family_index(
    uint32_t queuePropsCount,
    VkQueueFamilyProperties* queueProps
) {
    uint32_t idx = queuePropsCount; // so its easy to tell if no queue was found

    // find dedicated compute queue, best case
    for (uint32_t i = 0; i < queuePropsCount; i++) {
        bool g = VK_QUEUE_GRAPHICS_BIT & queueProps[i].queueFlags;
        bool c = VK_QUEUE_COMPUTE_BIT & queueProps[i].queueFlags;
        if (!g && c) idx = i;
    }

    if (idx != queuePropsCount) return idx;

    // find compute capable queue
    for (uint32_t i = 0; i < queuePropsCount; i++) {
        VkQueueFlags c = VK_QUEUE_COMPUTE_BIT & queueProps[i].queueFlags;
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
        if (v && c && size < heap.size) return i; // just first valid memory
    }

    // so its easy to tell that the function failed
    return memoryProps.memoryTypeCount;
}

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

const char* mc_debug_level_to_str(mc_DebugLevel_t level) {
    switch (level) {
        case MC_DEBUG_LEVEL_INFO: return "MC_DEBUG_LEVEL_INFO";
        case MC_DEBUG_LEVEL_LOW: return "MC_DEBUG_LEVEL_LOW";
        case MC_DEBUG_LEVEL_MEDIUM: return "MC_DEBUG_LEVEL_MEDIUM";
        case MC_DEBUG_LEVEL_HIGH: return "MC_DEBUG_LEVEL_HIGH";
        default: return "MC_DEBUG_LEVEL_UNKNOWN"; // just incase
    }
}

mc_Instance_t* mc_instance_create(mc_debug_cb* debug_cb, void* debugArg) {
    VkResult res;
    mc_Instance_t* self = malloc(sizeof *self);
    *self = (mc_Instance_t){0};

    VkApplicationInfo appInfo = {0};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "microcompute";
    appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);

#ifdef MC_ENABLE_VALIDATION_LAYER
    MC_MESSAGE_INFO(self, "enabling vulkan validation layer");

    VkDebugUtilsMessengerCreateInfoEXT msgInfo = {0};
    msgInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    msgInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
                            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    msgInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                        | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                        | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    msgInfo.pfnUserCallback = mc_vk_debug_callback;
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

    res = vkCreateInstance(&instanceInfo, NULL, &self->instance);

    if (res != VK_SUCCESS) {
        MC_MESSAGE_HIGH(self, "failed to create vulkan instance");
        return self;
    }

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
    res = vkEnumeratePhysicalDevices(self->instance, &physDevCount, NULL);

    if (res != VK_SUCCESS) {
        MC_MESSAGE_HIGH(self, "failed to get vulkan devices");
        return self;
    }

    VkPhysicalDevice* physDevs = malloc(sizeof *physDevs * physDevCount);
    res = vkEnumeratePhysicalDevices(self->instance, &physDevCount, physDevs);

    if (res != VK_SUCCESS) {
        MC_MESSAGE_HIGH(self, "failed to get vulkan devices");
        return self;
    }

    self->devices = malloc(sizeof *self->devices * physDevCount);

    uint32_t physDevIdx = 0, devIdx = 0;
    while (physDevIdx < physDevCount) {
        VkPhysicalDevice physDev = physDevs[physDevIdx];

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

        uint32_t queueFamilyIdx
            = mc_choose_queue_family_index(queuePropsCount, queueProps);

        free(queueProps);

        if (queueFamilyIdx == queuePropsCount) {
            physDevIdx++;
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
        res = vkCreateDevice(physDev, &devInfo, NULL, &device);

        if (res != VK_SUCCESS) {
            physDevIdx++;
            continue;
        }

        self->devices[devIdx] = malloc(sizeof *self->devices[devIdx]);
        self->devices[devIdx]->debugArg = debugArg;
        self->devices[devIdx]->debug_cb = debug_cb;
        self->devices[devIdx]->physDev = physDev;
        self->devices[devIdx]->queueFamilyIdx = queueFamilyIdx;
        self->devices[devIdx]->device = device;

        physDevIdx++;
        devIdx++;
    }

    free(physDevs);
    self->deviceCount = devIdx;

    MC_MESSAGE_INFO(self, "mc_Instance successfully initialized");
    self->isInitialized = true;
    return self;
}

void mc_instance_destroy(mc_Instance_t* self) {
    MC_MESSAGE_INFO(self, "destroying mc_Instance_t");

    for (uint32_t i = 0; i < self->deviceCount; i++)
        vkDestroyDevice(self->devices[i]->device, NULL);

#ifdef MC_ENABLE_VALIDATION_LAYER
    PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessenger
        = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            self->instance,
            "vkDestroyDebugUtilsMessengerEXT"
        );

    // vkGetInstanceProcAddr() can fail, so just to be sure
    if (self->msg && vkDestroyDebugUtilsMessenger) {
        vkDestroyDebugUtilsMessenger(self->instance, self->msg, NULL);
        self->msg = NULL;
    }
#endif // MC_ENABLE_VALIDATION_LAYER

    if (self->instance) {
        vkDestroyInstance(self->instance, NULL);
        self->instance = NULL;
    }

    free(self);
}

bool mc_instance_is_initialized(mc_Instance_t* self) {
    return self->isInitialized;
}

uint32_t mc_instance_get_device_count(mc_Instance_t* self) {
    if (!self->isInitialized) {
        MC_MESSAGE_MEDIUM(self, "mc_Instance_t not initialized");
        return 0;
    }
    return self->deviceCount;
}

mc_Device_t** mc_instance_get_devices(mc_Instance_t* self) {
    if (!self->isInitialized) {
        MC_MESSAGE_MEDIUM(self, "mc_Instance_t not initialized");
        return NULL;
    }
    return self->devices;
}

bool mc_device_is_discrete_gpu(mc_Device_t* device) {
    VkPhysicalDeviceProperties devProps;
    vkGetPhysicalDeviceProperties(device->physDev, &devProps);
    return devProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
}

bool mc_device_is_integrated_gpu(mc_Device_t* device) {
    VkPhysicalDeviceProperties devProps;
    vkGetPhysicalDeviceProperties(device->physDev, &devProps);
    return devProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
}

mc_Buffer_t* mc_buffer_create(mc_Device_t* device, uint64_t size) {
    VkResult res;
    mc_Buffer_t* self = malloc(sizeof *self);

    *self = (mc_Buffer_t){0};
    self->size = size;
    self->device = device;

    VkBufferCreateInfo bufferInfo = {0};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = self->size;
    bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferInfo.queueFamilyIndexCount = 1;
    bufferInfo.pQueueFamilyIndices = &self->device->queueFamilyIdx;

    res = vkCreateBuffer(
        self->device->device,
        &bufferInfo,
        NULL,
        &self->buffer
    );

    if (res != VK_SUCCESS) {
        MC_MESSAGE_HIGH(self->device, "failed to create vulkan buffer");
        return self;
    }

    VkMemoryRequirements memReqs;
    vkGetBufferMemoryRequirements(self->device->device, self->buffer, &memReqs);

    // check the minimum memory requirement
    if (memReqs.size > self->size) self->size = memReqs.size;

    VkPhysicalDeviceMemoryProperties memProps;
    vkGetPhysicalDeviceMemoryProperties(self->device->physDev, &memProps);

    uint32_t memTypeIdx = mc_choose_memory_type(memProps, self->size);
    if (memTypeIdx == memProps.memoryTypeCount) {
        MC_MESSAGE_HIGH(self->device, "no suitable memory type found");
        return self;
    }

    VkMemoryAllocateInfo memAllocInfo = {0};
    memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAllocInfo.allocationSize = self->size;
    memAllocInfo.memoryTypeIndex = memTypeIdx;

    res = vkAllocateMemory(
        self->device->device,
        &memAllocInfo,
        NULL,
        &self->memory
    );

    if (res != VK_SUCCESS) {
        MC_MESSAGE_HIGH(self->device, "failed to allocate vulkan memory");
        return self;
    }

    // reading / writing to self->memory has the same effect as reading /
    // writing to the buffer
    vkMapMemory(
        self->device->device,
        self->memory,
        0,
        self->size,
        0,
        &self->mappedMemory
    );

    res = vkBindBufferMemory(
        self->device->device,
        self->buffer,
        self->memory,
        0
    );

    if (res != VK_SUCCESS) {
        MC_MESSAGE_HIGH(self->device, "failed to map memory");
        return self;
    }

    MC_MESSAGE_INFO(self->device, "mc_Buffer_t successfully initialized");
    self->isInitialized = true;
    return self;
}

mc_Buffer_t* mc_buffer_from(mc_Device_t* device, uint64_t size, void* data) {
    mc_Buffer_t* self = mc_buffer_create(device, size);
    if (!self->isInitialized) return self;

    mc_buffer_write(self, 0, size, data);
    return self;
}

void mc_buffer_destroy(mc_Buffer_t* self) {
    MC_MESSAGE_INFO(self->device, "destroying mc_Buffer_t");

    if (self->memory) {
        vkFreeMemory(self->device->device, self->memory, NULL);
        self->memory = NULL;
    }

    if (self->buffer) {
        vkDestroyBuffer(self->device->device, self->buffer, NULL);
        self->buffer = NULL;
    }

    free(self);
}

bool mc_buffer_is_initialized(mc_Buffer_t* self) {
    return self->isInitialized;
}

uint64_t mc_buffer_get_size(mc_Buffer_t* self) {
    return self->size;
}

uint64_t mc_buffer_write(
    mc_Buffer_t* self,
    uint64_t offset,
    uint64_t size,
    void* data
) {
    if (!self->isInitialized) {
        MC_MESSAGE_MEDIUM(self->device, "mc_Buffer_t not initialized");
        return 0;
    }

    if (offset + size > self->size) {
        MC_MESSAGE_MEDIUM(self->device, "offset + size > mc_Buffer_t.size");
        return 0;
    }

    // since the buffer has been mapped, this is all thats needed
    memcpy((char*)self->mappedMemory + offset, data, size);
    return size;
}

uint64_t mc_buffer_read(
    mc_Buffer_t* self,
    uint64_t offset,
    uint64_t size,
    void* data
) {
    if (!self->isInitialized) {
        MC_MESSAGE_MEDIUM(self->device, "mc_Buffer_t not initialized");
        return 0;
    }

    if (offset + size > self->size) {
        MC_MESSAGE_MEDIUM(self->device, "offset + size > mc_Buffer_t.size");
        return 0;
    }

    // since the buffer has been mapped, this is all thats needed
    memcpy(data, (char*)self->mappedMemory + offset, size);
    return size;
}

mc_Program_t* mc_program_create(
    mc_Device_t* device,
    const char* fileName,
    const char* entryPoint,
    uint32_t bufferCount
) {
    VkResult res;
    mc_Program_t* self = malloc(sizeof *self);
    *self = (mc_Program_t){0};
    self->device = device;
    self->bufferCount = bufferCount;

    size_t shaderSize = mc_read_file(fileName, NULL);

    if (shaderSize == 0) {
        MC_MESSAGE_HIGH(self->device, "failed to open shader file");
        return self;
    }

    char* shaderCode = malloc(shaderSize);
    mc_read_file(fileName, shaderCode);

    VkShaderModuleCreateInfo moduleInfo = {0};
    moduleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleInfo.codeSize = shaderSize;
    moduleInfo.pCode = (const uint32_t*)shaderCode;

    res = vkCreateShaderModule(
        self->device->device,
        &moduleInfo,
        NULL,
        &self->shaderModule
    );

    free(shaderCode);

    if (res != VK_SUCCESS) {
        MC_MESSAGE_HIGH(self->device, "failed to create vulkan shader module");
        return self;
    }

    VkDescriptorSetLayoutBinding* descBindings
        = malloc(sizeof *descBindings * self->bufferCount);

    for (uint32_t i = 0; i < self->bufferCount; i++) {
        descBindings[i] = (VkDescriptorSetLayoutBinding){0};
        descBindings[i].binding = i;
        descBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descBindings[i].descriptorCount = 1;
        descBindings[i].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    }

    VkDescriptorSetLayoutCreateInfo descLayoutInfo = {0};
    descLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descLayoutInfo.bindingCount = self->bufferCount;
    descLayoutInfo.pBindings = descBindings;

    res = vkCreateDescriptorSetLayout(
        self->device->device,
        &descLayoutInfo,
        NULL,
        &self->descSetLayout
    );

    free(descBindings);

    if (res != VK_SUCCESS) {
        MC_MESSAGE_HIGH(self->device, "failed to create descriptor set layout");
        return self;
    }

    VkPipelineLayoutCreateInfo pipelineInfo = {0};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineInfo.setLayoutCount = 1;
    pipelineInfo.pSetLayouts = &self->descSetLayout;

    res = vkCreatePipelineLayout(
        self->device->device,
        &pipelineInfo,
        NULL,
        &self->pipelineLayout
    );

    if (res != VK_SUCCESS) {
        MC_MESSAGE_HIGH(self->device, "failed to create pipeline layout");
        return self;
    }

    VkPipelineShaderStageCreateInfo shaderStageInfo = {0};
    shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    shaderStageInfo.module = self->shaderModule;
    shaderStageInfo.pName = entryPoint;

    VkComputePipelineCreateInfo computePipelineInfo = {0};
    computePipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    computePipelineInfo.stage = shaderStageInfo;
    computePipelineInfo.layout = self->pipelineLayout;

    res = vkCreateComputePipelines(
        self->device->device,
        0,
        1,
        &computePipelineInfo,
        NULL,
        &self->pipeline
    );

    if (res != VK_SUCCESS) {
        MC_MESSAGE_HIGH(self->device, "failed to create compute pipeline");
        return self;
    }

    VkDescriptorPoolSize descPoolSize = {0};
    descPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descPoolSize.descriptorCount = self->bufferCount;

    VkDescriptorPoolCreateInfo descPoolInfo = {0};
    descPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descPoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    descPoolInfo.maxSets = 1;
    descPoolInfo.poolSizeCount = 1;
    descPoolInfo.pPoolSizes = &descPoolSize;

    res = vkCreateDescriptorPool(
        self->device->device,
        &descPoolInfo,
        NULL,
        &self->descPool
    );

    if (res != VK_SUCCESS) {
        MC_MESSAGE_HIGH(self->device, "failed to create descriptor pool");
        return self;
    }

    VkDescriptorSetAllocateInfo descAllocInfo = {0};
    descAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descAllocInfo.descriptorPool = self->descPool;
    descAllocInfo.descriptorSetCount = 1;
    descAllocInfo.pSetLayouts = &self->descSetLayout;

    res = vkAllocateDescriptorSets(
        self->device->device,
        &descAllocInfo,
        &self->descSet
    );

    if (res != VK_SUCCESS) {
        MC_MESSAGE_HIGH(self->device, "failed to allocate descriptor sets");
        return self;
    }

    VkCommandPoolCreateInfo cmdPoolInfo = {0};
    cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    cmdPoolInfo.queueFamilyIndex = self->device->queueFamilyIdx;

    res = vkCreateCommandPool(
        self->device->device,
        &cmdPoolInfo,
        NULL,
        &self->cmdPool
    );

    if (res != VK_SUCCESS) {
        MC_MESSAGE_HIGH(self->device, "failed to create command pool");
        return self;
    }

    VkCommandBufferAllocateInfo cmdBuffInfo = {0};
    cmdBuffInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBuffInfo.commandPool = self->cmdPool;
    cmdBuffInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdBuffInfo.commandBufferCount = 1;

    res = vkAllocateCommandBuffers(
        self->device->device,
        &cmdBuffInfo,
        &self->cmdBuff
    );

    if (res != VK_SUCCESS) {
        MC_MESSAGE_HIGH(self->device, "failed to allocate command buffers");
        return self;
    }

    self->descBuffInfo = malloc(sizeof *self->descBuffInfo * self->bufferCount);
    self->wrtDescSet = malloc(sizeof *self->wrtDescSet * self->bufferCount);

    MC_MESSAGE_INFO(self->device, "mc_Program_t successfully created");
    self->isInitialized = true;
    return self;
}

void mc_program_destroy(mc_Program_t* self) {
    MC_MESSAGE_INFO(self->device, "destroying mc_Program_t");

    if (self->cmdBuff) {
        vkFreeCommandBuffers(
            self->device->device,
            self->cmdPool,
            1,
            &self->cmdBuff
        );
        self->cmdBuff = NULL;
    }

    if (self->cmdPool) {
        vkDestroyCommandPool(self->device->device, self->cmdPool, NULL);
        self->cmdPool = NULL;
    }

    if (self->descSet) {
        vkFreeDescriptorSets(
            self->device->device,
            self->descPool,
            1,
            &self->descSet
        );
        self->descSet = NULL;
    }

    if (self->descPool) {
        vkDestroyDescriptorPool(self->device->device, self->descPool, NULL);
        self->descPool = NULL;
    }

    if (self->pipeline) {
        vkDestroyPipeline(self->device->device, self->pipeline, NULL);
        self->pipeline = NULL;
    }

    if (self->pipelineLayout) {
        vkDestroyPipelineLayout(
            self->device->device,
            self->pipelineLayout,
            NULL
        );
        self->pipelineLayout = NULL;
    }

    if (self->descSetLayout) {
        vkDestroyDescriptorSetLayout(
            self->device->device,
            self->descSetLayout,
            NULL
        );
        self->descSetLayout = NULL;
    }

    if (self->shaderModule) {
        vkDestroyShaderModule(self->device->device, self->shaderModule, NULL);
        self->shaderModule = NULL;
    }

    free(self);
}

bool mc_program_is_initialized(mc_Program_t* self) {
    return self->isInitialized;
}

double mc_program_run(mc_Program_t* self, mc_uvec3_t dims, ...) {
    VkResult res;

    if (!self->isInitialized) {
        MC_MESSAGE_MEDIUM(self->device, "mc_Program_t not initialized");
        return -1.0;
    }

    // an easy mistake to make
    if (dims.x * dims.y * dims.z == 0) {
        MC_MESSAGE_MEDIUM(self->device, "at least one dimension is 0");
        return -1.0;
    }

    va_list args;
    va_start(args, dims);

    for (uint32_t i = 0; i < self->bufferCount; i++) {
        mc_Buffer_t* buffer = va_arg(args, void*);

        if (!buffer->isInitialized) {
            MC_MESSAGE_MEDIUM(self->device, "found uninitialized buffer");
            return -1.0;
        }

        self->descBuffInfo[i] = (VkDescriptorBufferInfo){0};
        self->descBuffInfo[i].buffer = buffer->buffer;
        self->descBuffInfo[i].range = VK_WHOLE_SIZE;

        self->wrtDescSet[i] = (VkWriteDescriptorSet){0};
        self->wrtDescSet[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        self->wrtDescSet[i].dstSet = self->descSet;
        self->wrtDescSet[i].dstBinding = i;
        self->wrtDescSet[i].descriptorCount = 1;
        self->wrtDescSet[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        self->wrtDescSet[i].pBufferInfo = &self->descBuffInfo[i];
    }

    va_end(args);

    vkUpdateDescriptorSets(
        self->device->device,
        self->bufferCount,
        self->wrtDescSet,
        0,
        NULL
    );

    VkCommandBufferBeginInfo cmdBuffInfo = {0};
    cmdBuffInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBuffInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    res = vkBeginCommandBuffer(self->cmdBuff, &cmdBuffInfo);
    if (res != VK_SUCCESS) {
        MC_MESSAGE_HIGH(self->device, "failed to begin command buffer");
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

    vkCmdDispatch(self->cmdBuff, dims.x, dims.y, dims.z);
    res = vkEndCommandBuffer(self->cmdBuff);
    if (res != VK_SUCCESS) {
        MC_MESSAGE_HIGH(self->device, "failed to end command buffer");
        return -1.0;
    }

    VkQueue queue;
    vkGetDeviceQueue(
        self->device->device,
        self->device->queueFamilyIdx,
        0,
        &queue
    );

    VkSubmitInfo submitInfo = {0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &self->cmdBuff;

    res = vkQueueSubmit(queue, 1, &submitInfo, 0);
    if (res != VK_SUCCESS) {
        MC_MESSAGE_HIGH(self->device, "failed to submit queue");
        return -1.0;
    }

    double startTime = mc_get_time();
    res = vkQueueWaitIdle(queue);
    if (res != VK_SUCCESS) {
        MC_MESSAGE_HIGH(self->device, "failed to wait for queue completion");
        return -1.0;
    }

    return mc_get_time() - startTime;
}

double mc_get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)(1000000 * tv.tv_sec + tv.tv_usec) / 1000000.0;
}

#endif // MICROCOMPUTE_IMPLEMENTATION
#endif // MICROCOMPUTE_H_INCLUDE_GUARD