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

        std::vector<VkBuffer> m_uniformBuffers;
        std::vector<VkDeviceMemory> m_uniformBuffersMemory;
        std::vector<void *> m_uniformBuffersMapped;

    public:
        explicit UniformBuffer(Device &device) : m_device{device} {
            VkDeviceSize bufferSize = sizeof(T);

            m_uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
            m_uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
            m_uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

            for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
                m_device.createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                             m_uniformBuffers[i], m_uniformBuffersMemory[i]);

                vkMapMemory(m_device.getVkDevice(), m_uniformBuffersMemory[i], 0, bufferSize, 0,
                            &m_uniformBuffersMapped[i]);
            }
        }

        ~UniformBuffer() {
            for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
                vkDestroyBuffer(m_device.getVkDevice(), m_uniformBuffers[i], nullptr);
                vkFreeMemory(m_device.getVkDevice(), m_uniformBuffersMemory[i], nullptr);
            }
        }

        VkBuffer getUniformBuffer(size_t i) {
            return m_uniformBuffers[i];
        }

        void *getMappedUniformBuffer(size_t i) {
            return m_uniformBuffersMapped[i];
        }

        VkResult flush(size_t i, VkDeviceSize size, VkDeviceSize offset) {
            VkMappedMemoryRange mappedRange = {};
            mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
            mappedRange.memory = m_uniformBuffersMemory[i];
            mappedRange.offset = offset;
            mappedRange.size = size;
            return vkFlushMappedMemoryRanges(m_device.getVkDevice(), 1, &mappedRange);
        }
    };

}
#endif //MOONSHINE_UNIFORMBUFFER_H
