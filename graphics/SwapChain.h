//
// Created by marvi on 27.07.2023.
//

#ifndef MOONSHINE_SWAPCHAIN_H
#define MOONSHINE_SWAPCHAIN_H


#include <vulkan/vulkan.h>
#include <vector>

class SwapChain {

private:
    VkSwapchainKHR m_vkSwapChain;
    std::vector<VkImage> m_swapChainImages;
    std::vector<VkImageView> m_swapChainImageViews;
    VkFormat m_swapChainImageFormat;
    VkExtent2D m_swapChainExtent;
    VkRenderPass m_vkRenderPass;

private:

    void createSwapChain();

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

    void createImageViews();

    void createRenderPass();

    void cleanupSwapChain();

public:
    void recreateSwapChain();

    VkSwapchainKHR getSwapChain() { return m_vkSwapChain; }

    VkExtent2D getSwapChainExtent() { return m_swapChainExtent; }

    VkRenderPass getRenderPass() { return m_vkRenderPass; }
};


#endif //MOONSHINE_SWAPCHAIN_H
