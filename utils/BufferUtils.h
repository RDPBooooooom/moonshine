//
// Created by marvin on 04.06.2023.
//

#ifndef MOONSHINE_BUFFERUTILS_H
#define MOONSHINE_BUFFERUTILS_H

#include <vulkan/vulkan_core.h>
#include <stdexcept>
#include "../graphics/Device.h"
#include "CommandBufferUtils.h"

namespace moonshine {

    inline uint32_t find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties, Device *device) {
        VkPhysicalDeviceMemoryProperties mem_properties;
        vkGetPhysicalDeviceMemoryProperties(device->get_vk_physical_device(), &mem_properties);

        for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
            if ((type_filter & (1 << i)) &&
                (mem_properties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    inline void
    create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer,
                  VkDeviceMemory &buffer_memory, Device *device) {
        VkBufferCreateInfo buffer_info{};
        buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.size = size;
        buffer_info.usage = usage;
        buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(device->get_vk_device(), &buffer_info, nullptr, &buffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create buffer!");
        }

        VkMemoryRequirements mem_requirements;
        vkGetBufferMemoryRequirements(device->get_vk_device(), buffer, &mem_requirements);

        VkMemoryAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = mem_requirements.size;
        alloc_info.memoryTypeIndex = find_memory_type(mem_requirements.memoryTypeBits, properties, device);

        /*
         * TODO: vkAllocate should not me called because of maxMemoryAllocationCount. 
         * If you do this for every single buffer it might be exceeded
         * You need to handles this either by yourself or use something like
         * https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
        */
        if (vkAllocateMemory(device->get_vk_device(), &alloc_info, nullptr, &buffer_memory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate buffer memory!");
        }

        vkBindBufferMemory(device->get_vk_device(), buffer, buffer_memory, 0);
    }

    inline void
    copy_buffer(VkBuffer &src_buffer, VkBuffer &dst_buffer, VkDeviceSize size, Device *device,
                VkCommandPool vk_command_pool) {
        VkCommandBuffer commandBuffer = begin_single_time_commands(device, vk_command_pool);

        VkBufferCopy copy_region{};
        copy_region.srcOffset = 0; // Optional
        copy_region.dstOffset = 0; // Optional
        copy_region.size = size;
        vkCmdCopyBuffer(commandBuffer, src_buffer, dst_buffer, 1, &copy_region);

        end_single_time_commands(device, commandBuffer, vk_command_pool);
    }

    inline void copy_buffer_to_image(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, Device *device,
                                     VkCommandPool vk_command_pool) {
        VkCommandBuffer command_buffer = begin_single_time_commands(device, vk_command_pool);

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;

        region.imageOffset = {0, 0, 0};
        region.imageExtent = {
                width,
                height,
                1
        };

        vkCmdCopyBufferToImage(
                command_buffer,
                buffer,
                image,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1,
                &region
        );

        end_single_time_commands(device, command_buffer, vk_command_pool);
    }
}

#endif //MOONSHINE_BUFFERUTILS_H
