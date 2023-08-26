#ifndef MICROCOMPUTE_H_INCLUDE_GUARD
#define MICROCOMPUTE_H_INCLUDE_GUARD

/** text
 * # `microcompute.h`
 *
 * This library contains utilities that can be used to easily run SPIR-V compute
 * shaders using vulkan.
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
 * Get the video memory of a device.
 *
 * - `self`: A reference to a `mc_Device_t` object
 * - returns: The size of the video memory in bytes
 */
uint64_t mc_device_get_memory_size(mc_Device_t* self);

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
 * - returns: A reference to a `mc_Buffer_t` object
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
 * - returns: A reference to a `mc_Program_t` object
 */
mc_Program_t* mc_program_create(mc_Device_t* device, const char* fileName);

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
 * Bind buffers to a `mc_Program_t` object.
 *
 * - `self`: A reference to a `mc_Program_t` object
 * - `entryPoint`: The entry point name, generally `"main"`
 * - `pcSize`: The size of the push constant data, set to 0 to ignore
 * - `...`: A list of buffers to bind to the program
 */
#define mc_program_setup(self, entryPoint, pcSize, ...)                        \
    mc_program_setup__(self, entryPoint, pcSize, ##__VA_ARGS__, NULL)

/** code
 * Run a `mc_Program_t` object.
 *
 * - `self`: A reference to a `mc_Program_t` object
 * - `dimX`: The number of local workgroups in the x dimension
 * - `dimY`: The number of local workgroups in the y dimension
 * - `dimZ`: The number of local workgroups in the z dimension
 * - `pcData`: A reference to the push constant data, pass `NULL` to ignore
 * - returns: The time taken waiting for the compute operation tio finish, in
 *            seconds, -1.0 on fail
 */
double mc_program_run(
    mc_Program_t* self,
    uint32_t dimX,
    uint32_t dimY,
    uint32_t dimZ,
    void* pcData
);

/** code
 * Get the current time.
 *
 * - returns: The current time, in seconds
 */
double mc_get_time();

/** code
 * Wrapped functions. Don't use these directly, use their corresponding macros.
 *
 */
void mc_program_setup__(
    mc_Program_t* self,
    const char* entryPoint,
    uint32_t pcSize,
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
#include <string.h>
#include <sys/time.h>
#include <vulkan/vulkan.h>

#define MC_MSG(s, l, m)                                                        \
    if ((s)->debug_cb) (s)->debug_cb((l), "mc", (m), (s)->debugArg);

#define MC_MSG_INFO(s, m) MC_MSG((s), MC_DEBUG_LEVEL_INFO, (m))
#define MC_MSG_LOW(s, m) MC_MSG((s), MC_DEBUG_LEVEL_LOW, (m))
#define MC_MSG_MEDIUM(s, m) MC_MSG((s), MC_DEBUG_LEVEL_MEDIUM, (m))
#define MC_MSG_HIGH(s, m) MC_MSG((s), MC_DEBUG_LEVEL_HIGH, (m))

struct mc_Device {
    void* debugArg;
    mc_debug_cb* debug_cb;
    VkPhysicalDevice physDev;
    uint32_t queueFamilyIdx;
    VkDevice device;
    char devName[256];
};

struct mc_Instance {
    bool isInitialized;
    void* debugArg;
    mc_debug_cb* debug_cb;
    VkInstance instance;
    uint32_t devCount;
    mc_Device_t** devs;

#ifdef MC_ENABLE_VALIDATION_LAYER
    VkDebugUtilsMessengerEXT msg;
#endif // MC_ENABLE_VALIDATION_LAYER
};

struct mc_Buffer {
    bool isInitialized;
    mc_Device_t* dev;
    uint64_t size;
    void* mappedMemory;
    VkBuffer buffer;
    VkDeviceMemory memory;
};

struct mc_Program {
    bool isInitialized;
    uint32_t pcSize;
    mc_Device_t* dev;
    VkShaderModule shaderModule;
    VkDescriptorSetLayout descSetLayout;
    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;
    VkDescriptorPool descPool;
    VkDescriptorSet descSet;
    VkCommandPool cmdPool;
    VkCommandBuffer cmdBuff;
};

#ifdef MC_ENABLE_VALIDATION_LAYER
static VkBool32 mc_vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    __attribute__((unused)) VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT* message,
    void* userData
) {
    mc_Instance_t* instance = (mc_Instance_t*)userData;
    if (!instance->debug_cb) return VK_FALSE;

    enum mc_DebugLevel level;
    switch (severity) {
        case 0x00000100: level = MC_DEBUG_LEVEL_MEDIUM; break; // warning
        case 0x00001000: level = MC_DEBUG_LEVEL_HIGH; break;   // error
        default: return VK_FALSE;                              // other
    }

    instance->debug_cb(level, "vk", message->pMessage, instance->debugArg);
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
    mc_Instance_t* self = malloc(sizeof *self);
    *self = (mc_Instance_t){0};

    VkApplicationInfo appInfo = {0};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "microcompute";
    appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);

#ifdef MC_ENABLE_VALIDATION_LAYER
    MC_MSG_INFO(self, "enabling vulkan validation layer");

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

    if (vkCreateInstance(&instanceInfo, NULL, &self->instance)) {
        MC_MSG_HIGH(self, "failed to create vulkan instance");
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
    if (vkEnumeratePhysicalDevices(self->instance, &physDevCount, NULL)) {
        MC_MSG_HIGH(self, "failed to get vulkan devices");
        return self;
    }

    VkPhysicalDevice* physDevs = malloc(sizeof *physDevs * physDevCount);
    if (vkEnumeratePhysicalDevices(self->instance, &physDevCount, physDevs)) {
        MC_MSG_HIGH(self, "failed to get vulkan devices");
        return self;
    }

    self->devs = malloc(sizeof *self->devs * physDevCount);

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

        // so its easy to tell if no queue was found
        uint32_t queueFamilyIdx = queuePropsCount;

        // find dedicated compute queue, best case
        for (uint32_t i = 0; i < queuePropsCount; i++) {
            if (VK_QUEUE_COMPUTE_BIT & queueProps[i].queueFlags
                & !(VK_QUEUE_GRAPHICS_BIT & queueProps[i].queueFlags))
                queueFamilyIdx = i;
        }

        if (queueFamilyIdx == queuePropsCount) {
            // find compute capable queue
            for (uint32_t i = 0; i < queuePropsCount; i++) {
                if (VK_QUEUE_COMPUTE_BIT & queueProps[i].queueFlags)
                    queueFamilyIdx = i;
            }
        }

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
        if (vkCreateDevice(physDev, &devInfo, NULL, &device)) {
            physDevIdx++;
            continue;
        }

        self->devs[devIdx] = malloc(sizeof *self->devs[devIdx]);
        self->devs[devIdx]->debugArg = debugArg;
        self->devs[devIdx]->debug_cb = debug_cb;
        self->devs[devIdx]->physDev = physDev;
        self->devs[devIdx]->queueFamilyIdx = queueFamilyIdx;
        self->devs[devIdx]->device = device;

        VkPhysicalDeviceProperties devProps;
        vkGetPhysicalDeviceProperties(self->devs[devIdx]->physDev, &devProps);
        memcpy(
            self->devs[devIdx]->devName,
            devProps.deviceName,
            sizeof devProps.deviceName
        );

        physDevIdx++;
        devIdx++;
    }

    free(physDevs);
    self->devCount = devIdx;

    MC_MSG_INFO(self, "mc_Instance successfully initialized");
    self->isInitialized = true;
    return self;
}

