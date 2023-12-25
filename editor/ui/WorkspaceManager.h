//
// Created by marvin on 16.11.2023.
//

#ifndef MOONSHINE_WORKSPACEMANAGER_H
#define MOONSHINE_WORKSPACEMANAGER_H

#include <string>
#include <memory>
#include "../InputHandler.h"
#include "../../graphics/Device.h"
#include "../../graphics/MaterialManager.h"
#include "../Transform.h"
#include "../Camera.h"
#include "imgui_impl_vulkan.h"
#include <boost/uuid/uuid.hpp>
#include <boost/json/array.hpp>

namespace moonshine {

    class WorkspaceManager {

    private:
        struct import_data {
            std::string path = "";
            std::string file = "";
            std::string name = "";
            boost::uuids::uuid uuid;

            bool overwrite_pos = false;
            bool overwrite_rot = false;
            bool overwrite_scale = false;
            Transform transform = {};
        };

        struct workspace_object {
            std::string path = "";
            std::string file = "";
            std::string name = "";

            bool has_thumbnail = false;
            VkDescriptorSet m_imGui_thumbnail_DS;
            std::unique_ptr<TextureImage> m_thumbnail_image;
        };

    private:
        Device &m_device;
        Camera &m_camera;

        std::shared_ptr<InputHandler> m_inputHandler;
        std::shared_ptr<MaterialManager> m_materialManager;

        std::vector<boost::uuids::uuid> m_generated_ids;

        std::string m_workspacePath;

        std::mutex m_available_imports_mtx;
        std::vector<std::shared_ptr<workspace_object>> m_available_imports;

        std::unique_ptr<TextureSampler> m_previewSampler;
        VkDescriptorSet m_imGui_placeHolder_DS;
        std::unique_ptr<TextureImage> m_placeholder_image;

        std::mutex m_unique_temp_folder;
        std::mutex m_unique_workspace_folder;


    public:
        const std::string &get_workspace_path() const;

    private:
        bool m_workspaceModalActive = false;


        void drawInitModal();

        void draw_workspace_items();

        void import_object_gltf(import_data data);

        void load_workspace_scene();

        void handle_load_scene(const boost::json::array &objects);

        void import_object(import_data &data);

        void discover_workspace();

        std::shared_ptr<workspace_object> load_workspace_object(std::string path, std::string file, std::string name);

        void handle_workspace_import(std::string &path);

        void validate_path(const std::string &root_path, std::string &path, const std::string &name, int iteration = 0);

        void remove_workspace_object(std::shared_ptr<workspace_object> &toDelete);

    public:

        explicit WorkspaceManager(Device &device, std::shared_ptr<MaterialManager> &materialManager,
                                  std::shared_ptr<InputHandler> &inputHandler, Camera &camera) : m_inputHandler{
                inputHandler},
                                                                                                 m_device{device},
                                                                                                 m_materialManager{
                                                                                                         materialManager},
                                                                                                 m_camera{camera} {
            m_placeholder_image = std::make_unique<TextureImage>(
                    "resources\\editor\\SceneObject_placeholder.png", &device,
                    device.getCommandPool());
            m_previewSampler = std::make_unique<TextureSampler>(m_device);
            m_imGui_placeHolder_DS = ImGui_ImplVulkan_AddTexture(m_previewSampler->getVkSampler(),
                                                                 m_placeholder_image->getImageView(),
                                                                 VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }

        void draw();

        void import_object(std::string path, std::string file, Transform transform);

        void import_object(std::string path, std::string file, boost::uuids::uuid uuid, Transform transform = {});

        void save_scene();

        void clean_up();

    };

} // moonshine

#endif //MOONSHINE_WORKSPACEMANAGER_H
