// #ifndef DEVICE_HPP
// #define DEVICE_HPP
//
// #include <vulkan/vulkan_raii.hpp>
//
// #include "microcompute.hpp"
//
// class DeviceImpl final : public mc::Device {
//     public:
//         DeviceImpl(
//             std::shared_ptr<mc::Instance> instance,
//             vk::raii::PhysicalDevice physical_device,
//             std::shared_ptr<mc::Logger> logger
//         );
//         ~DeviceImpl() override;
//         std::string getName();
//         mc::Device::Type getType();
//         std::tuple<int, int, int> getMaxWorkgroupSize();
//         std::tuple<int, int, int> getMaxWorkgroupCount();
//
//     private:
//         std::shared_ptr<mc::Logger> logger;
//
//         std::shared_ptr<mc::Instance> instance;
//         std::unique_ptr<vk::raii::Device> device;
//         vk::PhysicalDeviceProperties properties;
// };
//
// #endif // DEVICE_HPP