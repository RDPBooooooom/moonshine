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
        PipelineConfigInfo(const PipelineConfigInfo&) = delete;
        PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

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
        
        Window* m_window;
        Device* m_device;
        
        VkShaderModule m_vertShaderModule;
        VkShaderModule m_fragShaderModule;

        VkPipeline m_vkGraphicsPipeline;
        
    private:
        
        void createGraphicsPipeline(const std::string &vertFilepath, const std::string &fragFilepath,
                                    const PipelineConfigInfo &configInfo);

        VkShaderModule createShaderModule(const std::vector<char> &code);
        
    public:
        Pipeline(Device &device, std::string vertFilepath, std::string fragFilepath,
                 const PipelineConfigInfo &configInfo);

        ~Pipeline();

        VkPipeline getGraphicsPipeline() { return m_vkGraphicsPipeline; }
        
        static void defaultPipelineConfigInfo(PipelineConfigInfo &configInfo);

        void bind(VkCommandBuffer commandBuffer);
    };

} // moonshine

#endif //MOONSHINE_PIPELINE_H
