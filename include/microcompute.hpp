#ifndef MICROCOMPUTE_HPP
#define MICROCOMPUTE_HPP

#include <memory>
#include <string>
#include <vector>

#include <vulkan/vulkan_raii.hpp>

namespace mc {
    class Device;

    class Instance {
        public:
            static std::shared_ptr<Instance> create(std::string app_name);

            std::string getVersion();

        private:
            vk::raii::Context context_;
            vk::raii::Instance instance_;
            vk::raii::DebugUtilsMessengerEXT messenger_;

            Instance(
                vk::raii::Context context,
                vk::raii::Instance instance,
                vk::raii::DebugUtilsMessengerEXT messenger
            );

            friend class Device;
    };

    // class Device {
    //     public:
    //         enum Type : uint8_t { DGPU, IGPU, VGPU, CPU, Other };
    //
    //         static std::vector<std::shared_ptr<Device>> create_all(
    //             std::shared_ptr<Instance> instance
    //         );
    //
    //         std::string getName();
    //
    //         Type getType();
    //
    //         std::tuple<int, int, int> getMaxWorkgroupSize();
    //
    //         std::tuple<int, int, int> getMaxWorkgroupCount();
    //
    //     private:
    //         std::shared_ptr<Instance> instance_;
    //         vk::raii::Device device_;
    //         vk::PhysicalDeviceProperties properties_;
    //
    //         Device(std::shared_ptr<Instance> instance, vk::raii::Device device);
    // };

} // namespace mc

#endif // MICROCOMPUTE_HPP
