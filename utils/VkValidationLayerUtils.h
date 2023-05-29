//
// Created by marvin on 29.05.2023.
//

#ifndef MOONSHINE_VKVALIDATIONLAYERUTILS_H
#define MOONSHINE_VKVALIDATIONLAYERUTILS_H

#include <vulkan/vulkan_core.h>

namespace moonshine {

    // Should Validation layer be enabled
#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

    static VkResult
    CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                 const VkAllocationCallbacks *pAllocator,
                                 VkDebugUtilsMessengerEXT *pDebugMessenger) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,
                                                                               "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }
}

#endif //MOONSHINE_VKVALIDATIONLAYERUTILS_H
