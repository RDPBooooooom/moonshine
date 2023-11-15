//
// Created by marvin on 28.07.2023.
//

#include <stdexcept>
#include "iostream"
#include <chrono>
#include <mutex>
#include "SimpleRenderSystem.h"
#include "../utils/Constants.h"
#include "../editor/Scene.h"

namespace moonshine {

    struct SimplePushConstantData {
        glm::mat4 modelMatrix{1.f};
        glm::mat4 tangentToWorld{1.f};
    };

    SimpleRenderSystem::SimpleRenderSystem(Device &device, VkRenderPass renderPass,
                                           VkDescriptorSetLayout globalSetLayout, VkDescriptorSetLayout materialSetLayout)
            : m_device{device} {
        createPipelineLayout(globalSetLayout, materialSetLayout);
        createPipeline(renderPass);
    }

    SimpleRenderSystem::~SimpleRenderSystem() {
        vkDestroyPipelineLayout(m_device.getVkDevice(), m_pipelineLayout, nullptr);
    }

    void SimpleRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout, VkDescriptorSetLayout materialSetLayout) {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        std::vector<VkDescriptorSetLayout> layouts = {globalSetLayout, materialSetLayout};
        
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = layouts.size();
        pipelineLayoutInfo.pSetLayouts = layouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
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
            FrameInfo &frmInfo, Scene &scene) {
        
        m_pipeline->bind(frmInfo.commandBuffer);

        std::vector<VkDescriptorSet> descriptorSets = {frmInfo.globalDescriptorSet};
        
        vkCmdBindDescriptorSets(frmInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                m_pipelineLayout, 0,
                                1, descriptorSets.data(), 0, nullptr);

        for (auto &obj: scene) {
            
            // Bind the correct material
            VkDescriptorSet materialDescriptorSet = frmInfo.materialDescriptorSets[obj->getMaterialIdx()];
            vkCmdBindDescriptorSets(frmInfo.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    m_pipelineLayout, 1,
                                    1, &materialDescriptorSet, 0, nullptr);


            SimplePushConstantData push{};
            push.modelMatrix = obj->getTransform()->getMatrix();
            push.tangentToWorld = glm::transpose(glm::inverse(obj->getTransform()->getMatrix()));

            vkCmdPushConstants(
                    frmInfo.commandBuffer,
                    m_pipelineLayout,
                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                    0,
                    sizeof(SimplePushConstantData),
                    &push);

            VkBuffer vertexBuffers[] = {obj->getVertBuffer()};
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(frmInfo.commandBuffer, 0, 1, vertexBuffers, offsets);

            vkCmdBindIndexBuffer(frmInfo.commandBuffer, obj->getIndexBuffer(), 0, VK_INDEX_TYPE_UINT16);
            
            vkCmdDrawIndexed(frmInfo.commandBuffer, static_cast<uint32_t>(obj->getIndexSize()), 1, 0, 0, 0);

        }
    }


} // moonshine