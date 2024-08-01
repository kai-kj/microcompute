// #ifndef INSTANCE_HPP
// #define INSTANCE_HPP
//
// #include <vulkan/vulkan_raii.hpp>
//
// #include "microcompute.hpp"
//
// class InstanceImpl final : public mc::Instance, public std::enable_shared_from_this<InstanceImpl>
// {
//     public:
//         friend class mc::Instance;
//
//         InstanceImpl(
//             vk::raii::Context context,
//             vk::raii::Instance instance,
//             vk::raii::DebugUtilsMessengerEXT messenger,
//             std::shared_ptr<mc::Logger> logger
//         );
//
//         ~InstanceImpl() override;
//
//         std::string getVersion() override;
//
//         std::vector<std::shared_ptr<mc::Device>> getDevices() override;
//
//     private:
//         std::shared_ptr<mc::Logger> logger_;
//         vk::raii::Context context_;
//         vk::raii::Instance instance_;
//         vk::raii::DebugUtilsMessengerEXT messenger_;
//
//         static VkBool32 debugMessageFunc(
//             VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
//             VkDebugUtilsMessageTypeFlagsEXT messageTypes,
//             VkDebugUtilsMessengerCallbackDataEXT const* pCallbackData,
//             void* userData
//         );
// };
//
// #endif // INSTANCE_HPP