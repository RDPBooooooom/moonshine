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

    inline uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, Device *device) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(device->getVkPhysicalDevice(), &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) &&
                (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    inline void
    createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer,
                 VkDeviceMemory &bufferMemory, Device *device) {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(device->getVkDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device->getVkDevice(), buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties, device);

        /*
         * TODO: vkAllocate should not me called because of maxMemoryAllocationCount. 
         * If you do this for every single buffer it might be exceeded
         * You need to handles this either by yourself or use something like
         * https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
        */
        if (vkAllocateMemory(device->getVkDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate buffer memory!");
        }

        vkBindBufferMemory(device->getVkDevice(), buffer, bufferMemory, 0);
    }

    inline void
    copyBuffer(VkBuffer &srcBuffer, VkBuffer &dstBuffer, VkDeviceSize size, Device *device,
               VkCommandPool vkCommandPool) {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands(device, vkCommandPool);

        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0; // Optional
        copyRegion.dstOffset = 0; // Optional
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        endSingleTimeCommands(device, commandBuffer, vkCommandPool);
    }

    inline void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, Device *device,
                                  VkCommandPool vkCommandPool) {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands(device, vkCommandPool);

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
                commandBuffer,
                buffer,
                image,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1,
                &region
        );

        endSingleTimeCommands(device, commandBuffer, vkCommandPool);
    }
}

#endif //MOONSHINE_BUFFERUTILS_H
