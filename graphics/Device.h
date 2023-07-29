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

        VkCommandPool m_vkCommandPool;

    private:

        void createInstance();

        std::vector<const char *> getRequiredExtensions();

        bool checkGLFWCompatability(std::vector<const char *> glfwExtensions, uint32_t extensionCount,
                                    std::vector<VkExtensionProperties> availableExtensions);

        void pickPhysicalDevice();

        int rateDeviceSuitability(VkPhysicalDevice device);

        bool checkDeviceExtensionSupport(VkPhysicalDevice device);

        void createLogicalDevice();

        void createCommandPool();

        VkCommandBuffer beginSingleTimeCommands();

        void endSingleTimeCommands(VkCommandBuffer commandBuffer);

    public:
        Device(Window &window);

        ~Device();

        Device(const Device &) = delete;

        Device &operator=(const Device &) = delete;

        Device(Device &&) = delete;

        Device &operator=(Device &&) = delete;

        VkInstance getVkInstance() { return m_vkInstance; }

        VkDevice getVkDevice() { return m_vkDevice; }

        VkPhysicalDevice getVkPhysicalDevice() { return m_physicalDevice; }

        VkSurfaceKHR getVkSurface() { return m_vkSurface; }

        VkQueue getGraphicsQueue() { return m_vkGraphicsQueue; }

        VkQueue getPresentQueue() { return m_vkPresentQueue; }

        VkCommandPool getCommandPool() { return m_vkCommandPool; }

        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        VkPhysicalDeviceProperties properties;

        void
        createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer,
                     VkDeviceMemory &bufferMemory);


        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    };

} // moonshine

#endif //MOONSHINE_DEVICE_H
