#include <stdlib.h>
#include <vulkan/vulkan.h>

#include "device.h"
#include "instance.h"
#include "log.h"

struct mc_Instance {
    mc_Instance* _instance; // for logging purposes
    void* logArg;
    mc_log_fn* log_fn;
    VkInstance instance;
    uint32_t devCount;
    mc_Device** devs;
    VkDebugUtilsMessengerEXT msg;
};

static VkBool32 mc_vk_log_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    __attribute__((unused)) VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT* msg,
    void* arg
) {
    mc_Instance* vulkan = (mc_Instance*)arg;

    enum mc_LogLevel lvl;
    switch (severity) {
        case 0x00000010: lvl = MC_LOG_LEVEL_DEBUG; break;
        case 0x00000100: lvl = MC_LOG_LEVEL_WARN; break;
        case 0x00001000: lvl = MC_LOG_LEVEL_ERROR; break;
        default: return VK_FALSE;
    }

    LOG(vulkan, lvl, msg->pMessage);
    return VK_FALSE;
}

mc_Instance* mc_instance_create(mc_log_fn* mc_log_fn, void* mcLogArg) {
    mc_Instance* instance = malloc(sizeof *instance);
    *instance = (mc_Instance){0};
    instance->_instance = instance;
    instance->log_fn = mc_log_fn;
    instance->logArg = mcLogArg;

    DEBUG(instance, "initializing instance");

    VkApplicationInfo appI = {0};
    appI.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appI.pApplicationName = "microcompute";
    appI.apiVersion = VK_MAKE_VERSION(1, 0, 0);

    DEBUG(instance, "enabling vulkan validation layer");

    VkDebugUtilsMessengerCreateInfoEXT msgI = {0};
    msgI.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    msgI.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                         | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
                         | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                         | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    msgI.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                     | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                     | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    msgI.pfnUserCallback = mc_vk_log_callback;
    msgI.pUserData = instance;

    VkInstanceCreateInfo instanceI = {0};
    instanceI.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceI.pApplicationInfo = &appI;
    instanceI.pNext = &msgI;
    instanceI.enabledLayerCount = 1;
    instanceI.ppEnabledLayerNames = (const char*[]){
        "VK_LAYER_KHRONOS_validation",
    };
    instanceI.enabledExtensionCount = 1;
    instanceI.ppEnabledExtensionNames = (const char*[]){
        "VK_EXT_debug_utils",
    };

    if (vkCreateInstance(&instanceI, NULL, &instance->instance)) {
        ERROR(instance, "failed to create vulkan instance");
        mc_instance_destroy(instance);
        return NULL;
    }

    uint32_t v;
    vkEnumerateInstanceVersion(&v);
    DEBUG(instance, "vulkan %d.%d", VK_VERSION_MAJOR(v), VK_VERSION_MINOR(v));

    PFN_vkCreateDebugUtilsMessengerEXT msg_create
        = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
            instance->instance,
            "vkCreateDebugUtilsMessengerEXT"
        );
    if (msg_create) msg_create(instance->instance, &msgI, NULL, &instance->msg);

    uint32_t pDevCount = 0;
    if (vkEnumeratePhysicalDevices(instance->instance, &pDevCount, NULL)) {
        ERROR(instance, "failed to get vulkan devices");
        mc_instance_destroy(instance);
        return NULL;
    }

    DEBUG(instance, "found %d vulkan device(s):", pDevCount);

    VkPhysicalDevice* pDevs = malloc(sizeof *pDevs * pDevCount);
    if (vkEnumeratePhysicalDevices(instance->instance, &pDevCount, pDevs)) {
        ERROR(instance, "failed to get vulkan devices");
        mc_instance_destroy(instance);
        return NULL;
    }

    instance->devs = malloc(sizeof *instance->devs * pDevCount);

    uint32_t physIdx = 0, idx = 0;
    while (physIdx < pDevCount) {
        VkPhysicalDevice pDev = pDevs[physIdx];

        uint32_t queuePropsCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(pDev, &queuePropsCount, NULL);

        VkQueueFamilyProperties* queueProps
            = malloc(sizeof *queueProps * queuePropsCount);
        vkGetPhysicalDeviceQueueFamilyProperties(
            pDev,
            &queuePropsCount,
            queueProps
        );

        uint32_t queueIdx = queuePropsCount;
        for (uint32_t i = 0; i < queuePropsCount; i++) {
            if (VK_QUEUE_COMPUTE_BIT & queueProps[i].queueFlags) queueIdx = i;
        }

        free(queueProps);

        if (queueIdx == queuePropsCount) {
            physIdx++;
            continue;
        }

        instance->devs[idx] = mc_device_create(instance, pDev, queueIdx);
        if (!instance->devs[idx]) {
            WARN(instance, "- failed to create devi %d", idx);
            physIdx++;
            continue;
        }

        DEBUG(
            instance,
            "- found devi %d: %s",
            idx,
            mc_device_get_name(instance->devs[idx])
        );

        physIdx++;
        idx++;
    }

    free(pDevs);
    instance->devCount = idx;

    return instance;
}

void mc_instance_destroy(mc_Instance* instance) {
    if (!instance) return;
    DEBUG(instance, "destroying instance");

    if (instance->devs) {
        for (uint32_t i = 0; i < instance->devCount; i++)
            mc_device_destroy(instance->devs[i]);
        free(instance->devs);
    }

    if (instance->msg) {
        PFN_vkDestroyDebugUtilsMessengerEXT msg_destroy
            = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
                instance->instance,
                "vkDestroyDebugUtilsMessengerEXT"
            );
        if (msg_destroy) msg_destroy(instance->instance, instance->msg, NULL);
    }

    if (instance->instance) vkDestroyInstance(instance->instance, NULL);
    free(instance);
}

uint32_t mc_instance_get_device_count(mc_Instance* instance) {
    return instance ? instance->devCount : 0;
}

mc_Device** mc_instance_get_devices(mc_Instance* instance) {
    return instance ? instance->devs : NULL;
}

mc_log_fn* mc_instance_get_log_fn(mc_Instance* instance) {
    if (!instance) return NULL;
    return instance->log_fn ? instance->log_fn : mc_log_cb_sink;
}

void* mc_instance_get_log_arg(mc_Instance* instance) {
    return instance ? instance->logArg : NULL;
}