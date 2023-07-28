//
// Created by marvi on 27.07.2023.
//

#ifndef MOONSHINE_SWAPCHAIN_H
#define MOONSHINE_SWAPCHAIN_H


#include <vulkan/vulkan.h>
#include <vector>
#include <memory>
#include "Device.h"

namespace moonshine {
    class SwapChain {

    private:
        moonshine::Device &m_device;
        
        VkExtent2D m_windowExtent;

        VkSwapchainKHR m_vkSwapChain;
        std::vector<VkImage> m_swapChainImages;
        std::vector<VkImageView> m_swapChainImageViews;
        VkFormat m_swapChainImageFormat;
        VkExtent2D m_swapChainExtent;
        VkRenderPass m_vkRenderPass;

        std::vector<VkSemaphore> m_vkImageAvailableSemaphores;
        std::vector<VkSemaphore> m_vkRenderFinishedSemaphores;
        std::vector<VkFence> m_vkInFlightFences;
        std::vector<VkFence> m_imagesInFlight;

        std::vector<VkFramebuffer> m_swapChainFramebuffers;

        std::shared_ptr<SwapChain> m_oldSwapChain;
        
        size_t m_currentFrame = 0;


    private:

        void createSwapChain();

        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

        void createImageViews();

        void createRenderPass();

        void createFramebuffers();

        void cleanupSwapChain();

        void init();
        void createSyncObjects();
        
    public:

        SwapChain(moonshine::Device &deviceRef, VkExtent2D windowExtent);

        SwapChain(moonshine::Device &deviceRef, VkExtent2D windowExtent, std::shared_ptr<SwapChain> previous);

        SwapChain(const SwapChain &) = delete;

        SwapChain &operator=(const SwapChain &) = delete;

        ~SwapChain();

        VkSwapchainKHR getSwapChain() { return m_vkSwapChain; }

        VkExtent2D getSwapChainExtent() { return m_swapChainExtent; }

        VkRenderPass getRenderPass() { return m_vkRenderPass; }

        std::vector<VkFramebuffer> getFramebuffers() { return m_swapChainFramebuffers; }
        
        bool compareSwapFormats(const SwapChain &swapChain) const {
            return swapChain.m_swapChainImageFormat == m_swapChainImageFormat;
        }

        VkResult acquireNextImage(uint32_t *imageIndex);

        VkResult submitCommandBuffers(VkCommandBuffer const *buffers, uint32_t *imageIndex);
        size_t imageCount() { return m_swapChainImages.size(); }
    };

} // moonshine
#endif //MOONSHINE_SWAPCHAIN_H
