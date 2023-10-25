//
// Created by marvin on 29.09.2023.
//

#ifndef MOONSHINE_MATERIALMANAGER_H
#define MOONSHINE_MATERIALMANAGER_H

#include <memory>
#include "Descriptors.h"
#include "TextureSampler.h"
#include "Material.h"

namespace moonshine {

    class MaterialManager {

    private:

        Device *m_device;
        std::shared_ptr<DescriptorPool> materialPool{};
        std::shared_ptr<TextureSampler> m_sampler;

        std::unique_ptr<DescriptorSetLayout> m_materialLayout;
        std::vector<VkDescriptorSet> m_materialDescriptorSets{};
        std::vector<std::shared_ptr<Material>> m_materials;

    public:
        explicit MaterialManager(Device *device);

        VkDescriptorSetLayout getMaterialLayout() {
            return m_materialLayout->getDescriptorSetLayout();
        }

        std::vector<VkDescriptorSet> getDescriptorSet(){
            return m_materialDescriptorSets;
        }
        
        uint16_t createMaterial(std::string &name, std::string &textureName, std::string &pathToTexture);

        std::shared_ptr<Material> getMaterial(const uint16_t matIdx);
    };

} // moonshine

#endif //MOONSHINE_MATERIALMANAGER_H
