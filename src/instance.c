#include "microcompute_internal.h"

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
            if (memProps.memoryHeaps[i].flags
                & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
                dev->memTot = memProps.memoryHeaps[i].size;
                break;
            }
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