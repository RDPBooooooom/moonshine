//
// Created by marvin on 04.06.2023.
//

#ifndef MOONSHINE_GPUBUFFER_H
#define MOONSHINE_GPUBUFFER_H

#include "Device.h"
#include "../utils/BufferUtils.h"

namespace moonshine {
    
    template<typename T>
    class GpuBuffer {
        
    private:
        Device* m_device;
        VkCommandPool m_vkCommandPool;
        std::vector<T> m_buffer;

        VkBuffer m_vkBuffer;
        VkDeviceMemory m_vkBufferMemory;

    public:
        GpuBuffer(std::vector<T> &buffer, Device &device, VkCommandPool vkCommandPool, VkBufferUsageFlagBits vkBufferUsageFlag) : m_device{&device}, m_vkCommandPool{vkCommandPool}, m_buffer{buffer}{
            
            VkDeviceSize bufferSize = sizeof(buffer[0]) * buffer.size();

            VkBuffer stagingBuffer;
            VkDeviceMemory stagingBufferMemory;
            createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
                         stagingBufferMemory, m_device);

            void *data;
            vkMapMemory(m_device->getVkDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
            memcpy(data, buffer.data(), (size_t) bufferSize);
            vkUnmapMemory(m_device->getVkDevice(), stagingBufferMemory);

            createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | vkBufferUsageFlag,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vkBuffer, m_vkBufferMemory, m_device);

            copyBuffer(stagingBuffer, m_vkBuffer, bufferSize, m_device, m_vkCommandPool);

            vkDestroyBuffer(m_device->getVkDevice(), stagingBuffer, nullptr);
            vkFreeMemory(m_device->getVkDevice(), stagingBufferMemory, nullptr);
        };

        ~GpuBuffer() {
            vkDestroyBuffer(m_device->getVkDevice(), m_vkBuffer, nullptr);
            vkFreeMemory(m_device->getVkDevice(), m_vkBufferMemory, nullptr);
        };
        

        VkBuffer &getBuffer() { return m_vkBuffer; }
        VkDeviceMemory &getBufferMemory() { return m_vkBufferMemory; }
        
    };

} // moonshine

#endif //MOONSHINE_GPUBUFFER_H
