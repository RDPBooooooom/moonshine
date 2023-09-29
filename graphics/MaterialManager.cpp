//
// Created by marvin on 29.09.2023.
//

#include "MaterialManager.h"
#include "../utils/Constants.h"

namespace moonshine {
    
    MaterialManager::MaterialManager(Device *device) : m_device{device}{
        materialPool = DescriptorPool::Builder(*m_device).setMaxSets(MAX_MATERIALS)
                .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1)
                .build();

        m_sampler = std::make_shared<TextureSampler>(m_device);

        m_materialLayout = DescriptorSetLayout::Builder(*m_device)
                .addBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
                .build();
        
    }

    uint16_t MaterialManager::createMaterial(std::string &name, std::string &textureName, std::string &pathToTexture) {
        
        std::shared_ptr<Material> material = std::make_shared<Material>(name, textureName, pathToTexture, m_sampler);
        material->loadTexture(*m_device);
        m_materials.push_back(material);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = material->getVkImageView();
        imageInfo.sampler = material->getVkSampler();
        
        uint16_t size = m_materialDescriptorSets.size();
        m_materialDescriptorSets.resize(size + 1);
        
        DescriptorWriter(*m_materialLayout, *materialPool)
                .writeImage(0, &imageInfo)
                .build(m_materialDescriptorSets[size]);
        
        return size;
    }
} // moonshine