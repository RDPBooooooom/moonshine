//
// Created by marvin on 01.06.2023.
//

#ifndef MOONSHINE_PIPELINE_H
#define MOONSHINE_PIPELINE_H

#include <vulkan/vulkan_core.h>
#include "Device.h"

namespace moonshine {

    class Pipeline {

    private:
        Window* m_window;
        Device* m_device;

        VkDescriptorSetLayout m_descriptorSetLayout;

        VkPipelineLayout m_vkPipelineLayout;
        VkPipeline m_vkGraphicsPipeline;

        std::vector<VkFramebuffer> m_swapChainFramebuffers;

    private:
        

        void createDescriptorSetLayout();

        void createGraphicsPipeline();

        VkShaderModule createShaderModule(const std::vector<char> &code);

        void createFramebuffers();

        

    public:
        Pipeline(Window &window, Device &device);

        ~Pipeline();

        VkPipeline getGraphicsPipeline() { return m_vkGraphicsPipeline; }

        VkPipelineLayout getPipelineLayout() { return m_vkPipelineLayout; }

        VkDescriptorSetLayout getDiscriptorSetLayout() { return m_descriptorSetLayout; }

        std::vector<VkFramebuffer> getFramebuffers() { return m_swapChainFramebuffers; }
    };

} // moonshine

#endif //MOONSHINE_PIPELINE_H
