//
// Created by marvin on 04.06.2023.
//

#ifndef MOONSHINE_GPUBUFFER_H
#define MOONSHINE_GPUBUFFER_H

#include "Device.h"

namespace moonshine {

    template<typename T>
    class GpuBuffer {

    private:
        Device m_device;
        VkCommandPool m_vkCommandPool;
        std::vector<T> m_buffer;

        VkBuffer m_vkBuffer;
        VkDeviceMemory m_vkBufferMemory;

    public:
        GpuBuffer(std::vector<T> &buffer, Device &device, VkCommandPool &vkCommandPool) : m_device{device}, m_vkCommandPool{vkCommandPool}, m_buffer{buffer}{

            VkDeviceSize bufferSize = sizeof(buffer[0]) * buffer.size();

            VkBuffer stagingBuffer;
            VkDeviceMemory stagingBufferMemory;
            createBufferT(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
                         stagingBufferMemory);

            void *data;
            vkMapMemory(m_device.getVkDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
            memcpy(data, buffer.data(), (size_t) bufferSize);
            vkUnmapMemory(m_device.getVkDevice(), stagingBufferMemory);

            createBufferT(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vkBuffer, m_vkBufferMemory);

            copyBufferT(stagingBuffer, m_vkBuffer, bufferSize);

            vkDestroyBuffer(m_device.getVkDevice(), stagingBuffer, nullptr);
            vkFreeMemory(m_device.getVkDevice(), stagingBufferMemory, nullptr);
        };

        ~GpuBuffer() {
            vkDestroyBuffer(m_device.getVkDevice(), m_vkBuffer, nullptr);
            vkFreeMemory(m_device.getVkDevice(), m_vkBufferMemory, nullptr);
        };
        

        VkBuffer &getBuffer() { return m_vkBuffer; }
        VkDeviceMemory &getBufferMemory() { return m_vkBufferMemory; }

        void
        createBufferT(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer,
                     VkDeviceMemory &bufferMemory) {
            VkBufferCreateInfo bufferInfo{};
            bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferInfo.size = size;
            bufferInfo.usage = usage;
            bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            if (vkCreateBuffer(m_device.getVkDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
                throw std::runtime_error("failed to create buffer!");
            }

            VkMemoryRequirements memRequirements;
            vkGetBufferMemoryRequirements(m_device.getVkDevice(), buffer, &memRequirements);

            VkMemoryAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties, m_device);

            /*
             * TODO: vkAllocate should not me called because of maxMemoryAllocationCount. 
             * If you do this for every single buffer it might be exceeded
             * You need to handles this either by yourself or use something like
             * https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
            */
            if (vkAllocateMemory(m_device.getVkDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
                throw std::runtime_error("failed to allocate buffer memory!");
            }

            vkBindBufferMemory(m_device.getVkDevice(), buffer, bufferMemory, 0);
        }


        void copyBufferT(VkBuffer srcBuffer, VkBuffer &dstBuffer, VkDeviceSize size) {
            VkCommandBufferAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocInfo.commandPool = m_vkCommandPool;
            allocInfo.commandBufferCount = 1;

            VkCommandBuffer commandBuffer;
            vkAllocateCommandBuffers(m_device.getVkDevice(), &allocInfo, &commandBuffer);

            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

            vkBeginCommandBuffer(commandBuffer, &beginInfo);

            VkBufferCopy copyRegion{};
            copyRegion.srcOffset = 0; // Optional
            copyRegion.dstOffset = 0; // Optional
            copyRegion.size = size;
            vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

            vkEndCommandBuffer(commandBuffer);

            VkSubmitInfo submitInfo{};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuffer;

            vkQueueSubmit(m_device.getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
            vkQueueWaitIdle(m_device.getGraphicsQueue());

            vkFreeCommandBuffers(m_device.getVkDevice(), m_vkCommandPool, 1, &commandBuffer);
        }

        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, Device &device) {
            VkPhysicalDeviceMemoryProperties memProperties;
            vkGetPhysicalDeviceMemoryProperties(device.getVkPhysicalDevice(), &memProperties);

            for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
                if ((typeFilter & (1 << i)) &&
                    (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                    return i;
                }
            }

            throw std::runtime_error("failed to find suitable memory type!");
        }
    };

} // moonshine

#endif //MOONSHINE_GPUBUFFER_H
