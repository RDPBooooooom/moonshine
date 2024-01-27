//
// Created by marvin on 01.06.2023.
//

#include <vector>
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <map>
#include <optional>
#include <set>
#include "Device.h"
#include "../utils/VkValidationLayerUtils.h"
#include "GLFW/glfw3.h"
#include "../editor/EngineSystems.h"

namespace moonshine {

    Device::Device(Window &window) : m_window{window} {
        create_instance();
        m_window.create_surface(m_vk_instance, &m_vk_surface);
        set_up_debug_messenger(m_vk_instance, m_debug_messenger);
        pick_physical_device();
        create_logical_device();
        create_command_pool();
    }

    Device::~Device() {
        vkDestroyCommandPool(get_vk_device(), m_vk_command_pool, nullptr);

        vkDestroyDevice(m_vk_device, nullptr);

        if (enable_validation_layers) {
            destroy_debug_utils_messenger_ext(m_vk_instance, m_debug_messenger, nullptr);
        }

        vkDestroySurfaceKHR(m_vk_instance, m_vk_surface, nullptr);

        vkDestroyInstance(m_vk_instance, nullptr);
    }

    void Device::create_instance() {
        if (enable_validation_layers && !check_validation_layer_support()) {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        VkApplicationInfo app_info{};
        app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pApplicationName = "Moonshine";
        app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.pEngineName = "Moonshine";
        app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create_info.pApplicationInfo = &app_info;

        VkDebugUtilsMessengerCreateInfoEXT debug_create_info{};
        if (enable_validation_layers) {
            create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
            create_info.ppEnabledLayerNames = validation_layers.data();

            populate_debug_messenger_create_info(&debug_create_info);
            create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debug_create_info;
        } else {
            create_info.enabledLayerCount = 0;

            create_info.pNext = nullptr;
        }

        auto glfw_extensions = get_required_extensions();
        create_info.enabledExtensionCount = static_cast<uint32_t>(glfw_extensions.size());
        create_info.ppEnabledExtensionNames = glfw_extensions.data();

        uint32_t extension_count = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
        std::vector<VkExtensionProperties> extensions(extension_count);
        vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());

        std::string message = std::string("enabled extensions:\n");

        for (const auto &extension: glfw_extensions) {
            message.append(std::string("\t") + std::string(extension) + std::string("\n"));
        }
        EngineSystems::get_instance().get_logger()->debug(LoggerType::Rendering, message);

        if (!check_glfw_compatability(glfw_extensions, static_cast<uint32_t>(glfw_extensions.size()), extensions)) {
            EngineSystems::get_instance().get_logger()->critical(LoggerType::Rendering, "missing required extension!");
            throw std::runtime_error("missing required extension!");
        }

        if (vkCreateInstance(&create_info, nullptr, &m_vk_instance) != VK_SUCCESS) {
            EngineSystems::get_instance().get_logger()->critical(LoggerType::Rendering, "failed to create instance!");
            throw std::runtime_error("failed to create instance!");
        }
    }

