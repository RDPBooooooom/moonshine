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

        Device& m_device;
        std::shared_ptr<DescriptorPool> m_material_pool{};
        std::shared_ptr<TextureSampler> m_sampler;

        std::unique_ptr<DescriptorSetLayout> m_material_layout;
        std::vector<VkDescriptorSet> m_material_descriptor_sets{};
        std::vector<std::shared_ptr<Material>> m_materials;

    public:
        explicit MaterialManager(Device &device);

        VkDescriptorSetLayout get_material_layout() {
            return m_material_layout->get_descriptor_set_layout();
        }

        std::vector<VkDescriptorSet> get_descriptor_set(){
            return m_material_descriptor_sets;
        }
        
        uint16_t create_material(std::string &name, std::string &texture_name, std::string &path_to_texture);

        std::shared_ptr<Material> get_material(const uint16_t mat_idx);
        
        void clean_up();
    };

} // moonshine

#endif //MOONSHINE_MATERIALMANAGER_H
