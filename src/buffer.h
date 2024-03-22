#ifndef MC_BUFFER_H
#define MC_BUFFER_H

#include <vulkan/vulkan.h>

#include "microcompute.h"

VkBuffer mc_buffer_get_vk_buffer(mc_Buffer* buffer);

#endif // MC_BUFFER_H