//
// Created by marvin on 28.07.2023.
//

#ifndef MOONSHINE_DESCRIPTORS_H
#define MOONSHINE_DESCRIPTORS_H

#include <memory>
#include <unordered_map>
#include <vector>
#include "Device.h"

namespace moonshine {

    class DescriptorSetLayout {
    public:
        class Builder {
        public:
            Builder(Device &device) : m_device{device} {}

            Builder &add_binding(
                    uint32_t binding,
                    VkDescriptorType descriptor_type,
                    VkShaderStageFlags stage_flags,
                    uint32_t count = 1);

            std::unique_ptr<DescriptorSetLayout> build() const;

        private:
            Device &m_device;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_bindings{};
        };

        DescriptorSetLayout(
                Device
                &device,
                std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings
        );

        ~DescriptorSetLayout();

        DescriptorSetLayout(const DescriptorSetLayout &) = delete;

        DescriptorSetLayout &operator=(const DescriptorSetLayout &) = delete;

        VkDescriptorSetLayout get_descriptor_set_layout() const { return m_descriptorSetLayout; }

    private:
        Device &m_device;
        VkDescriptorSetLayout m_descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_bindings;

        friend class DescriptorWriter;
    };

    class DescriptorPool {
    public:
        class Builder {
        public:
            Builder(Device &device) : m_device{device} {}

            Builder &add_pool_size(VkDescriptorType descriptor_type, uint32_t count);

            Builder &set_pool_flags(VkDescriptorPoolCreateFlags flags);

            Builder &set_max_sets(uint32_t count);

            std::unique_ptr<DescriptorPool> build() const;

        private:
            Device &m_device;
            std::vector<VkDescriptorPoolSize> m_pool_sizes{};
            uint32_t m_max_sets = 1000;
            VkDescriptorPoolCreateFlags m_pool_flags = 0;
        };

        DescriptorPool(
                Device &device,
                uint32_t max_sets,
                VkDescriptorPoolCreateFlags pool_flags,
                const std::vector<VkDescriptorPoolSize> &pool_sizes);

        ~DescriptorPool();

        DescriptorPool(const DescriptorPool &) = delete;

        DescriptorPool &operator=(const DescriptorPool &) = delete;

        bool allocate_descriptor(
                const VkDescriptorSetLayout descriptor_set_layout, VkDescriptorSet &descriptor) const;

        void free_descriptors(std::vector<VkDescriptorSet> &descriptors) const;

        void reset_pool();

        VkDescriptorPool get_vk_discriptor_pool() { return m_descriptor_pool; }

    private:
        Device &m_device;
        VkDescriptorPool m_descriptor_pool;

        friend class DescriptorWriter;
    };

    class DescriptorWriter {
    public:
        DescriptorWriter(DescriptorSetLayout &set_layout, DescriptorPool &pool);

        DescriptorWriter &write_buffer(uint32_t binding, VkDescriptorBufferInfo *buffer_info);

        DescriptorWriter &write_image(uint32_t binding, VkDescriptorImageInfo *image_info);

        bool build(VkDescriptorSet &set);

        void overwrite(VkDescriptorSet &set);

    private:
        DescriptorSetLayout &m_set_layout;
        DescriptorPool &m_pool;
        std::vector<VkWriteDescriptorSet> m_writes;
    };


} // moonshine

#endif //MOONSHINE_DESCRIPTORS_H
