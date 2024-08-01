// #include "microcompute.hpp"
//
// using namespace mc;
//
// std::vector<std::shared_ptr<Device>> Device::create_all(std::shared_ptr<Instance> instance) {
//     auto physical_devices = instance->instance_.enumeratePhysicalDevices();
//     auto devices = std::vector<std::shared_ptr<Device>>();
//
//     for (auto &physical_device : physical_devices) {
//         const auto properties = physical_device.getProperties();
//
//         const auto queue_family_properties = physical_device.getQueueFamilyProperties();
//         const auto queue_family_property = std::find_if(
//             queue_family_properties.begin(),
//             queue_family_properties.end(),
//             [](auto queue_family_property) {
//                 return queue_family_property.queueFlags & vk::QueueFlagBits::eCompute;
//             }
//         );
//
//         const auto queue_family_index
//             = std::distance(queue_family_properties.begin(), queue_family_property);
//
//         auto queue_priority = 0.0F;
//
//         const auto device_queue_create_info
//             = vk::DeviceQueueCreateInfo({}, queue_family_index, 1, &queue_priority);
//
//         const auto device_create_info = vk::DeviceCreateInfo({}, device_queue_create_info);
//
//         const auto device = vk::raii::Device(physical_device, device_create_info);
//
//         // auto allocator_create_info = VmaAllocatorCreateInfo{
//         //     .flags = {},
//         //     .physicalDevice = static_cast<VkPhysicalDevice>(*physical_device),
//         //     .device = <vk::raii::Device::CType>(*device),
//         //     .preferredLargeHeapBlockSize = 0,
//         //     .pAllocationCallbacks = nullptr,
//         //     .pDeviceMemoryCallbacks = nullptr,
//         //     .pHeapSizeLimit = 0,
//         //     .pVulkanFunctions = nullptr,
//         //     .instance = instance,
//         //     .vulkanApiVersion = VK_MAKE_API_VERSION(0, 0, 1, 0),
//         // };
//
//         devices.push_back(std::make_shared<Device>(instance, device));
//     }
//
//     return devices;
// }
//
// std::string Device::getName() {
//     return properties_.deviceName;
// }
//
// Device::Type Device::getType() {
//     switch (properties_.deviceType) {
//         case vk::PhysicalDeviceType::eDiscreteGpu: return DGPU;
//         case vk::PhysicalDeviceType::eIntegratedGpu: return IGPU;
//         case vk::PhysicalDeviceType::eVirtualGpu: return VGPU;
//         default: return Other;
//     }
// }
//
// std::tuple<int, int, int> Device::getMaxWorkgroupCount() {
//     return {
//         properties_.limits.maxComputeWorkGroupCount[0],
//         properties_.limits.maxComputeWorkGroupCount[1],
//         properties_.limits.maxComputeWorkGroupCount[2]
//     };
// }
//
// std::tuple<int, int, int> Device::getMaxWorkgroupSize() {
//     return {
//         properties_.limits.maxComputeWorkGroupSize[0],
//         properties_.limits.maxComputeWorkGroupSize[1],
//         properties_.limits.maxComputeWorkGroupSize[2]
//     };
// }
//
// Device::Device(std::shared_ptr<Instance> instance, vk::raii::Device device) :
//     instance_(instance),
//     device_(std::move(device)) {}
