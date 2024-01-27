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
                VkDeviceSize instance_size,
                uint32_t instance_count,
                VkBufferUsageFlags usage_flags,
                VkMemoryPropertyFlags memory_property_flags,
                VkDeviceSize min_offset_alignment = 1);

        ~Buffer();

        Buffer(const Buffer &) = delete;

        Buffer &operator=(const Buffer &) = delete;

        VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        void unmap();

        void write_to_buffer(void *data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        VkDescriptorBufferInfo descriptor_info(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        void write_to_index(void *data, int index);

        VkResult flush_index(int index);

        VkDescriptorBufferInfo descriptor_info_for_index(int index);

        VkResult invalidate_index(int index);

        VkBuffer const get_buffer() const { return m_buffer; }

        void *get_mapped_memory() const { return m_mapped; }

        uint32_t get_instance_count() const { return m_instance_count; }

        VkDeviceSize get_instance_size() const { return m_instance_size; }

        VkDeviceSize get_alignment_size() const { return m_instance_size; }

        VkBufferUsageFlags get_usage_flags() const { return m_usage_flags; }

        VkMemoryPropertyFlags get_memory_property_flags() const { return m_memory_property_flags; }

        VkDeviceSize get_buffer_size() const { return m_buffer_size; }

    private:
        static VkDeviceSize get_alignment(VkDeviceSize instance_size, VkDeviceSize min_offset_alignment);

        Device &m_device;
        void *m_mapped = nullptr;
        VkBuffer m_buffer = VK_NULL_HANDLE;
        VkDeviceMemory m_memory = VK_NULL_HANDLE;

        VkDeviceSize m_buffer_size;
        uint32_t m_instance_count;
        VkDeviceSize m_instance_size;
        VkDeviceSize m_alignment_size;
        VkBufferUsageFlags m_usage_flags;
        VkMemoryPropertyFlags m_memory_property_flags;
    };


} // moonshine

#endif //MOONSHINE_BUFFER_H
