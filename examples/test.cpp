#include <iostream>

#include "microcompute.hpp"

int main() {
    const auto instance = mc::Instance::create("test app");

    std::cout << "using vulkan version " << instance->getVersion() << "\n";
    //
    // const auto devices = mc::Device::create_all(instance);
    //
    // for (const auto &device : devices) {
    //     std::cout << "found device " << device->getName() << "\n";
    // }
}