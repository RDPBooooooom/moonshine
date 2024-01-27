//
// Created by marvin on 30.07.2023.
//

#ifndef MOONSHINE_UNIFORMBUFFER_H
#define MOONSHINE_UNIFORMBUFFER_H

#include "Device.h"
#include "../utils/Constants.h"
#include "../utils/BufferUtils.h"

namespace moonshine {
    template<typename T>
    class UniformBuffer {

    private:
        Device &m_device;

        std::vector<VkBuffer> m_uniform_buffers;
        std::vector<VkDeviceMemory> m_uniform_buffers_memory;
        std::vector<void *> m_uniform_buffers_mapped;

    public:
        explicit UniformBuffer(Device &device) : m_device{device} {
            VkDeviceSize buffer_size = sizeof(T);

            m_uniform_buffers.resize(MAX_FRAMES_IN_FLIGHT);
            m_uniform_buffers_memory.resize(MAX_FRAMES_IN_FLIGHT);
            m_uniform_buffers_mapped.resize(MAX_FRAMES_IN_FLIGHT);

            for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
                m_device.create_buffer(buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                                       m_uniform_buffers[i], m_uniform_buffers_memory[i]);

                vkMapMemory(m_device.get_vk_device(), m_uniform_buffers_memory[i], 0, buffer_size, 0,
                            &m_uniform_buffers_mapped[i]);
            }
        }

        ~UniformBuffer() {
            for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
                vkDestroyBuffer(m_device.get_vk_device(), m_uniform_buffers[i], nullptr);
                vkFreeMemory(m_device.get_vk_device(), m_uniform_buffers_memory[i], nullptr);
            }
        }

        VkBuffer get_uniform_buffer(size_t i) {
            return m_uniform_buffers[i];
        }

        void *get_mapped_uniform_buffer(size_t i) {
            return m_uniform_buffers_mapped[i];
        }

        VkResult flush(size_t i, VkDeviceSize size, VkDeviceSize offset) {
            VkMappedMemoryRange mapped_range = {};
            mapped_range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
            mapped_range.memory = m_uniform_buffers_memory[i];
            mapped_range.offset = offset;
            mapped_range.size = size;
            return vkFlushMappedMemoryRanges(m_device.get_vk_device(), 1, &mapped_range);
        }
    };

}
#endif //MOONSHINE_UNIFORMBUFFER_H
