//
// Created by marvin on 29.09.2023.
//

#ifndef MOONSHINE_MATERIAL_H
#define MOONSHINE_MATERIAL_H

#include <memory>
#include "TextureImage.h"
#include "TextureSampler.h"

namespace moonshine {

    class Material {

    private:
        std::string m_name;
        std::string m_texture_name;
        std::string m_path_to_texture;

        VkDescriptorSet m_imGui_DS;

        std::shared_ptr<TextureSampler> m_sampler;
        std::unique_ptr<TextureImage> m_texture;

    public:

        Material(std::string name, std::string texture_name, std::string path_to_texture,
                 std::shared_ptr<TextureSampler> &sampler);

        VkSampler get_vk_sampler() {
            return m_sampler->get_vk_sampler();
        }

        VkImageView get_vk_image_view() {
            return m_texture->get_image_view();
        }

        void load_texture(Device &device);

        void draw_gui();
    };

} // moonshine

#endif //MOONSHINE_MATERIAL_H
