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
        std::vector<T> m_buffer_data;

        std::unique_ptr<Buffer> m_buffer;

    public:
        GpuBuffer(std::vector<T> &buffer, Device &device, VkBufferUsageFlagBits vk_buffer_usage_flag) :
                m_device{device},
                m_buffer_data{buffer} {

            VkDeviceSize buffer_size = sizeof(buffer[0]) * buffer.size();
            uint32_t size = sizeof(buffer[0]);

            Buffer staging_buffer{
                    reinterpret_cast<Device &>(m_device),
                    buffer.size(),
                    size,
                    VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
            };

            staging_buffer.map();
            staging_buffer.write_to_buffer((void *) m_buffer_data.data());

            m_buffer = std::make_unique<Buffer>(m_device,
                                                buffer.size(),
                                                size,
                                                VK_BUFFER_USAGE_TRANSFER_DST_BIT | vk_buffer_usage_flag,
                                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

            m_device.copy_buffer(staging_buffer.get_buffer(), m_buffer->get_buffer(), buffer_size);
        };

        VkBuffer get_buffer() { return m_buffer->get_buffer(); }

        ~GpuBuffer() {

        };
    };

} // moonshine

#endif //MOONSHINE_GPUBUFFER_H
