//
// Created by marvin on 29.09.2023.
//

#include "Material.h"
#include "../utils/FileUtils.h"
#include "imgui.h"
#include "../external/imgui/misc/cpp/imgui_stdlib.h"
#include "imgui_impl_vulkan.h"

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
        
        // Create Descriptor Set using ImGUI's implementation
        m_imGui_DS = ImGui_ImplVulkan_AddTexture(m_sampler->getVkSampler(), m_texture->getImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    }
    
    void Material::drawGui(){
        ImGui::Text("Material");
        ImGui::BeginDisabled();
        ImGui::Text(m_name.c_str());
        ImGui::Image(ImTextureID(m_imGui_DS), ImVec2(100, 100));
        ImGui::EndDisabled();
    }

} // moonshine