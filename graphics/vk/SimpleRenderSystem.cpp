//
// Created by marvin on 28.07.2023.
//

#include <winsock2.h>
#include <stdexcept>
#include "SimpleRenderSystem.h"
#include "../utils/Constants.h"
#include "../editor/EngineSystems.h"

namespace moonshine {

    struct SimplePushConstantData {
        glm::mat4 modelMatrix{1.f};
        glm::mat4 tangentToWorld{1.f};
    };

    SimpleRenderSystem::SimpleRenderSystem(Device &device, VkRenderPass render_pass,
                                           VkDescriptorSetLayout global_set_layout,
                                           VkDescriptorSetLayout material_set_layout)
            : m_device{device} {
        create_pipeline_layout(global_set_layout, material_set_layout);
        create_pipeline(render_pass);
    }

    SimpleRenderSystem::~SimpleRenderSystem() {
        vkDestroyPipelineLayout(m_device.get_vk_device(), m_pipeline_layout, nullptr);
    }

    void SimpleRenderSystem::create_pipeline_layout(VkDescriptorSetLayout global_set_layout,
                                                    VkDescriptorSetLayout material_set_layout) {
        VkPushConstantRange push_constant_range{};
        push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        push_constant_range.offset = 0;
        push_constant_range.size = sizeof(SimplePushConstantData);

        std::vector<VkDescriptorSetLayout> layouts = {global_set_layout, material_set_layout};

        VkPipelineLayoutCreateInfo pipeline_layout_info{};
        pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipeline_layout_info.setLayoutCount = layouts.size();
        pipeline_layout_info.pSetLayouts = layouts.data();
        pipeline_layout_info.pushConstantRangeCount = 1;
        pipeline_layout_info.pPushConstantRanges = &push_constant_range;
        if (vkCreatePipelineLayout(m_device.get_vk_device(), &pipeline_layout_info, nullptr, &m_pipeline_layout) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void SimpleRenderSystem::create_pipeline(VkRenderPass pT) {
        assert(m_pipeline_layout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipeline_config{};
        Pipeline::default_pipeline_config_info(pipeline_config);

        pipeline_config.renderPass = pT;
        pipeline_config.pipelineLayout = m_pipeline_layout;

        m_pipeline = std::make_unique<Pipeline>(
                m_device,
                "resources/shaders/shader.vert.spv",
                "resources/shaders/shader.frag.spv",
                pipeline_config);
    }

    void SimpleRenderSystem::render_game_objects(
            FrameInfo &frm_info, Scene &scene) {

        m_pipeline->bind(frm_info.commandBuffer);

        std::vector<VkDescriptorSet> descriptor_sets = {frm_info.globalDescriptorSet};

        vkCmdBindDescriptorSets(frm_info.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                m_pipeline_layout, 0,
                                1, descriptor_sets.data(), 0, nullptr);

        for (auto &obj: scene) {

            for (auto &node: obj->get_nodes()) {
                for (auto &submesh: node->get_sub_meshes()) {
                    // Bind the correct material
                    VkDescriptorSet material_descriptor_set = frm_info.materialDescriptorSets[submesh.m_materialIdx];
                    vkCmdBindDescriptorSets(frm_info.commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                            m_pipeline_layout, 1,
                                            1, &material_descriptor_set, 0, nullptr);


                    SimplePushConstantData push{};
                    if (node->has_parent()) {
                        glm::mat4 model_mat = node->get_submesh_matrix();
                        push.modelMatrix = model_mat;
                        push.tangentToWorld = glm::transpose(glm::inverse(model_mat));
                    } else {
                        push.modelMatrix = obj->get_transform()->get_model_matrix();
                        push.tangentToWorld = glm::transpose(glm::inverse(obj->get_transform()->get_model_matrix()));
                    }

                    vkCmdPushConstants(
                            frm_info.commandBuffer,
                            m_pipeline_layout,
                            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                            0,
                            sizeof(SimplePushConstantData),
                            &push);

                    VkBuffer vertex_buffers[] = {submesh.m_vertexBuffer->get_buffer()};
                    VkDeviceSize offsets[] = {0};
                    vkCmdBindVertexBuffers(frm_info.commandBuffer, 0, 1, vertex_buffers, offsets);

                    vkCmdBindIndexBuffer(frm_info.commandBuffer, submesh.m_indexBuffer->get_buffer(), 0,
                                         VK_INDEX_TYPE_UINT16);

                    vkCmdDrawIndexed(frm_info.commandBuffer, static_cast<uint32_t>(submesh.m_indices.size()), 1, 0, 0,
                                     0);

                    EngineSystems::get_instance().get_statistics()->increment_draw_call();
                    EngineSystems::get_instance().get_statistics()->add_vertex_count(submesh.m_vertices.size());
                }
            }
        }
    }
} // moonshine