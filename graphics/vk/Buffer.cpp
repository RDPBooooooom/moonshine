//
// Created by marvin on 15.06.2023.
//

#include "Buffer.h"

namespace moonshine {

/**
 * Returns the minimum instance size required to be compatible with devices minOffsetAlignment
 *
 * @param instance_size The size of an instance
 * @param min_offset_alignment The minimum required alignment, in bytes, for the offset member (eg
 * minUniformBufferOffsetAlignment)
 *
 * @return VkResult of the buffer mapping call
 */
    VkDeviceSize Buffer::get_alignment(VkDeviceSize instance_size, VkDeviceSize min_offset_alignment) {
        if (min_offset_alignment > 0) {
            return (instance_size + min_offset_alignment - 1) & ~(min_offset_alignment - 1);
        }
        return instance_size;
    }

    Buffer::Buffer(
            Device &device,
            VkDeviceSize instance_size,
            uint32_t instance_count,
            VkBufferUsageFlags usage_flags,
            VkMemoryPropertyFlags memory_property_flags,
            VkDeviceSize min_offset_alignment)
            : m_device{device},
              m_instance_size{instance_size},
              m_instance_count{instance_count},
              m_usage_flags{usage_flags},
              m_memory_property_flags{memory_property_flags} {
        m_alignment_size = get_alignment(instance_size, min_offset_alignment);
        m_buffer_size = m_alignment_size * instance_count;
        device.create_buffer(m_buffer_size, usage_flags, memory_property_flags, m_buffer, m_memory);
    }

    Buffer::~Buffer() {
        unmap();
        vkDestroyBuffer(m_device.get_vk_device(), m_buffer, nullptr);
        vkFreeMemory(m_device.get_vk_device(), m_memory, nullptr);
    }

/**
 * Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
 *
 * @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete
 * buffer range.
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkResult of the buffer mapping call
 */
    VkResult Buffer::map(VkDeviceSize size, VkDeviceSize offset) {
        assert(m_buffer && m_memory && "Called map on buffer before create");
        return vkMapMemory(m_device.get_vk_device(), m_memory, offset, size, 0, &m_mapped);
    }

/**
 * Unmap a mapped memory range
 *
 * @note Does not return a result as vkUnmapMemory can't fail
 */
    void Buffer::unmap() {
        if (m_mapped) {
            vkUnmapMemory(m_device.get_vk_device(), m_memory);
            m_mapped = nullptr;
        }
    }

/**
 * Copies the specified data to the mapped buffer. Default value writes whole buffer range
 *
 * @param data Pointer to the data to copy
 * @param size (Optional) Size of the data to copy. Pass VK_WHOLE_SIZE to flush the complete buffer
 * range.
 * @param offset (Optional) Byte offset from beginning of mapped region
 *
 */
    void Buffer::write_to_buffer(void *data, VkDeviceSize size, VkDeviceSize offset) {
        assert(m_mapped && "Cannot copy to unmapped buffer");

        if (size == VK_WHOLE_SIZE) {
            memcpy(m_mapped, data, m_buffer_size);
        } else {
            char *mem_offset = (char *) m_mapped;
            mem_offset += offset;
            memcpy(mem_offset, data, size);
        }
    }

/**
 * Flush a memory range of the buffer to make it visible to the device
 *
 * @note Only required for non-coherent memory
 *
 * @param size (Optional) Size of the memory range to flush. Pass VK_WHOLE_SIZE to flush the
 * complete buffer range.
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkResult of the flush call
 */
    VkResult Buffer::flush(VkDeviceSize size, VkDeviceSize offset) {
        VkMappedMemoryRange mapped_range = {};
        mapped_range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mapped_range.memory = m_memory;
        mapped_range.offset = offset;
        mapped_range.size = size;
        return vkFlushMappedMemoryRanges(m_device.get_vk_device(), 1, &mapped_range);
    }

/**
 * Invalidate a memory range of the buffer to make it visible to the host
 *
 * @note Only required for non-coherent memory
 *
 * @param size (Optional) Size of the memory range to invalidate. Pass VK_WHOLE_SIZE to invalidate
 * the complete buffer range.
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkResult of the invalidate call
 */
    VkResult Buffer::invalidate(VkDeviceSize size, VkDeviceSize offset) {
        VkMappedMemoryRange mapped_range = {};
        mapped_range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mapped_range.memory = m_memory;
        mapped_range.offset = offset;
        mapped_range.size = size;
        return vkInvalidateMappedMemoryRanges(m_device.get_vk_device(), 1, &mapped_range);
    }

/**
 * Create a buffer info descriptor
 *
 * @param size (Optional) Size of the memory range of the descriptor
 * @param offset (Optional) Byte offset from beginning
 *
 * @return VkDescriptorBufferInfo of specified offset and range
 */
    VkDescriptorBufferInfo Buffer::descriptor_info(VkDeviceSize size, VkDeviceSize offset) {
        return VkDescriptorBufferInfo{
                m_buffer,
                offset,
                size,
        };
    }

/**
 * Copies "instanceSize" bytes of data to the mapped buffer at an offset of index * alignmentSize
 *
 * @param data Pointer to the data to copy
 * @param index Used in offset calculation
 *
 */
    void Buffer::write_to_index(void *data, int index) {
        write_to_buffer(data, m_instance_size, index * m_alignment_size);
    }

/**
 *  Flush the memory range at index * alignmentSize of the buffer to make it visible to the device
 *
 * @param index Used in offset calculation
 *
 */
    VkResult Buffer::flush_index(int index) { return flush(m_alignment_size, index * m_alignment_size); }

/**
 * Create a buffer info descriptor
 *
 * @param index Specifies the region given by index * alignmentSize
 *
 * @return VkDescriptorBufferInfo for instance at index
 */
    VkDescriptorBufferInfo Buffer::descriptor_info_for_index(int index) {
        return descriptor_info(m_alignment_size, index * m_alignment_size);
    }

/**
 * Invalidate a memory range of the buffer to make it visible to the host
 *
 * @note Only required for non-coherent memory
 *
 * @param index Specifies the region to invalidate: index * alignmentSize
 *
 * @return VkResult of the invalidate call
 */
    VkResult Buffer::invalidate_index(int index) {
        return invalidate(m_alignment_size, index * m_alignment_size);
    }
} // moonshine