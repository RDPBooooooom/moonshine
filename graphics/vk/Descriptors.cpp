//
// Created by marvin on 28.07.2023.
//

#include "Descriptors.h"
#include <cassert>
#include <stdexcept>

namespace moonshine {

    // *************** Descriptor Set Layout Builder *********************

    DescriptorSetLayout::Builder &DescriptorSetLayout::Builder::add_binding(
            uint32_t binding,
            VkDescriptorType descriptor_type,
            VkShaderStageFlags stage_flags,
            uint32_t count) {
        assert(m_bindings.count(binding) == 0 && "Binding already in use");
        VkDescriptorSetLayoutBinding layout_binding{};
        layout_binding.binding = binding;
        layout_binding.descriptorType = descriptor_type;
        layout_binding.descriptorCount = count;
        layout_binding.stageFlags = stage_flags;
        m_bindings[binding] = layout_binding;
        return *this;
    }

    std::unique_ptr<DescriptorSetLayout> DescriptorSetLayout::Builder::build() const {
        return std::make_unique<DescriptorSetLayout>(m_device, m_bindings);
    }

// *************** Descriptor Set Layout *********************

    DescriptorSetLayout::DescriptorSetLayout(
            Device &device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
            : m_device{device}, m_bindings{bindings} {
        std::vector<VkDescriptorSetLayoutBinding> set_layout_bindings{};
        for (auto kv: bindings) {
            set_layout_bindings.push_back(kv.second);
        }

        VkDescriptorSetLayoutCreateInfo descriptor_set_layout_info{};
        descriptor_set_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptor_set_layout_info.bindingCount = static_cast<uint32_t>(set_layout_bindings.size());
        descriptor_set_layout_info.pBindings = set_layout_bindings.data();

        if (vkCreateDescriptorSetLayout(
                m_device.get_vk_device(),
                &descriptor_set_layout_info,
                nullptr,
                &m_descriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    DescriptorSetLayout::~DescriptorSetLayout() {
        vkDestroyDescriptorSetLayout(m_device.get_vk_device(), m_descriptorSetLayout, nullptr);
    }

// *************** Descriptor Pool Builder *********************

    DescriptorPool::Builder &DescriptorPool::Builder::add_pool_size(
            VkDescriptorType descriptor_type, uint32_t count) {
        m_pool_sizes.push_back({descriptor_type, count});
        return *this;
    }

    DescriptorPool::Builder &DescriptorPool::Builder::set_pool_flags(
            VkDescriptorPoolCreateFlags flags) {
        m_pool_flags = flags;
        return *this;
    }

    DescriptorPool::Builder &DescriptorPool::Builder::set_max_sets(uint32_t count) {
        m_max_sets = count;
        return *this;
    }

    std::unique_ptr<DescriptorPool> DescriptorPool::Builder::build() const {
        return std::make_unique<DescriptorPool>(m_device, m_max_sets, m_pool_flags, m_pool_sizes);
    }

// *************** Descriptor Pool *********************

    DescriptorPool::DescriptorPool(
            Device &device,
            uint32_t max_sets,
            VkDescriptorPoolCreateFlags pool_flags,
            const std::vector<VkDescriptorPoolSize> &pool_sizes)
            : m_device{device} {
        VkDescriptorPoolCreateInfo descriptor_pool_info{};
        descriptor_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptor_pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
        descriptor_pool_info.pPoolSizes = pool_sizes.data();
        descriptor_pool_info.maxSets = max_sets;
        descriptor_pool_info.flags = pool_flags;

        if (vkCreateDescriptorPool(m_device.get_vk_device(), &descriptor_pool_info, nullptr, &m_descriptor_pool) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    DescriptorPool::~DescriptorPool() {
        vkDestroyDescriptorPool(m_device.get_vk_device(), m_descriptor_pool, nullptr);
    }

    bool DescriptorPool::allocate_descriptor(
            const VkDescriptorSetLayout descriptor_set_layout, VkDescriptorSet &descriptor) const {
        VkDescriptorSetAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.descriptorPool = m_descriptor_pool;
        alloc_info.pSetLayouts = &descriptor_set_layout;
        alloc_info.descriptorSetCount = 1;

        // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
        // a new pool whenever an old pool fills up. But this is beyond our current scope
        if (vkAllocateDescriptorSets(m_device.get_vk_device(), &alloc_info, &descriptor) != VK_SUCCESS) {
            return false;
        }
        return true;
    }

    void DescriptorPool::free_descriptors(std::vector<VkDescriptorSet> &descriptors) const {
        vkFreeDescriptorSets(
                m_device.get_vk_device(),
                m_descriptor_pool,
                static_cast<uint32_t>(descriptors.size()),
                descriptors.data());
    }

    void DescriptorPool::reset_pool() {
        vkResetDescriptorPool(m_device.get_vk_device(), m_descriptor_pool, 0);
    }

// *************** Descriptor Writer *********************

    DescriptorWriter::DescriptorWriter(DescriptorSetLayout &set_layout, DescriptorPool &pool)
            : m_set_layout{set_layout}, m_pool{pool} {}

    DescriptorWriter &DescriptorWriter::write_buffer(
            uint32_t binding, VkDescriptorBufferInfo *buffer_info) {
        assert(m_set_layout.m_bindings.count(binding) == 1 && "Layout does not contain specified binding");

        auto &binding_description = m_set_layout.m_bindings[binding];

        assert(
                binding_description.descriptorCount == 1 &&
                "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = binding_description.descriptorType;
        write.dstBinding = binding;
        write.pBufferInfo = buffer_info;
        write.descriptorCount = 1;

        m_writes.push_back(write);
        return *this;
    }

    DescriptorWriter &DescriptorWriter::write_image(
            uint32_t binding, VkDescriptorImageInfo *image_info) {
        assert(m_set_layout.m_bindings.count(binding) == 1 && "Layout does not contain specified binding");

        auto &binding_description = m_set_layout.m_bindings[binding];

        assert(
                binding_description.descriptorCount == 1 &&
                "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = binding_description.descriptorType;
        write.dstBinding = binding;
        write.pImageInfo = image_info;
        write.descriptorCount = 1;

        m_writes.push_back(write);
        return *this;
    }

    bool DescriptorWriter::build(VkDescriptorSet &set) {
        bool success = m_pool.allocate_descriptor(m_set_layout.get_descriptor_set_layout(), set);
        if (!success) {
            return false;
        }
        overwrite(set);
        return true;
    }

    void DescriptorWriter::overwrite(VkDescriptorSet &set) {
        for (auto &write: m_writes) {
            write.dstSet = set;
        }
        vkUpdateDescriptorSets(m_pool.m_device.get_vk_device(), m_writes.size(), m_writes.data(), 0, nullptr);
    }
} // moonshine