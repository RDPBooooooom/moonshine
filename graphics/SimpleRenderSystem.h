//
// Created by marvin on 28.07.2023.
//

#ifndef MOONSHINE_SIMPLERENDERSYSTEM_H
#define MOONSHINE_SIMPLERENDERSYSTEM_H

#include <memory>
#include "Device.h"
#include "Pipeline.h"
#include "../editor/SceneObject.h"
#include "TextureImage.h"
#include "TextureSampler.h"
#include "Buffer.h"
#include "FrameInfo.h"
#include "../editor/Scene.h"

namespace moonshine {

    class SimpleRenderSystem {

    private:

        Device &m_device;

        VkPipelineLayout m_pipeline_layout;
        std::unique_ptr<Pipeline> m_pipeline;

    public:
        SimpleRenderSystem(Device &device, VkRenderPass render_pass, VkDescriptorSetLayout global_set_layout,
                           VkDescriptorSetLayout material_set_layout);

        ~SimpleRenderSystem();

        SimpleRenderSystem(const SimpleRenderSystem &) = delete;

        SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

        void
        render_game_objects(FrameInfo &frm_info, Scene &scene);

    private:

        void create_pipeline(VkRenderPass pT);

        void create_pipeline_layout(VkDescriptorSetLayout global_set_layout, VkDescriptorSetLayout material_set_layout);
    };

} // moonshine

#endif //MOONSHINE_SIMPLERENDERSYSTEM_H
