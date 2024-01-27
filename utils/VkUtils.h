//
// Created by marvin on 29.05.2023.
//

#ifndef MOONSHINE_VKUTILS_H
#define MOONSHINE_VKUTILS_H
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

#include <vulkan/vulkan_core.h>
#include <vector>
#include <optional>
#include<glm/glm.hpp>
#include<glm/gtc/quaternion.hpp>
#include<glm/common.hpp>
#include "array"

namespace moonshine {
    const std::vector<const char *> validation_layers = {
            "VK_LAYER_KHRONOS_validation"
    };

    const std::vector<const char *> device_extensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities{};
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    static SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice
    device,
    VkSurfaceKHR surface
    ) {
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details
    .capabilities);

    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count,
    nullptr);

    if (format_count != 0) {
    details.formats.
    resize(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, details
    .formats.

    data()

    );
}

uint32_t present_mode_count;
vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count,
nullptr);

if (present_mode_count != 0) {
details.presentModes.
resize(present_mode_count);
vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, details
.presentModes.

data()

);
}
return
details;
}

struct QueueFamilyIndices {
    std::optional<uint32_t> graphics_family;
    std::optional<uint32_t> present_family;

    bool is_complete() {
        return graphics_family.has_value() && present_family.has_value();
    }
};


static QueueFamilyIndices find_queue_families(VkPhysicalDevice device, VkSurfaceKHR surface) {
    QueueFamilyIndices indices;
    // Logic to find queue family indices to populate struct with

    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());

    int i = 0;
    for (const auto &queue_family: queue_families) {
        if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphics_family = i;
        }

        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present_support);
        if (present_support) {
            indices.present_family = i;
        }

        if (indices.is_complete()) break;

        i++;
    }

    return indices;
}

/*
 * TODO: Decide if this is the right place for it, probably not
 */

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;
    glm::vec3 normal;

    static VkVertexInputBindingDescription get_vertex_input_binding_description() {
        VkVertexInputBindingDescription binding_description{};
        binding_description.binding = 0;
        binding_description.stride = sizeof(Vertex);
        binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return binding_description;
    }

    static std::array<VkVertexInputAttributeDescription, 4> get_attribute_descriptions() {
        std::array<VkVertexInputAttributeDescription, 4> attribute_descriptions{};
        attribute_descriptions[0].binding = 0;
        attribute_descriptions[0].location = 0;
        attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attribute_descriptions[0].offset = offsetof(Vertex, pos);

        attribute_descriptions[1].binding = 0;
        attribute_descriptions[1].location = 1;
        attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attribute_descriptions[1].offset = offsetof(Vertex, color);

        attribute_descriptions[2].binding = 0;
        attribute_descriptions[2].location = 2;
        attribute_descriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attribute_descriptions[2].offset = offsetof(Vertex, texCoord);

        attribute_descriptions[3].binding = 0;
        attribute_descriptions[3].location = 3;
        attribute_descriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
        attribute_descriptions[3].offset = offsetof(Vertex, normal);

        return attribute_descriptions;
    }
};

struct UniformBufferObject {
    glm::mat4 view;
    glm::mat4 proj;
};

struct MaterialData {
    float ambient = 1;
    float diffuse = 1;
    float specular = 1;
    float shininess = 128;
};

struct DirLight {
    alignas(16) glm::vec3 direction;
    alignas(16) glm::vec3 ambient;
    alignas(16) glm::vec3 diffuse;
    alignas(16) glm::vec3 specular;
};

struct FragmentUniformBufferObject {
    MaterialData material;
    alignas(16) glm::vec3 viewPos;
    alignas(16) DirLight dirLight;
};


}
#endif //MOONSHINE_VKUTILS_H
