//
// Created by marvin on 29.09.2023.
//

#include "Material.h"
#include "../utils/FileUtils.h"

#include <utility>

namespace moonshine {

    Material::Material(std::string name, std::string textureName, std::string pathToTexture,
                       std::shared_ptr<TextureSampler> &sampler)
            : m_name{std::move(name)},
              m_textureName{
                      std::move(textureName)},
              m_pathToTexture{
                      std::move(pathToTexture)} {
        m_sampler = sampler;
    }

    void Material::loadTexture(Device &device) {
        m_texture = std::make_unique<TextureImage>(
                (m_pathToTexture + m_textureName).c_str(), &device,
                device.getCommandPool());
    }

} // moonshine