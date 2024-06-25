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

    Material::Material(std::string name, std::string texture_name, std::string path_to_texture,
                       std::shared_ptr<TextureSampler> &sampler)
            : m_name{std::move(name)},
              m_texture_name{
                      std::move(texture_name)},
              m_path_to_texture{
                      std::move(path_to_texture)} {
        m_sampler = sampler;
    }

    void Material::load_texture(Device &device) {
        m_texture = std::make_unique<TextureImage>(
                (m_path_to_texture + m_texture_name).c_str(), &device,
                device.get_command_pool());
        
        // Create Descriptor Set using ImGUI's implementation
        m_imGui_DS = ImGui_ImplVulkan_AddTexture(m_sampler->get_vk_sampler(), m_texture->get_image_view(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    }
    
    void Material::draw_gui(){
        ImGui::SeparatorText("Material");
        ImGui::BeginDisabled();
        ImGui::Text(m_name.c_str());
        ImGui::Image(ImTextureID(m_imGui_DS), ImVec2(100, 100));
        ImGui::EndDisabled();
    }

} // moonshine