//
// Created by marvin on 01.06.2023.
//

#ifndef MOONSHINE_PIPELINE_H
#define MOONSHINE_PIPELINE_H

#include <vulkan/vulkan_core.h>
#include "Device.h"

namespace moonshine {

    struct PipelineConfigInfo {
        PipelineConfigInfo() = default;

        PipelineConfigInfo(const PipelineConfigInfo &) = delete;

        PipelineConfigInfo &operator=(const PipelineConfigInfo &) = delete;

        VkPipelineViewportStateCreateInfo viewportInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        std::vector<VkDynamicState> dynamicStateEnables;
        VkPipelineDynamicStateCreateInfo dynamicStateInfo;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };


    class Pipeline {

    private:
        Window *m_window;
        Device *m_device;

        VkShaderModule m_vert_shader_module;
        VkShaderModule m_frag_shader_module;

        VkPipeline m_vk_graphics_pipeline;
        
    private:

        void create_graphics_pipeline(const std::string &vert_filepath, const std::string &frag_filepath,
                                      const PipelineConfigInfo &config_info);

        VkShaderModule create_shader_module(const std::vector<char> &code);

    public:
        Pipeline(Device &device, std::string vert_filepath, std::string frag_filepath,
                 const PipelineConfigInfo &config_info);

        ~Pipeline();

        VkPipeline get_graphics_pipeline() { return m_vk_graphics_pipeline; }

        static void default_pipeline_config_info(PipelineConfigInfo &config_info);

        void bind(VkCommandBuffer command_buffer);
    };

} // moonshine

#endif //MOONSHINE_PIPELINE_H