void mc_instance_destroy(mc_Instance_t* self) {
    MC_MSG_INFO(self, "destroying mc_Instance_t");

    for (uint32_t i = 0; i < self->devCount; i++)
        vkDestroyDevice(self->devs[i]->device, NULL);

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

    vkDestroyInstance(self->instance, NULL);
    free(self);
}

bool mc_instance_is_initialized(mc_Instance_t* self) {
    return self->isInitialized;
}

uint32_t mc_instance_get_device_count(mc_Instance_t* self) {
    if (!self->isInitialized) {
        MC_MSG_MEDIUM(self, "mc_Instance_t not initialized");
        return 0;
    }
    return self->devCount;
}

mc_Device_t** mc_instance_get_devices(mc_Instance_t* self) {
    if (!self->isInitialized) {
        MC_MSG_MEDIUM(self, "mc_Instance_t not initialized");
        return NULL;
    }
    return self->devs;
}

bool mc_device_is_discrete_gpu(mc_Device_t* self) {
    VkPhysicalDeviceProperties devProps;
    vkGetPhysicalDeviceProperties(self->physDev, &devProps);
    return devProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
}

bool mc_device_is_integrated_gpu(mc_Device_t* self) {
    VkPhysicalDeviceProperties devProps;
    vkGetPhysicalDeviceProperties(self->physDev, &devProps);
    return devProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
}

