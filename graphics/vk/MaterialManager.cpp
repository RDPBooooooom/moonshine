//
// Created by marvin on 29.09.2023.
//

#include "MaterialManager.h"
#include "../utils/Constants.h"

namespace moonshine {
    
    MaterialManager::MaterialManager(Device &device) : m_device{device}{
        m_material_pool = DescriptorPool::Builder(m_device).set_max_sets(MAX_MATERIALS)
                .add_pool_size(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1)
                .build();

        m_sampler = std::make_shared<TextureSampler>(m_device);

        m_material_layout = DescriptorSetLayout::Builder(m_device)
                .add_binding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
                .build();
        
    }

    uint16_t MaterialManager::create_material(std::string &name, std::string &texture_name, std::string &path_to_texture) {
        
        std::shared_ptr<Material> material = std::make_shared<Material>(name, texture_name, path_to_texture, m_sampler);
        material->load_texture(m_device);
        m_materials.push_back(material);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = material->get_vk_image_view();
        imageInfo.sampler = material->get_vk_sampler();
        
        uint16_t size = m_material_descriptor_sets.size();
        m_material_descriptor_sets.resize(size + 1);

        DescriptorWriter(*m_material_layout, *m_material_pool)
                .write_image(0, &imageInfo)
                .build(m_material_descriptor_sets[size]);
        
        return size;
    }
    
    std::shared_ptr<Material> MaterialManager::get_material(const uint16_t mat_idx) {
        //TODO: Validation
        return m_materials.at((size_t) mat_idx);
    }

    void MaterialManager::clean_up() {
        m_materials.clear();
        m_material_layout = nullptr;
        m_material_descriptor_sets.clear();
        m_material_pool = nullptr;
        m_sampler = nullptr;
        
    }
} // moonshine