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
} mc_DebugLevel;

/** code
 * The debug callback type passed to `mc_set_debug_cb()`.
 *
 * - `level`: A `mc_DebugLevel` indicating the severity of the message
 * - `source`: The message source (NULL terminated)
 * - `msg`: The message contents (NULL terminated)
 * - `arg`: The value passed to `debugArg` in `mc_instance_create()`
 */
typedef void(mc_debug_cb)(
    mc_DebugLevel level,
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
 * Convert a `mc_DebugLevel` enum to a human readable string.
 *
 * - `level`: A `mc_DebugLevel` value
 * - returns: A human readable string (`NULL` terminated)
 */
const char* mc_debug_level_to_str(mc_DebugLevel level);

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

#define MC_EMIT_MESSAGE(self, level, source, message)                          \
    do {                                                                       \
        if ((self)->debug_cb)                                                  \
            (self)->debug_cb((level), (source), (message), (self)->debugArg);  \
    } while (0)

struct mc_Device {
    void* debugArg;
    mc_debug_cb* debug_cb;

    VkPhysicalDevice physicalDevice;
    uint32_t queueFamilyIndex;
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
    VkDebugUtilsMessengerEXT messenger;
    PFN_vkCreateDebugUtilsMessengerEXT create_messenger;
    PFN_vkDestroyDebugUtilsMessengerEXT destroy_messenger;
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
    VkDescriptorSetLayout descriptorSetLayout;

    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;

    VkDescriptorPool descriptorPool;
    VkDescriptorSet descriptorSet;

    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;

    uint32_t bufferCount;
    VkDescriptorBufferInfo* descriptorBufferInfo;
    VkWriteDescriptorSet* writeDescriptorSet;
};

#ifdef MC_ENABLE_VALIDATION_LAYER
static VkBool32 mc_vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    __attribute__((unused)) VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT* message,
    void* userData
) {
    switch (severity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            MC_EMIT_MESSAGE(
                (mc_Instance_t*)userData,
                MC_DEBUG_LEVEL_MEDIUM,
                "Vulkan",
                message->pMessage
            );
            return VK_FALSE;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            MC_EMIT_MESSAGE(
                (mc_Instance_t*)userData,
                MC_DEBUG_LEVEL_HIGH,
                "Vulkan",
                message->pMessage
            );
            return VK_FALSE;
        default: return VK_FALSE; // dont bother with info or verbose
    }
}
#endif // MC_ENABLE_VALIDATION_LAYER