    std::vector<const char *> Device::get_required_extensions() {
        uint32_t glfw_extension_count = 0;
        const char **glfw_extensions;
        glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

        std::vector<const char *> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

        if (enable_validation_layers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    struct compare {
        const char *extension;

        explicit compare(const char *extension) : extension(extension) {}

        bool operator()(VkExtensionProperties &to_compare) {
            return (strcmp(to_compare.extensionName, extension) == 0);
        }
    };

    bool Device::check_glfw_compatability(std::vector<const char *> glfw_extensions, uint32_t extension_count,
                                          std::vector<VkExtensionProperties> available_extensions) {

        if (glfw_extensions.empty()) {
            EngineSystems::get_instance().get_logger()->critical(LoggerType::Rendering,
                                                                 "required extensions not available");
            return false;
        }

        for (int i = 0; i < extension_count; ++i) {
            const char *current_extension = glfw_extensions[i];
            if (std::find_if(available_extensions.begin(), available_extensions.end(),
                             compare(current_extension)) ==
                available_extensions.end()) {
                EngineSystems::get_instance().get_logger()->critical(LoggerType::Rendering,
                                                                     "required extensions not supported: {}",
                                                                     current_extension);
                return false;
            }
            EngineSystems::get_instance().get_logger()->debug(LoggerType::Rendering,
                                                              "Found {}", current_extension);
        }

        return true;
    }

    void Device::pick_physical_device() {
        uint32_t device_count = 0;
        vkEnumeratePhysicalDevices(m_vk_instance, &device_count, nullptr);

        if (device_count == 0) {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(device_count);
        vkEnumeratePhysicalDevices(m_vk_instance, &device_count, devices.data());

        // Use an ordered map to automatically sort candidates by increasing score
        std::multimap<int, VkPhysicalDevice> candidates;

        for (const auto &device: devices) {
            int score = rate_device_suitability(device);
            candidates.insert(std::make_pair(score, device));
        }

        // Check if the best candidate is suitable at all
        if (candidates.rbegin()->first > 0) {
            m_physical_device = candidates.rbegin()->second;
        } else {
            throw std::runtime_error("failed to find a suitable GPU!");
        }

        if (m_physical_device == VK_NULL_HANDLE) {
            throw std::runtime_error("failed to find a suitable GPU!");
        }

        vkGetPhysicalDeviceProperties(m_physical_device, &properties);
    }

    int Device::rate_device_suitability(VkPhysicalDevice device) {

        VkPhysicalDeviceProperties device_properties;
        VkPhysicalDeviceFeatures device_features;
        vkGetPhysicalDeviceProperties(device, &device_properties);
        vkGetPhysicalDeviceFeatures(device, &device_features);

        QueueFamilyIndices indices = find_queue_families(device, m_vk_surface);

        int score = 0;

        // We need at least one queue family
        if (!indices.is_complete()) {
            return 0;
        }

        if (!check_device_extension_support(device)) {
            return 0;
        }

        if (!device_features.samplerAnisotropy) {
            return 0;
        }

        // Do this after extensions
        SwapChainSupportDetails swap_chain_support = query_swap_chain_support(device, m_vk_surface);
        if (swap_chain_support.formats.empty() || swap_chain_support.presentModes.empty()) {
            return 0;
        }

        // Discrete GPUs have a significant performance advantage
        if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            score += 1000;
        }

        return score;
    }

    bool Device::check_device_extension_support(VkPhysicalDevice device) {
        uint32_t extension_count;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

        std::vector<VkExtensionProperties> available_extensions(extension_count);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());

        std::set<std::string> required_extensions(device_extensions.begin(), device_extensions.end());

        for (const auto &extension: available_extensions) {
            required_extensions.erase(extension.extensionName);
        }

        return required_extensions.empty();
    }

    void Device::create_logical_device() {
        QueueFamilyIndices indices = find_queue_families(m_physical_device, m_vk_surface);

        std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
        std::set<uint32_t> unique_queue_families = {indices.graphics_family.value(), indices.present_family.value()};

        float queue_priority = 1.0f;
        for (uint32_t queue_family: unique_queue_families) {
            VkDeviceQueueCreateInfo queue_create_info{};
            queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_info.queueFamilyIndex = queue_family;
            queue_create_info.queueCount = 1;
            queue_create_info.pQueuePriorities = &queue_priority;
            queue_create_infos.push_back(queue_create_info);
        }

        VkPhysicalDeviceFeatures device_features{};
        device_features.samplerAnisotropy = VK_TRUE;

        VkDeviceCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
        create_info.pQueueCreateInfos = queue_create_infos.data();
        create_info.queueCreateInfoCount = 1;

        create_info.pEnabledFeatures = &device_features;

        create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
        create_info.ppEnabledExtensionNames = device_extensions.data();

        // Deprecated, but should be set anyway to support older implementations
        if (enable_validation_layers) {
            create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
            create_info.ppEnabledLayerNames = validation_layers.data();
        } else {
            create_info.enabledLayerCount = 0;
        }

        if (vkCreateDevice(m_physical_device, &create_info, nullptr, &m_vk_device) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }

        m_graphics_queue_family = indices.graphics_family.value();
        m_present_queue_family = indices.present_family.value();

        vkGetDeviceQueue(m_vk_device, indices.graphics_family.value(), 0, &m_vk_graphics_queue);
        vkGetDeviceQueue(m_vk_device, indices.present_family.value(), 0, &m_vk_present_queue);
    }

    void Device::create_command_pool() {
        QueueFamilyIndices queue_family_indices = find_queue_families(get_vk_physical_device(),
                                                                      get_vk_surface());

        VkCommandPoolCreateInfo pool_info{};
        pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        pool_info.queueFamilyIndex = queue_family_indices.graphics_family.value();

        if (vkCreateCommandPool(get_vk_device(), &pool_info, nullptr, &m_vk_command_pool) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create command pool!");
        }
    }

    void
    Device::create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                          VkBuffer &buffer,
                          VkDeviceMemory &buffer_memory) {
        VkBufferCreateInfo buffer_info{};
        buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.size = size;
        buffer_info.usage = usage;
        buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(m_vk_device, &buffer_info, nullptr, &buffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vertex buffer!");
        }


        VkMemoryRequirements mem_requirements;
        vkGetBufferMemoryRequirements(m_vk_device, buffer, &mem_requirements);

        VkMemoryAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = mem_requirements.size;
        alloc_info.memoryTypeIndex = find_memory_type(mem_requirements.memoryTypeBits, properties);

        if (vkAllocateMemory(m_vk_device, &alloc_info, nullptr, &buffer_memory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate vertex buffer memory!");
        }

        vkBindBufferMemory(m_vk_device, buffer, buffer_memory, 0);
    }

    void Device::copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size) {
        VkCommandBuffer command_buffer = begin_single_time_commands();

        VkBufferCopy copy_region{};
        copy_region.srcOffset = 0;  // Optional
        copy_region.dstOffset = 0;  // Optional
        copy_region.size = size;
        vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);

