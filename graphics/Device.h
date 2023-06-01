//
// Created by marvin on 01.06.2023.
//

#ifndef MOONSHINE_DEVICE_H
#define MOONSHINE_DEVICE_H

#include <vulkan/vulkan_core.h>
#include "Window.h"
#include "../utils/VkUtils.h"

namespace moonshine {

    class Device {

    private:
        Window m_window;

        VkInstance m_vkInstance;
        VkSurfaceKHR m_vkSurface;

        VkDebugUtilsMessengerEXT m_debugMessenger;

        VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
        VkDevice m_vkDevice;

        VkQueue m_vkGraphicsQueue;
        VkQueue m_vkPresentQueue;

    public:


    private:

        void createInstance();

        std::vector<const char *> getRequiredExtensions();

        bool checkGLFWCompatability(std::vector<const char *> glfwExtensions, uint32_t extensionCount,
                                    std::vector<VkExtensionProperties> availableExtensions);

        void pickPhysicalDevice();

        int rateDeviceSuitability(VkPhysicalDevice device);

        bool checkDeviceExtensionSupport(VkPhysicalDevice device);

        void createLogicalDevice();

    public:
        Device(Window &window);

        ~Device();

        VkInstance getVkInstance() { return m_vkInstance; }

        VkDevice getVkDevice() { return m_vkDevice; }

        VkPhysicalDevice getVkPhysicalDevice() { return m_physicalDevice; }

        VkSurfaceKHR getVkSurface() { return m_vkSurface; }

        VkQueue getGraphicsQueue() { return m_vkGraphicsQueue; }

        VkQueue getPresentQueue() { return m_vkPresentQueue; }
    };

} // moonshine

#endif //MOONSHINE_DEVICE_H
