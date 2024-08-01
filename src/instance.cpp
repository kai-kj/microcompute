#include <fmt/core.h>
#include <iostream>
#include <utility>

#include "microcompute.hpp"

using namespace mc;

VkBool32 debugMessageFunc(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    VkDebugUtilsMessengerCallbackDataEXT const* pCallbackData,
    void* /*userData*/
) {
    auto message = std::string(pCallbackData->pMessage);

    auto type = "";
    switch (static_cast<vk::DebugUtilsMessageTypeFlagBitsEXT>(messageType)) {
        case vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral: type = "general"; break;
        case vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation: type = "validation"; break;
        case vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance: type = "performance"; break;
        case vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding: type = "binding"; break;
    }

    switch (static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(messageSeverity)) {
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
            std::cout << "[trace] " << fmt::format("[{}] {}", type, message) << "\n";
            break;
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
            std::cout << "[info] " << fmt::format("[{}] {}", type, message) << "\n";
            break;
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
            std::cout << "[warn] " << fmt::format("[{}] {}", type, message) << "\n";
            break;
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
            std::cout << "[error] " << fmt::format("[{}] {}", type, message) << "\n";
            break;
        default: break;
    }

    return false;
}

std::shared_ptr<Instance> Instance::create(std::string app_name) {
    const auto context = vk::raii::Context();

    const auto application_info = vk::ApplicationInfo(
        app_name.c_str(),
        VK_MAKE_VERSION(0, 1, 0),
        "microcompute",
        VK_MAKE_VERSION(0, 1, 0),
        VK_MAKE_API_VERSION(0, 0, 1, 0)
    );

    const auto instance_layer_names = std::vector{"VK_LAYER_KHRONOS_validation"};

    const auto instance_extension_names = std::vector{VK_EXT_DEBUG_UTILS_EXTENSION_NAME};

    const auto instance_create_info = vk::InstanceCreateInfo(
        vk::InstanceCreateFlags(),
        &application_info,
        instance_layer_names,
        instance_extension_names
    );

    const auto instance = context.createInstance(instance_create_info);

    const auto message_severity_flags = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose
                                      | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo
                                      | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
                                      | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;

    const auto message_type_flags = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
                                  | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
                                  | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;

    const auto messenger_create_info = vk::DebugUtilsMessengerCreateInfoEXT(
        {},
        message_severity_flags,
        message_type_flags,
        &debugMessageFunc,
        nullptr
    );

    const auto messenger = instance.createDebugUtilsMessengerEXT(messenger_create_info);

    return std::make_shared<Instance>(context, instance, messenger);
}

std::string Instance::getVersion() {
    auto version = context_.enumerateInstanceVersion();
    return fmt::format(
        "{}.{}.{}",
        VK_VERSION_MAJOR(version),
        VK_VERSION_MINOR(version),
        VK_VERSION_PATCH(version)
    );
}

Instance::Instance(
    vk::raii::Context context,
    vk::raii::Instance instance,
    vk::raii::DebugUtilsMessengerEXT messenger
) :
    context_(std::move(context)),
    instance_(std::move(instance)),
    messenger_(std::move(messenger)) {}