        end_single_time_commands(command_buffer);
    }

    VkCommandBuffer Device::begin_single_time_commands() {
        VkCommandBufferAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandPool = m_vk_command_pool;
        alloc_info.commandBufferCount = 1;

        VkCommandBuffer command_buffer;
        vkAllocateCommandBuffers(m_vk_device, &alloc_info, &command_buffer);

        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(command_buffer, &begin_info);
        return command_buffer;
    }

    void Device::end_single_time_commands(VkCommandBuffer command_buffer) {
        vkEndCommandBuffer(command_buffer);

        VkSubmitInfo submit_info{};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &command_buffer;

        vkQueueSubmit(m_vk_graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
        vkQueueWaitIdle(m_vk_graphics_queue);

        vkFreeCommandBuffers(m_vk_device, m_vk_command_pool, 1, &command_buffer);
    }

    uint32_t Device::find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties) {
        VkPhysicalDeviceMemoryProperties mem_properties;
        vkGetPhysicalDeviceMemoryProperties(m_physical_device, &mem_properties);
        for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
            if ((type_filter & (1 << i)) &&
                (mem_properties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    VkFormat Device::find_supported_format(
            const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
        for (VkFormat format: candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(m_physical_device, format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
                return format;
            } else if (
                    tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
                return format;
            }
        }
        throw std::runtime_error("failed to find supported format!");
    }

    void Device::create_image_with_info(
            const VkImageCreateInfo &image_info,
            VkMemoryPropertyFlags properties,
            VkImage &image,
            VkDeviceMemory &image_memory) {
        if (vkCreateImage(m_vk_device, &image_info, nullptr, &image) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image!");
        }

        VkMemoryRequirements mem_requirements;
        vkGetImageMemoryRequirements(m_vk_device, image, &mem_requirements);

        VkMemoryAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = mem_requirements.size;
        alloc_info.memoryTypeIndex = find_memory_type(mem_requirements.memoryTypeBits, properties);

        if (vkAllocateMemory(m_vk_device, &alloc_info, nullptr, &image_memory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate image memory!");
        }

        if (vkBindImageMemory(m_vk_device, image, image_memory, 0) != VK_SUCCESS) {
            throw std::runtime_error("failed to bind image memory!");
        }
    }
} // moonshine