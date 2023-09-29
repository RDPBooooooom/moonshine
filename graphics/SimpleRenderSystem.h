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

namespace moonshine {

    class SimpleRenderSystem {

    private:

        Device &m_device;

        VkPipelineLayout m_pipelineLayout;
        std::unique_ptr<Pipeline> m_pipeline;

    public:
        SimpleRenderSystem(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout, VkDescriptorSetLayout materialSetLayout);

        ~SimpleRenderSystem();

        SimpleRenderSystem(const SimpleRenderSystem &) = delete;

        SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

        void
        renderGameObjects(FrameInfo &frmInfo, std::vector<std::shared_ptr<SceneObject>> gameObjects,
                          std::mutex *toLock);

    private:

        void createPipeline(VkRenderPass pT);

        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout, VkDescriptorSetLayout materialSetLayout);
    };

} // moonshine

#endif //MOONSHINE_SIMPLERENDERSYSTEM_H