uint64_t mc_device_get_memory_size(mc_Device_t* self) {
    VkPhysicalDeviceMemoryProperties memProps;
    vkGetPhysicalDeviceMemoryProperties(self->physDev, &memProps);

    for (uint32_t i = 0; i < memProps.memoryHeapCount; i++) {
        if (memProps.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
            return memProps.memoryHeaps[i].size;
    }

    return 0;
}

char* mc_device_get_name(mc_Device_t* self) {
    return self->devName;
}

mc_Buffer_t* mc_buffer_create(mc_Device_t* device, uint64_t size) {
    mc_Buffer_t* self = malloc(sizeof *self);

    *self = (mc_Buffer_t){0};
    self->size = size;
    self->dev = device;

    VkBufferCreateInfo bufferInfo = {0};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = self->size;
    bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferInfo.queueFamilyIndexCount = 1;
    bufferInfo.pQueueFamilyIndices = &self->dev->queueFamilyIdx;

    if (vkCreateBuffer(self->dev->device, &bufferInfo, NULL, &self->buffer)) {
        MC_MSG_HIGH(self->dev, "failed to create vulkan buffer");
        return self;
    }

    VkMemoryRequirements memReqs;
    vkGetBufferMemoryRequirements(self->dev->device, self->buffer, &memReqs);

    // check the minimum memory size
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
        MC_MSG_HIGH(self->dev, "no suitable memory type found");
        return self;
    }

    VkMemoryAllocateInfo memAllocInfo = {0};
    memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAllocInfo.allocationSize = self->size;
    memAllocInfo.memoryTypeIndex = memTypeIdx;

    if (vkAllocateMemory(
            self->dev->device,
            &memAllocInfo,
            NULL,
            &self->memory
        )) {
        MC_MSG_HIGH(self->dev, "failed to allocate vulkan memory");
        return self;
    }

    vkMapMemory(
        self->dev->device,
        self->memory,
        0,
        self->size,
        0,
        &self->mappedMemory
    );

    if (vkBindBufferMemory(self->dev->device, self->buffer, self->memory, 0)) {
        MC_MSG_HIGH(self->dev, "failed to map memory");
        return self;
    }

    MC_MSG_INFO(self->dev, "mc_Buffer_t successfully initialized");
    self->isInitialized = true;
    return self;
}

mc_Buffer_t* mc_buffer_create_from(
    mc_Device_t* device,
    uint64_t size,
    void* data
) {
    mc_Buffer_t* self = mc_buffer_create(device, size);
    if (!self->isInitialized) return self;

    mc_buffer_write(self, 0, size, data);
    return self;
}

void mc_buffer_destroy(mc_Buffer_t* self) {
    MC_MSG_INFO(self->dev, "destroying mc_Buffer_t");
    vkFreeMemory(self->dev->device, self->memory, NULL);
    vkDestroyBuffer(self->dev->device, self->buffer, NULL);
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
        MC_MSG_MEDIUM(self->dev, "mc_Buffer_t not initialized");
        return 0;
    }

    if (offset + size > self->size) {
        MC_MSG_MEDIUM(self->dev, "offset + size > mc_Buffer_t.size");
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
        MC_MSG_MEDIUM(self->dev, "mc_Buffer_t not initialized");
        return 0;
    }

    if (offset + size > self->size) {
        MC_MSG_MEDIUM(self->dev, "offset + size > mc_Buffer_t.size");
        return 0;
    }

    // since the buffer has been mapped, this is all thats needed
    memcpy(data, (char*)self->mappedMemory + offset, size);
    return size;
}

mc_Program_t* mc_program_create(mc_Device_t* device, const char* fileName) {
    mc_Program_t* self = malloc(sizeof *self);
    *self = (mc_Program_t){0};
    self->dev = device;

    size_t shaderSize = mc_read_file(fileName, NULL);

    if (shaderSize == 0) {
        MC_MSG_HIGH(self->dev, "failed to open shader file");
        return self;
    }

    char* shaderCode = malloc(shaderSize);
    mc_read_file(fileName, shaderCode);

    VkShaderModuleCreateInfo moduleInfo = {0};
    moduleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    moduleInfo.codeSize = shaderSize;
    moduleInfo.pCode = (const uint32_t*)shaderCode;

    if (vkCreateShaderModule(
            self->dev->device,
            &moduleInfo,
            NULL,
            &self->shaderModule
        )) {
        MC_MSG_HIGH(self->dev, "failed to create vulkan shader module");
        free(shaderCode);
        return self;
    }

    free(shaderCode);

    MC_MSG_INFO(self->dev, "mc_Program_t successfully created");
    self->isInitialized = true;
    return self;
}

