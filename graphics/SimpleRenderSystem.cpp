//
// Created by marvin on 28.07.2023.
//

#include <stdexcept>
#include "iostream"
#include <chrono>
#include "SimpleRenderSystem.h"
#include "../utils/Constants.h"

namespace moonshine {


    SimpleRenderSystem::SimpleRenderSystem(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
            : m_device{device} {
        createPipelineLayout(globalSetLayout);
        createPipeline(renderPass);
    }

    SimpleRenderSystem::~SimpleRenderSystem() {
        vkDestroyPipelineLayout(m_device.getVkDevice(), m_pipelineLayout, nullptr);
    }

    void SimpleRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &globalSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        if (vkCreatePipelineLayout(m_device.getVkDevice(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void SimpleRenderSystem::createPipeline(VkRenderPass renderPass) {
        assert(m_pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");
        
        std::cout << "Creating pipeline \n";

        PipelineConfigInfo pipelineConfig{};
        Pipeline::defaultPipelineConfigInfo(pipelineConfig);
        
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = m_pipelineLayout;
        
        m_pipeline = std::make_unique<Pipeline>(
                m_device,
                "resources/shaders/shader.vert.spv",
                "resources/shaders/shader.frag.spv",
                pipelineConfig);
    }

    void SimpleRenderSystem::renderGameObjects(
            FrameInfo& frmInfo, std::vector<std::shared_ptr<SceneObject>> gameObjects) {
        m_pipeline->bind(frmInfo.commandBuffer);

        vkCmdBindDescriptorSets(frmInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                m_pipelineLayout, 0,
                                1, &frmInfo.globalDescriptorSet, 0, nullptr);

        for (auto &obj: gameObjects) {

            //obj.model->bind(commandBuffer);
            //obj.model->draw(commandBuffer);

            VkBuffer vertexBuffers[] = {obj->getVertBuffer()};
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(frmInfo.commandBuffer, 0, 1, vertexBuffers, offsets);

            vkCmdBindIndexBuffer(frmInfo.commandBuffer, obj->getIndexBuffer(), 0, VK_INDEX_TYPE_UINT16);

            
            vkCmdDrawIndexed(frmInfo.commandBuffer, static_cast<uint32_t>(obj->getIndexSize()), 1, 0, 0, 0);

        }
    }

    

} // moonshine