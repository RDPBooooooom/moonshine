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

        VkSwapchainKHR m_vkSwapChain;
        std::vector<VkImage> m_swapChainImages;
        std::vector<VkImageView> m_swapChainImageViews;
        VkFormat m_swapChainImageFormat;
        VkExtent2D m_swapChainExtent;
        VkRenderPass m_vkRenderPass;

        VkDescriptorSetLayout m_descriptorSetLayout;

        VkPipelineLayout m_vkPipelineLayout;
        VkPipeline m_vkGraphicsPipeline;

        std::vector<VkFramebuffer> m_swapChainFramebuffers;

    private:
        void createSwapChain();

        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

        void createImageViews();

        void createRenderPass();

        void createDescriptorSetLayout();

        void createGraphicsPipeline();

        VkShaderModule createShaderModule(const std::vector<char> &code);

        void createFramebuffers();

        void cleanupSwapChain();

    public:
        Pipeline(Window &window, Device &device);

        ~Pipeline();

        void recreateSwapChain();

        VkSwapchainKHR getSwapChain() { return m_vkSwapChain; }

        VkExtent2D getSwapChainExtent() { return m_swapChainExtent; }

        VkRenderPass getRenderPass() { return m_vkRenderPass; }

        VkPipeline getGraphicsPipeline() { return m_vkGraphicsPipeline; }

        VkPipelineLayout getPipelineLayout() { return m_vkPipelineLayout; }

        VkDescriptorSetLayout getDiscriptorSetLayout() { return m_descriptorSetLayout; }

        std::vector<VkFramebuffer> getFramebuffers() { return m_swapChainFramebuffers; }
    };

} // moonshine

#endif //MOONSHINE_PIPELINE_H
