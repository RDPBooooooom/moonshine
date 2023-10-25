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
        std::string m_textureName;
        std::string m_pathToTexture;

        VkDescriptorSet m_imGui_DS;

        std::shared_ptr<TextureSampler> m_sampler;
        std::unique_ptr<TextureImage> m_texture;

    public:

        Material(std::string name, std::string textureName, std::string pathToTexture,
                 std::shared_ptr<TextureSampler> &sampler);

        VkSampler getVkSampler() {
            return m_sampler->getVkSampler();
        }

        VkImageView getVkImageView() {
            return m_texture->getImageView();
        }

        void loadTexture(Device &device);

        void drawGui();
    };

} // moonshine

#endif //MOONSHINE_MATERIAL_H
