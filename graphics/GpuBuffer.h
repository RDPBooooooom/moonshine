//
// Created by marvin on 04.06.2023.
//

#ifndef MOONSHINE_GPUBUFFER_H
#define MOONSHINE_GPUBUFFER_H

#include "Device.h"
#include "../utils/BufferUtils.h"
#include "Buffer.h"

namespace moonshine {

    template<typename T>
    class GpuBuffer {

    private:
        Device &m_device;
        std::vector<T> m_bufferData;

        std::unique_ptr<Buffer> m_buffer;

    public:
        GpuBuffer(std::vector<T> &buffer, Device &device, VkBufferUsageFlagBits vkBufferUsageFlag) :
                m_device{device},
                m_bufferData{buffer} {

            VkDeviceSize bufferSize = sizeof(buffer[0]) * buffer.size();
            uint32_t size = sizeof(buffer[0]);

            Buffer stagingBuffer{
                    reinterpret_cast<Device &>(m_device),
                    buffer.size(),
                    size,
                    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
            };

            stagingBuffer.map();
            stagingBuffer.writeToBuffer((void *) m_bufferData.data());

            m_buffer = std::make_unique<Buffer>(m_device,
                                                buffer.size(),
                                                size,
                                                VK_BUFFER_USAGE_TRANSFER_DST_BIT | vkBufferUsageFlag,
                                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

            m_device.copyBuffer(stagingBuffer.getBuffer(), m_buffer->getBuffer(), bufferSize);
        };

        VkBuffer getBuffer() { return m_buffer->getBuffer(); }

        ~GpuBuffer() {

        };
    };

} // moonshine

#endif //MOONSHINE_GPUBUFFER_H