static uint32_t mc_choose_queue_family_index(
    uint32_t queueFamilyPropsCount,
    VkQueueFamilyProperties* queueFamilyProps
) {
    // so its easy to tell if no queue was found
    uint32_t idx = queueFamilyPropsCount;

    // find dedicated compute queue, best case
    for (uint32_t i = 0; i < queueFamilyPropsCount; i++) {
        bool g = VK_QUEUE_GRAPHICS_BIT & queueFamilyProps[i].queueFlags;
        bool c = VK_QUEUE_COMPUTE_BIT & queueFamilyProps[i].queueFlags;
        if (!g && c) idx = i;
    }

    if (idx != queueFamilyPropsCount) return idx;

    // find compute capable queue
    for (uint32_t i = 0; i < queueFamilyPropsCount; i++) {
        VkQueueFlags c = VK_QUEUE_COMPUTE_BIT & queueFamilyProps[i].queueFlags;
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

        // just return the first memory type that works
        if (v && c && size < heap.size) return i;
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

const char* mc_debug_level_to_str(mc_DebugLevel level) {
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

    *self = (mc_Instance_t){
        .isInitialized = false,

        .debugArg = debugArg,
        .debug_cb = debug_cb,

        .instance = NULL,
        .deviceCount = 0,
        .devices = NULL,

#ifdef MC_ENABLE_VALIDATION_LAYER
        .messenger = NULL,
        .create_messenger = NULL,
        .destroy_messenger = NULL,
#endif // MC_ENABLE_VALIDATION_LAYER
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

#ifdef MC_ENABLE_VALIDATION_LAYER
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

    MC_EMIT_MESSAGE(
        self,
        MC_DEBUG_LEVEL_INFO,
        "mc_Instance_t",
        "MC_ENABLE_VALIDATION_LAYER enabled"
    );
#endif // MC_ENABLE_VALIDATION_LAYER

    VkInstanceCreateInfo instanceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .flags = 0,
        .pApplicationInfo = &applicationInfo,
#ifdef MC_ENABLE_VALIDATION_LAYER
        .pNext = &messengerCreateInfo, // for validation during vkCreateInstance
        .enabledLayerCount = 1,
        .ppEnabledLayerNames = (const char*[]){"VK_LAYER_KHRONOS_validation"},
        .enabledExtensionCount = 1,
        .ppEnabledExtensionNames = (const char*[]){"VK_EXT_debug_utils"},
#else
        .pNext = NULL,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = NULL,
        .enabledExtensionCount = 0,
        .ppEnabledExtensionNames = NULL,
#endif // MC_ENABLE_VALIDATION_LAYER
    };

    res = vkCreateInstance(&instanceCreateInfo, NULL, &self->instance);

    if (res != VK_SUCCESS) {
        MC_EMIT_MESSAGE(
            self,
            MC_DEBUG_LEVEL_HIGH,
            "mc_Instance_t",
            "vkCreateInstance() failed"
        );
        return self;
    }

#ifdef MC_ENABLE_VALIDATION_LAYER
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

    // vkGetInstanceProcAddr() can fail, so just to be sure
    if (self->create_messenger) {
        self->create_messenger(
            self->instance,
            &messengerCreateInfo,
            NULL,
            &self->messenger
        );
    }
#endif // MC_ENABLE_VALIDATION_LAYER

    uint32_t physicalDeviceCount = 0;
    res = vkEnumeratePhysicalDevices(
        self->instance,
        &physicalDeviceCount,
        NULL
    );

    if (res != VK_SUCCESS) {
        MC_EMIT_MESSAGE(
            self,
            MC_DEBUG_LEVEL_HIGH,
            "mc_Instance_t",
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
        MC_EMIT_MESSAGE(
            self,
            MC_DEBUG_LEVEL_HIGH,
            "mc_Instance_t",
            "vkEnumeratePhysicalDevices() failed"
        );
        return self;
    }

    self->devices = malloc(sizeof *self->devices * physicalDeviceCount);

    uint32_t physicalDeviceIndex = 0, deviceIndex = 0;
    while (physicalDeviceIndex < physicalDeviceCount) {
        VkPhysicalDevice physicalDevice = physicalDevices[physicalDeviceIndex];

        uint32_t queueFamilyPropertiesCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(
            physicalDevice,
            &queueFamilyPropertiesCount,
            NULL
        );

        VkQueueFamilyProperties* queueFamilyProperties = malloc(
            sizeof *queueFamilyProperties * queueFamilyPropertiesCount
        );

        vkGetPhysicalDeviceQueueFamilyProperties(
            physicalDevice,
            &queueFamilyPropertiesCount,
            queueFamilyProperties
        );

        uint32_t queueFamilyIndex = mc_choose_queue_family_index(
            queueFamilyPropertiesCount,
            queueFamilyProperties
        );

        free(queueFamilyProperties);

        if (queueFamilyIndex == queueFamilyPropertiesCount) {
            physicalDeviceIndex++;
            continue;
        }

        float queuePriority = 1.0;
        VkDeviceQueueCreateInfo deviceQueueCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .queueFamilyIndex = queueFamilyIndex,
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

        VkDevice device;
        res = vkCreateDevice(physicalDevice, &deviceCreateInfo, NULL, &device);

        if (res != VK_SUCCESS) {
            physicalDeviceIndex++;
            continue;
        }

        self->devices[deviceIndex] = malloc(sizeof *self->devices[deviceIndex]);
        *self->devices[deviceIndex] = (mc_Device_t){
            .debugArg = debugArg,
            .debug_cb = debug_cb,
            .physicalDevice = physicalDevice,
            .queueFamilyIndex = queueFamilyIndex,
            .device = device,
        };

        physicalDeviceIndex++;
        deviceIndex++;
    }

    free(physicalDevices);

    self->deviceCount = deviceIndex;
    self->isInitialized = true;
    return self;
}

void mc_instance_destroy(mc_Instance_t* self) {
    MC_EMIT_MESSAGE(
        self,
        MC_DEBUG_LEVEL_INFO,
        "mc_Instance_t",
        "destroying mc_Instance_t"
    );

    for (uint32_t i = 0; i < self->deviceCount; i++)
        vkDestroyDevice(self->devices[i]->device, NULL);

#ifdef MC_ENABLE_VALIDATION_LAYER
    // self->destroy_messenger might be NULL, so just to be sure
    if (self->messenger && self->destroy_messenger) {
        self->destroy_messenger(self->instance, self->messenger, NULL);
        self->messenger = NULL;
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
        MC_EMIT_MESSAGE(
            self,
            MC_DEBUG_LEVEL_MEDIUM,
            "mc_Instance_t",
            "mc_Instance_t not initialized"
        );
        return 0;
    }
    return self->deviceCount;
}

mc_Device_t** mc_instance_get_devices(mc_Instance_t* self) {
    if (!self->isInitialized) {
        MC_EMIT_MESSAGE(
            self,
            MC_DEBUG_LEVEL_MEDIUM,
            "mc_Instance_t",
            "mc_Instance_t not initialized"
        );
        return NULL;
    }
    return self->devices;
}

bool mc_device_is_discrete_gpu(mc_Device_t* device) {
    VkPhysicalDeviceProperties deviceProps;
    vkGetPhysicalDeviceProperties(device->physicalDevice, &deviceProps);
    return deviceProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
}

bool mc_device_is_integrated_gpu(mc_Device_t* device) {
    VkPhysicalDeviceProperties deviceProps;
    vkGetPhysicalDeviceProperties(device->physicalDevice, &deviceProps);
    return deviceProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
}

mc_Buffer_t* mc_buffer_create(mc_Device_t* device, uint64_t size) {
    VkResult res;
    mc_Buffer_t* self = malloc(sizeof *self);

    *self = (mc_Buffer_t){
        .isInitialized = false,
        .size = size,
        .mappedMemory = NULL,
        .device = device,
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
        .pQueueFamilyIndices = &self->device->queueFamilyIndex,
    };

    res = vkCreateBuffer(
        self->device->device,
        &createInfo,
        NULL,
        &self->buffer
    );

    if (res != VK_SUCCESS) {
        MC_EMIT_MESSAGE(
            self->device,
            MC_DEBUG_LEVEL_HIGH,
            "mc_Buffer_t",
            "vkCreateBuffer() failed"
        );
        return self;
    }

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(
        self->device->device,
        self->buffer,
        &memoryRequirements
    );

    // check the minimum memory requirement
    if (memoryRequirements.size > self->size)
        self->size = memoryRequirements.size;

    VkPhysicalDeviceMemoryProperties memoryProps;
    vkGetPhysicalDeviceMemoryProperties(
        self->device->physicalDevice,
        &memoryProps
    );

    uint32_t memoryTypeIdx = mc_choose_memory_type(memoryProps, self->size);
    if (memoryTypeIdx == memoryProps.memoryTypeCount) {
        MC_EMIT_MESSAGE(
            self->device,
            MC_DEBUG_LEVEL_HIGH,
            "mc_Buffer_t",
            "no suitable memory type found"
        );
        return self;
    }

    VkMemoryAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = NULL,
        .allocationSize = self->size,
        .memoryTypeIndex = memoryTypeIdx,
    };

    res = vkAllocateMemory(
        self->device->device,
        &allocInfo,
        NULL,
        &self->memory
    );

    if (res != VK_SUCCESS) {
        MC_EMIT_MESSAGE(
            self->device,
            MC_DEBUG_LEVEL_HIGH,
            "mc_Buffer_t",
            "vkAllocateMemory() failed"
        );
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

    MC_EMIT_MESSAGE(
        self->device,
        MC_DEBUG_LEVEL_INFO,
        "mc_Buffer_t",
        "mc_Buffer_t successfully initialized"
    );

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
    MC_EMIT_MESSAGE(
        self->device,
        MC_DEBUG_LEVEL_INFO,
        "mc_Buffer_t",
        "destroying mc_Buffer_t"
    );

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
        MC_EMIT_MESSAGE(
            self->device,
            MC_DEBUG_LEVEL_MEDIUM,
            "mc_Buffer_t",
            "mc_Buffer_t not initialized"
        );
        return 0;
    }

    if (offset + size > self->size) {
        MC_EMIT_MESSAGE(
            self->device,
            MC_DEBUG_LEVEL_MEDIUM,
            "mc_Buffer_t",
            "offset + size > mc_Buffer_t.size"
        );
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
        MC_EMIT_MESSAGE(
            self->device,
            MC_DEBUG_LEVEL_MEDIUM,
            "mc_Buffer_t",
            "mc_Buffer_t not initialized"
        );
        return 0;
    }

    if (offset + size > self->size) {
        MC_EMIT_MESSAGE(
            self->device,
            MC_DEBUG_LEVEL_MEDIUM,
            "mc_Buffer_t",
            "offset + size > mc_Buffer_t.size"
        );
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
    *self = (mc_Program_t){
        .isInitialized = false,
        .device = device,
        .shaderModule = NULL,
        .descriptorSetLayout = NULL,
        .pipelineLayout = NULL,
        .pipeline = NULL,
        .descriptorPool = NULL,
        .descriptorSet = NULL,
        .commandPool = NULL,
        .commandBuffer = NULL,
        .bufferCount = bufferCount,
        .descriptorBufferInfo = NULL,
        .writeDescriptorSet = NULL,
    };

    size_t shaderSize = mc_read_file(fileName, NULL);

    if (shaderSize == 0) {
        MC_EMIT_MESSAGE(
            self->device,
            MC_DEBUG_LEVEL_MEDIUM,
            "mc_Program_t",
            "failed to open file `fileName`"
        );
        return self;
    }

    char* shaderCode = malloc(shaderSize);
    mc_read_file(fileName, shaderCode);

    VkShaderModuleCreateInfo shaderModuleCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .codeSize = shaderSize,
        .pCode = (const uint32_t*)shaderCode,
    };

    res = vkCreateShaderModule(
        self->device->device,
        &shaderModuleCreateInfo,
        NULL,
        &self->shaderModule
    );

    free(shaderCode);

    if (res != VK_SUCCESS) {
        MC_EMIT_MESSAGE(
            self->device,
            MC_DEBUG_LEVEL_HIGH,
            "mc_Program_t",
            "vkCreateShaderModule() failed"
        );
        return self;
    }

    VkDescriptorSetLayoutBinding* descriptorSetLayoutBindings
        = malloc(sizeof *descriptorSetLayoutBindings * self->bufferCount);

    for (uint32_t i = 0; i < self->bufferCount; i++) {
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
        .bindingCount = self->bufferCount,
        .pBindings = descriptorSetLayoutBindings,
    };

    res = vkCreateDescriptorSetLayout(
        self->device->device,
        &descriptorSetLayoutCreateInfo,
        NULL,
        &self->descriptorSetLayout
    );

    free(descriptorSetLayoutBindings);

    if (res != VK_SUCCESS) {
        MC_EMIT_MESSAGE(
            self->device,
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
        self->device->device,
        &pipelineLayoutCreateInfo,
        NULL,
        &self->pipelineLayout
    );

    if (res != VK_SUCCESS) {
        MC_EMIT_MESSAGE(
            self->device,
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
        .pName = entryPoint,
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
        self->device->device,
        0,
        1,
        &computePipelineCreateInfo,
        NULL,
        &self->pipeline
    );

    if (res != VK_SUCCESS) {
        MC_EMIT_MESSAGE(
            self->device,
            MC_DEBUG_LEVEL_HIGH,
            "mc_Program_t",
            "vkCreateComputePipelines() failed"
        );
        return self;
    }

    VkDescriptorPoolSize descriptorPoolSize = {
        .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        .descriptorCount = self->bufferCount,
    };

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
        .maxSets = 1,
        .poolSizeCount = 1,
        .pPoolSizes = &descriptorPoolSize,
    };

    res = vkCreateDescriptorPool(
        self->device->device,
        &descriptorPoolCreateInfo,
        NULL,
        &self->descriptorPool
    );

    if (res != VK_SUCCESS) {
        MC_EMIT_MESSAGE(
            self->device,
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
        self->device->device,
        &descriptorSetAllocateInfo,
        &self->descriptorSet
    );

    if (res != VK_SUCCESS) {
        MC_EMIT_MESSAGE(
            self->device,
            MC_DEBUG_LEVEL_HIGH,
            "mc_Program_t",
            "vkAllocateDescriptorSets() failed"
        );
        return self;
    }

    VkCommandPoolCreateInfo commandPoolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = self->device->queueFamilyIndex,
    };

    res = vkCreateCommandPool(
        self->device->device,
        &commandPoolCreateInfo,
        NULL,
        &self->commandPool
    );

    if (res != VK_SUCCESS) {
        MC_EMIT_MESSAGE(
            self->device,
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
        self->device->device,
        &commandBufferAllocateInfo,
        &self->commandBuffer
    );

    if (res != VK_SUCCESS) {
        MC_EMIT_MESSAGE(
            self->device,
            MC_DEBUG_LEVEL_HIGH,
            "mc_Program_t",
            "vkAllocateCommandBuffers() failed"
        );
        return self;
    }

    MC_EMIT_MESSAGE(
        self->device,
        MC_DEBUG_LEVEL_INFO,
        "mc_Program_t",
        "mc_Program_t successfully created"
    );

    self->descriptorBufferInfo
        = malloc(sizeof *self->descriptorBufferInfo * self->bufferCount);

    self->writeDescriptorSet
        = malloc(sizeof *self->writeDescriptorSet * self->bufferCount);

    self->isInitialized = true;
    return self;
}

void mc_program_destroy(mc_Program_t* self) {
    MC_EMIT_MESSAGE(
        self->device,
        MC_DEBUG_LEVEL_INFO,
        "mc_Program_t",
        "destroying mc_Program_t"
    );

    if (self->commandBuffer) {
        vkFreeCommandBuffers(
            self->device->device,
            self->commandPool,
            1,
            &self->commandBuffer
        );
        self->commandBuffer = NULL;
    }

    if (self->commandPool) {
        vkDestroyCommandPool(self->device->device, self->commandPool, NULL);
        self->commandPool = NULL;
    }

    if (self->descriptorSet) {
        vkFreeDescriptorSets(
            self->device->device,
            self->descriptorPool,
            1,
            &self->descriptorSet
        );
        self->descriptorSet = NULL;
    }

    if (self->descriptorPool) {
        vkDestroyDescriptorPool(
            self->device->device,
            self->descriptorPool,
            NULL
        );
        self->descriptorPool = NULL;
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

    if (self->descriptorSetLayout) {
        vkDestroyDescriptorSetLayout(
            self->device->device,
            self->descriptorSetLayout,
            NULL
        );
        self->descriptorSetLayout = NULL;
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
        MC_EMIT_MESSAGE(
            self->device,
            MC_DEBUG_LEVEL_MEDIUM,
            "mc_Program_t",
            "mc_Program_t not initialized"
        );
        return -1.0;
    }

    // an easy mistake to make
    if (dims.x * dims.y * dims.z == 0) {
        MC_EMIT_MESSAGE(
            self->device,
            MC_DEBUG_LEVEL_MEDIUM,
            "mc_Program_t",
            "at least one dimension is 0"
        );
        return -1.0;
    }

    va_list args;
    va_start(args, dims);

    for (uint32_t i = 0; i < self->bufferCount; i++) {
        mc_Buffer_t* buffer = va_arg(args, void*);

        if (!buffer->isInitialized) {
            MC_EMIT_MESSAGE(
                self->device,
                MC_DEBUG_LEVEL_MEDIUM,
                "mc_Program_t",
                "at least one buffer has not been initialized"
            );
            return -1.0;
        }

        self->descriptorBufferInfo[i] = (VkDescriptorBufferInfo){
            .buffer = buffer->buffer,
            .offset = 0,
            .range = VK_WHOLE_SIZE,
        };

        self->writeDescriptorSet[i] = (VkWriteDescriptorSet){
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = 0,
            .dstSet = self->descriptorSet,
            .dstBinding = i,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .pImageInfo = NULL,
            .pBufferInfo = &self->descriptorBufferInfo[i],
            .pTexelBufferView = NULL,
        };
    }

    va_end(args);

    vkUpdateDescriptorSets(
        self->device->device,
        self->bufferCount,
        self->writeDescriptorSet,
        0,
        NULL
    );

    VkCommandBufferBeginInfo commandBufferBeginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = NULL,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = NULL,
    };

    res = vkBeginCommandBuffer(self->commandBuffer, &commandBufferBeginInfo);
    if (res != VK_SUCCESS) {
        MC_EMIT_MESSAGE(
            self->device,
            MC_DEBUG_LEVEL_HIGH,
            "mc_Program_t",
            "vkBeginCommandBuffer() failed"
        );
        return -1.0;
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

    vkCmdDispatch(self->commandBuffer, dims.x, dims.y, dims.z);
    res = vkEndCommandBuffer(self->commandBuffer);
    if (res != VK_SUCCESS) {
        MC_EMIT_MESSAGE(
            self->device,
            MC_DEBUG_LEVEL_HIGH,
            "mc_Program_t",
            "vkEndCommandBuffer() failed"
        );
        return -1.0;
    }

    VkQueue queue;
    vkGetDeviceQueue(
        self->device->device,
        self->device->queueFamilyIndex,
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
        MC_EMIT_MESSAGE(
            self->device,
            MC_DEBUG_LEVEL_HIGH,
            "mc_Program_t",
            "vkQueueSubmit() failed"
        );
        return -1.0;
    }

    double startTime = mc_get_time();
    res = vkQueueWaitIdle(queue);
    if (res != VK_SUCCESS) {
        MC_EMIT_MESSAGE(
            self->device,
            MC_DEBUG_LEVEL_HIGH,
            "mc_Program_t",
            "vkQueueWaitIdle() failed"
        );
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