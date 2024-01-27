//
// Created by marvin on 01.06.2023.
//

#include <stdexcept>
#include <limits>
#include <functional>
#include "Pipeline.h"
#include "../utils/FileUtils.h"
#include "iostream"
#include "../editor/EngineSystems.h"

namespace moonshine {

    Pipeline::Pipeline(Device &device, const std::string vert_filepath, const std::string frag_filepath,
                       const PipelineConfigInfo &config_info) : m_device{&device} {

        create_graphics_pipeline(vert_filepath, frag_filepath, config_info);
    }

    Pipeline::~Pipeline() {
        vkDestroyShaderModule(m_device->get_vk_device(), m_vert_shader_module, nullptr);
        vkDestroyShaderModule(m_device->get_vk_device(), m_frag_shader_module, nullptr);
        vkDestroyPipeline(m_device->get_vk_device(), m_vk_graphics_pipeline, nullptr);
    }

    void Pipeline::bind(VkCommandBuffer command_buffer) {
        vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_vk_graphics_pipeline);
    }

    void Pipeline::create_graphics_pipeline(const std::string &vert_filepath, const std::string &frag_filepath,
                                            const PipelineConfigInfo &config_info) {
        assert(
                config_info.pipelineLayout != VK_NULL_HANDLE &&
                "Cannot create graphics pipeline: no pipelineLayout provided in configInfo");
        assert(
                config_info.renderPass != VK_NULL_HANDLE &&
                "Cannot create graphics pipeline: no renderPass provided in configInfo");

        EngineSystems::get_instance().get_logger()->debug(LoggerType::Rendering, "Reading vertex shader code");
        auto vert_shader_code = FileUtils::read_file(vert_filepath); // "resources/shaders/shader.vert.spv"

        EngineSystems::get_instance().get_logger()->debug(LoggerType::Rendering, "Reading fragment shader code");
        auto frag_shader_code = FileUtils::read_file(frag_filepath); // "resources/shaders/shader.frag.spv"

        m_vert_shader_module = create_shader_module(vert_shader_code);
        m_frag_shader_module = create_shader_module(frag_shader_code);

        VkPipelineShaderStageCreateInfo vert_shader_stage_info{};
        vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vert_shader_stage_info.module = m_vert_shader_module;
        vert_shader_stage_info.pName = "main";

        VkPipelineShaderStageCreateInfo frag_shader_stage_info{};
        frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        frag_shader_stage_info.module = m_frag_shader_module;
        frag_shader_stage_info.pName = "main";

        VkPipelineShaderStageCreateInfo shader_stages[] = {vert_shader_stage_info, frag_shader_stage_info};

        auto binding_description = Vertex::get_vertex_input_binding_description();
        auto attribute_descriptions = Vertex::get_attribute_descriptions();

        VkPipelineVertexInputStateCreateInfo vertex_input_info{};
        vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertex_input_info.vertexBindingDescriptionCount = 1;
        vertex_input_info.pVertexBindingDescriptions = &binding_description; // Optional
        vertex_input_info.vertexAttributeDescriptionCount = 1;
        vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size());
        vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions.data(); // Optional

        VkGraphicsPipelineCreateInfo pipeline_info{};
        pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipeline_info.stageCount = 2;
        pipeline_info.pStages = shader_stages;
        pipeline_info.pVertexInputState = &vertex_input_info;
        pipeline_info.pInputAssemblyState = &config_info.inputAssemblyInfo;
        pipeline_info.pViewportState = &config_info.viewportInfo;
        pipeline_info.pRasterizationState = &config_info.rasterizationInfo;
        pipeline_info.pMultisampleState = &config_info.multisampleInfo;
        pipeline_info.pColorBlendState = &config_info.colorBlendInfo;
        pipeline_info.pDepthStencilState = &config_info.depthStencilInfo;
        pipeline_info.pDynamicState = &config_info.dynamicStateInfo;

        pipeline_info.layout = config_info.pipelineLayout;
        pipeline_info.renderPass = config_info.renderPass;
        pipeline_info.subpass = config_info.subpass;

        pipeline_info.basePipelineIndex = -1;
        pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(
                m_device->get_vk_device(),
                VK_NULL_HANDLE,
                1,
                &pipeline_info,
                nullptr,
                &m_vk_graphics_pipeline) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline");
        }
    }

    VkShaderModule Pipeline::create_shader_module(const std::vector<char> &code) {
        VkShaderModuleCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        create_info.codeSize = code.size();
        create_info.pCode = reinterpret_cast<const uint32_t *>(code.data());

        VkShaderModule shader_module;
        if (vkCreateShaderModule(m_device->get_vk_device(), &create_info, nullptr, &shader_module) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module!");
        }

        return shader_module;
    }
    
    void Pipeline::default_pipeline_config_info(PipelineConfigInfo &config_info) {
        config_info.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        config_info.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        config_info.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        config_info.viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        config_info.viewportInfo.viewportCount = 1;
        config_info.viewportInfo.pViewports = nullptr;
        config_info.viewportInfo.scissorCount = 1;
        config_info.viewportInfo.pScissors = nullptr;


        config_info.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        config_info.rasterizationInfo.depthClampEnable = VK_FALSE;
        config_info.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
        config_info.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
        config_info.rasterizationInfo.lineWidth = 1.0f;
        config_info.rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
        config_info.rasterizationInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        config_info.rasterizationInfo.depthBiasEnable = VK_FALSE;
        config_info.rasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
        config_info.rasterizationInfo.depthBiasClamp = 0.0f;           // Optional
        config_info.rasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional

        config_info.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        config_info.multisampleInfo.sampleShadingEnable = VK_FALSE;
        config_info.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        config_info.multisampleInfo.minSampleShading = 1.0f;           // Optional
        config_info.multisampleInfo.pSampleMask = nullptr;             // Optional
        config_info.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
        config_info.multisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional

        config_info.colorBlendAttachment.colorWriteMask =
                VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                VK_COLOR_COMPONENT_A_BIT;
        config_info.colorBlendAttachment.blendEnable = VK_FALSE;
        config_info.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
        config_info.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
        config_info.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
        config_info.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
        config_info.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
        config_info.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

        config_info.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        config_info.colorBlendInfo.logicOpEnable = VK_FALSE;
        config_info.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
        config_info.colorBlendInfo.attachmentCount = 1;
        config_info.colorBlendInfo.pAttachments = &config_info.colorBlendAttachment;
        config_info.colorBlendInfo.blendConstants[0] = 0.0f;  // Optional
        config_info.colorBlendInfo.blendConstants[1] = 0.0f;  // Optional
        config_info.colorBlendInfo.blendConstants[2] = 0.0f;  // Optional
        config_info.colorBlendInfo.blendConstants[3] = 0.0f;  // Optional

        config_info.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        config_info.depthStencilInfo.depthTestEnable = VK_TRUE;
        config_info.depthStencilInfo.depthWriteEnable = VK_TRUE;
        config_info.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
        config_info.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
        config_info.depthStencilInfo.minDepthBounds = 0.0f;  // Optional
        config_info.depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
        config_info.depthStencilInfo.stencilTestEnable = VK_FALSE;
        config_info.depthStencilInfo.front = {};  // Optional
        config_info.depthStencilInfo.back = {};   // Optional

        config_info.dynamicStateEnables = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
        config_info.dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        config_info.dynamicStateInfo.pDynamicStates = config_info.dynamicStateEnables.data();
        config_info.dynamicStateInfo.dynamicStateCount =
                static_cast<uint32_t>(config_info.dynamicStateEnables.size());
        config_info.dynamicStateInfo.flags = 0;

    }


} // moonshine