void mc_program_destroy(mc_Program_t* self) {
    MC_MSG_INFO(self->dev, "destroying mc_Program_t");
    vkFreeCommandBuffers(self->dev->device, self->cmdPool, 1, &self->cmdBuff);
    vkDestroyCommandPool(self->dev->device, self->cmdPool, NULL);
    vkFreeDescriptorSets(self->dev->device, self->descPool, 1, &self->descSet);
    vkDestroyDescriptorPool(self->dev->device, self->descPool, NULL);
    vkDestroyPipeline(self->dev->device, self->pipeline, NULL);
    vkDestroyPipelineLayout(self->dev->device, self->pipelineLayout, NULL);
    vkDestroyDescriptorSetLayout(self->dev->device, self->descSetLayout, NULL);
    vkDestroyShaderModule(self->dev->device, self->shaderModule, NULL);
    free(self);
}

bool mc_program_is_initialized(mc_Program_t* self) {
    return self->isInitialized;
}

void mc_program_setup__(
    mc_Program_t* self,
    const char* entryPoint,
    uint32_t pcSize,
    ...
) {
    if (!self->isInitialized) {
        MC_MSG_MEDIUM(self->dev, "mc_Program_t not initialized");
        return;
    }

    self->pcSize = pcSize;

    // destroy old objects if they exist
    vkFreeCommandBuffers(self->dev->device, self->cmdPool, 1, &self->cmdBuff);
    vkDestroyCommandPool(self->dev->device, self->cmdPool, NULL);
    vkFreeDescriptorSets(self->dev->device, self->descPool, 1, &self->descSet);
    vkDestroyDescriptorPool(self->dev->device, self->descPool, NULL);
    vkDestroyPipeline(self->dev->device, self->pipeline, NULL);
    vkDestroyPipelineLayout(self->dev->device, self->pipelineLayout, NULL);
    vkDestroyDescriptorSetLayout(self->dev->device, self->descSetLayout, NULL);

    va_list args;

    va_start(args, pcSize);
    uint32_t argc = 0;
    while (va_arg(args, void*) != NULL) argc++;
    va_end(args);

    VkDescriptorSetLayoutBinding* descBindings
        = malloc(sizeof *descBindings * argc);

    for (uint32_t i = 0; i < argc; i++) {
        descBindings[i] = (VkDescriptorSetLayoutBinding){0};
        descBindings[i].binding = i;
        descBindings[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descBindings[i].descriptorCount = 1;
        descBindings[i].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    }

    VkDescriptorSetLayoutCreateInfo descLayoutInfo = {0};
    descLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descLayoutInfo.bindingCount = argc;
    descLayoutInfo.pBindings = descBindings;

    if (vkCreateDescriptorSetLayout(
            self->dev->device,
            &descLayoutInfo,
            NULL,
            &self->descSetLayout
        )) {
        MC_MSG_HIGH(self->dev, "failed to create descriptor set layout");
        free(descBindings);
        return;
    }

    free(descBindings);

    VkPushConstantRange pushConstant;
    pushConstant.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    pushConstant.offset = 0;
    pushConstant.size = self->pcSize;

    VkPipelineLayoutCreateInfo pipelineInfo = {0};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineInfo.setLayoutCount = 1;
    pipelineInfo.pSetLayouts = &self->descSetLayout;
    pipelineInfo.pushConstantRangeCount = 1;
    pipelineInfo.pPushConstantRanges = &pushConstant;

    if (vkCreatePipelineLayout(
            self->dev->device,
            &pipelineInfo,
            NULL,
            &self->pipelineLayout
        )) {
        MC_MSG_HIGH(self->dev, "failed to create pipeline layout");
        return;
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

    if (vkCreateComputePipelines(
            self->dev->device,
            0,
            1,
            &computePipelineInfo,
            NULL,
            &self->pipeline
        )) {
        MC_MSG_HIGH(self->dev, "failed to create compute pipeline");
        return;
    }

    VkDescriptorPoolSize descPoolSize = {0};
    descPoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descPoolSize.descriptorCount = argc;

    VkDescriptorPoolCreateInfo descPoolInfo = {0};
    descPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descPoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    descPoolInfo.maxSets = 1;
    descPoolInfo.poolSizeCount = 1;
    descPoolInfo.pPoolSizes = &descPoolSize;

    if (vkCreateDescriptorPool(
            self->dev->device,
            &descPoolInfo,
            NULL,
            &self->descPool
        )) {
        MC_MSG_HIGH(self->dev, "failed to create descriptor pool");
        return;
    }

    VkDescriptorSetAllocateInfo descAllocInfo = {0};
    descAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descAllocInfo.descriptorPool = self->descPool;
    descAllocInfo.descriptorSetCount = 1;
    descAllocInfo.pSetLayouts = &self->descSetLayout;

    if (vkAllocateDescriptorSets(
            self->dev->device,
            &descAllocInfo,
            &self->descSet
        )) {
        MC_MSG_HIGH(self->dev, "failed to allocate descriptor sets");
        return;
    }

    VkCommandPoolCreateInfo cmdPoolInfo = {0};
    cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    cmdPoolInfo.queueFamilyIndex = self->dev->queueFamilyIdx;

    if (vkCreateCommandPool(
            self->dev->device,
            &cmdPoolInfo,
            NULL,
            &self->cmdPool
        )) {
        MC_MSG_HIGH(self->dev, "failed to create command pool");
        return;
    }

    VkDescriptorBufferInfo* descBuffInfo = malloc(sizeof *descBuffInfo * argc);
    VkWriteDescriptorSet* wrtDescSet = malloc(sizeof *wrtDescSet * argc);
    va_start(args, pcSize);

    for (uint32_t i = 0; i < argc; i++) {
        mc_Buffer_t* buffer = va_arg(args, void*);

        if (!buffer->isInitialized) {
            MC_MSG_MEDIUM(self->dev, "found uninitialized buffer");
            return;
        }

        descBuffInfo[i] = (VkDescriptorBufferInfo){0};
        descBuffInfo[i].buffer = buffer->buffer;
        descBuffInfo[i].range = VK_WHOLE_SIZE;

        wrtDescSet[i] = (VkWriteDescriptorSet){0};
        wrtDescSet[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        wrtDescSet[i].dstSet = self->descSet;
        wrtDescSet[i].dstBinding = i;
        wrtDescSet[i].descriptorCount = 1;
        wrtDescSet[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        wrtDescSet[i].pBufferInfo = &descBuffInfo[i];
    }

    va_end(args);

    vkUpdateDescriptorSets(self->dev->device, argc, wrtDescSet, 0, NULL);
    free(descBuffInfo);
    free(wrtDescSet);

    VkCommandBufferAllocateInfo cmdBuffAllocInfo = {0};
    cmdBuffAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBuffAllocInfo.commandPool = self->cmdPool;
    cmdBuffAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdBuffAllocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(
            self->dev->device,
            &cmdBuffAllocInfo,
            &self->cmdBuff
        )) {
        MC_MSG_HIGH(self->dev, "failed to allocate command buffers");
        return;
    }

    MC_MSG_INFO(self->dev, "mc_Program_t successfully setup");
}

double mc_program_run(
    mc_Program_t* self,
    uint32_t dimX,
    uint32_t dimY,
    uint32_t dimZ,
    void* pcData
) {
    if (!self->isInitialized) {
        MC_MSG_MEDIUM(self->dev, "mc_Program_t not initialized");
        return -1.0;
    }

    // an easy mistake to make
    if (dimX * dimY * dimZ == 0) {
        MC_MSG_MEDIUM(self->dev, "at least one dimension is 0");
        return -1.0;
    }

    VkCommandBufferBeginInfo cmdBuffBeginInfo = {0};
    cmdBuffBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBuffBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (vkBeginCommandBuffer(self->cmdBuff, &cmdBuffBeginInfo)) {
        MC_MSG_HIGH(self->dev, "failed to begin command buffer");
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

    if (pcData) {
        vkCmdPushConstants(
            self->cmdBuff,
            self->pipelineLayout,
            VK_SHADER_STAGE_COMPUTE_BIT,
            0,
            self->pcSize,
            pcData
        );
    }

    vkCmdDispatch(self->cmdBuff, dimX, dimY, dimZ);
    if (vkEndCommandBuffer(self->cmdBuff)) {
        MC_MSG_HIGH(self->dev, "failed to end command buffer");
        return -1.0;
    }

    VkQueue queue;
    vkGetDeviceQueue(self->dev->device, self->dev->queueFamilyIdx, 0, &queue);

    VkSubmitInfo submitInfo = {0};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &self->cmdBuff;

    if (vkQueueSubmit(queue, 1, &submitInfo, 0)) {
        MC_MSG_HIGH(self->dev, "failed to submit queue");
        return -1.0;
    }

    double startTime = mc_get_time();
    if (vkQueueWaitIdle(queue)) {
        MC_MSG_HIGH(self->dev, "failed to wait for queue completion");
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