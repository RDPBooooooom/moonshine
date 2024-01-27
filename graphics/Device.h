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

        VkInstance m_vk_instance;
        VkSurfaceKHR m_vk_surface;

        VkDebugUtilsMessengerEXT m_debug_messenger;

        VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
        VkDevice m_vk_device;

        VkQueue m_vk_graphics_queue;
        int32_t m_graphics_queue_family;
        VkQueue m_vk_present_queue;
        int32_t m_present_queue_family;

        VkCommandPool m_vk_command_pool;

    private:

        void create_instance();

        std::vector<const char *> get_required_extensions();

        bool check_glfw_compatability(std::vector<const char *> glfw_extensions, uint32_t extension_count,
                                      std::vector<VkExtensionProperties> available_extensions);

        void pick_physical_device();

        int rate_device_suitability(VkPhysicalDevice device);

        bool check_device_extension_support(VkPhysicalDevice device);

        void create_logical_device();

        void create_command_pool();

        VkCommandBuffer begin_single_time_commands();

        void end_single_time_commands(VkCommandBuffer command_buffer);

    public:
        Device(Window &window);

        ~Device();

        Device(const Device &) = delete;

        Device &operator=(const Device &) = delete;

        Device(Device &&) = delete;

        Device &operator=(Device &&) = delete;

        VkInstance get_vk_instance() { return m_vk_instance; }

        VkDevice get_vk_device() { return m_vk_device; }

        VkPhysicalDevice get_vk_physical_device() { return m_physical_device; }

        VkSurfaceKHR get_vk_surface() { return m_vk_surface; }

        VkQueue get_graphics_queue() { return m_vk_graphics_queue; }

        int32_t get_graphics_queue_family() { return m_graphics_queue_family; }

        VkQueue get_present_queue() { return m_vk_present_queue; }

        int32_t get_present_queue_family() { return m_present_queue_family; }

        VkCommandPool get_command_pool() { return m_vk_command_pool; }

        uint32_t find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties);

        VkPhysicalDeviceProperties properties;

        void
        create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer,
                      VkDeviceMemory &buffer_memory);

        void copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);

        VkFormat
        find_supported_format(const std::vector<VkFormat> &candidates, VkImageTiling tiling,
                              VkFormatFeatureFlags features);

        void
        create_image_with_info(const VkImageCreateInfo &image_info, VkMemoryPropertyFlags properties, VkImage &image,
                               VkDeviceMemory &image_memory);
    };

} // moonshine

#endif //MOONSHINE_DEVICE_H
