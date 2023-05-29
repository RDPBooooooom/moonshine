//
// Created by marvin on 29.05.2023.
//

#ifndef MOONSHINE_VKUTILS_H
#define MOONSHINE_VKUTILS_H

#include <vulkan/vulkan_core.h>
#include <vector>

namespace moonshine {
    const std::vector<const char *> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };

    const std::vector<const char *> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
}
#endif //MOONSHINE_VKUTILS_H
