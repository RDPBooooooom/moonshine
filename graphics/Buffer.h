//
// Created by marvin on 15.06.2023.
//

#ifndef MOONSHINE_BUFFER_H
#define MOONSHINE_BUFFER_H

#include <vulkan/vulkan_core.h>
#include <vector>
#include "Device.h"
#include "../utils/BufferUtils.h"
#include "../utils/Constants.h"

namespace moonshine {


    class Buffer {
    public:
        Buffer(
                Device &device,
                VkDeviceSize instanceSize,
                uint32_t instanceCount,
                VkBufferUsageFlags usageFlags,
                VkMemoryPropertyFlags memoryPropertyFlags,
                VkDeviceSize minOffsetAlignment = 1);

        ~Buffer();

        Buffer(const Buffer &) = delete;

        Buffer &operator=(const Buffer &) = delete;

        VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        void unmap();

        void writeToBuffer(void *data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        VkDescriptorBufferInfo descriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        void writeToIndex(void *data, int index);

        VkResult flushIndex(int index);

        VkDescriptorBufferInfo descriptorInfoForIndex(int index);

        VkResult invalidateIndex(int index);

        VkBuffer const getBuffer() const { return m_buffer; }

        void *getMappedMemory() const { return m_mapped; }

        uint32_t getInstanceCount() const { return instanceCount; }

        VkDeviceSize getInstanceSize() const { return instanceSize; }

        VkDeviceSize getAlignmentSize() const { return instanceSize; }

        VkBufferUsageFlags getUsageFlags() const { return usageFlags; }

        VkMemoryPropertyFlags getMemoryPropertyFlags() const { return memoryPropertyFlags; }

        VkDeviceSize getBufferSize() const { return bufferSize; }

    private:
        static VkDeviceSize getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

        Device &m_device;
        void *m_mapped = nullptr;
        VkBuffer m_buffer = VK_NULL_HANDLE;
        VkDeviceMemory m_memory = VK_NULL_HANDLE;

        VkDeviceSize bufferSize;
        uint32_t instanceCount;
        VkDeviceSize instanceSize;
        VkDeviceSize alignmentSize;
        VkBufferUsageFlags usageFlags;
        VkMemoryPropertyFlags memoryPropertyFlags;
    };


} // moonshine

#endif //MOONSHINE_BUFFER_H
