//
// Created by marvi on 27.07.2023.
//

#include <stdexcept>
#include <functional>
#include <limits>
#include "SwapChain.h"
#include "../utils/Constants.h"

namespace moonshine {
    void SwapChain::createSwapChain() {
        //TODO: Move querySwapChainSupport on device
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_device.getVkPhysicalDevice(),
                                                                         m_device.getVkSurface());

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 &&
            imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = m_device.getVkSurface();

        createInfo.minImageCount = 2;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        //TODO: Move find Queue families to device
        QueueFamilyIndices indices = findQueueFamilies(m_device.getVkPhysicalDevice(), m_device.getVkSurface());
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        if (indices.graphicsFamily != indices.presentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0; // Optional
            createInfo.pQueueFamilyIndices = nullptr; // Optional
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = m_oldSwapChain == nullptr ? VK_NULL_HANDLE : m_oldSwapChain->getSwapChain();

        if (vkCreateSwapchainKHR(m_device.getVkDevice(), &createInfo, nullptr, &m_vkSwapChain) != VK_SUCCESS) {
            throw std::runtime_error("failed to create swap chain!");
        }

        vkGetSwapchainImagesKHR(m_device.getVkDevice(), m_vkSwapChain, &imageCount, nullptr);
        m_swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(m_device.getVkDevice(), m_vkSwapChain, &imageCount, m_swapChainImages.data());

        m_swapChainImageFormat = surfaceFormat.format;
        m_swapChainExtent = extent;
    }

    VkSurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) {

        for (const auto &availableFormat: availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    VkPresentModeKHR SwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes) {
        for (const auto &availablePresentMode: availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        } else {
            VkExtent2D actualExtent = m_windowExtent;

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width,
                                            capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height,
                                             capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    void SwapChain::createRenderPass() {
        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = findDepthFormat();
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = m_swapChainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depthAttachmentRef;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = attachments.size();
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(m_device.getVkDevice(), &renderPassInfo, nullptr, &m_vkRenderPass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }

    }

    void SwapChain::createImageViews() {
        m_swapChainImageViews.resize(m_swapChainImages.size());

        int i = 0;
        for (auto &m_swapChainImage: m_swapChainImages) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = m_swapChainImage;
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = m_swapChainImageFormat;

            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;


            if (vkCreateImageView(m_device.getVkDevice(), &createInfo, nullptr, &m_swapChainImageViews[i]) !=
                VK_SUCCESS) {
                throw std::runtime_error("failed to create image views!");
            }
            i++;
        }

    }

    void SwapChain::createFramebuffers() {
        m_swapChainFramebuffers.resize(m_swapChainImageViews.size());


        for (size_t i = 0; i < m_swapChainImageViews.size(); i++) {
            std::array<VkImageView, 2> attachments = {m_swapChainImageViews[i], m_depthImageViews[i]};

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = m_vkRenderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());;
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = m_swapChainExtent.width;
            framebufferInfo.height = m_swapChainExtent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(m_device.getVkDevice(), &framebufferInfo, nullptr,
                                    &m_swapChainFramebuffers[i]) !=
                VK_SUCCESS) {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }

    void SwapChain::createSyncObjects() {
        m_vkImageAvailableSemaphores.resize(moonshine::MAX_FRAMES_IN_FLIGHT);
        m_vkRenderFinishedSemaphores.resize(moonshine::MAX_FRAMES_IN_FLIGHT);
        m_vkInFlightFences.resize(moonshine::MAX_FRAMES_IN_FLIGHT);
        m_imagesInFlight.resize(imageCount(), VK_NULL_HANDLE);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < moonshine::MAX_FRAMES_IN_FLIGHT; i++) {
            if (vkCreateSemaphore(m_device.getVkDevice(), &semaphoreInfo, nullptr,
                                  &m_vkImageAvailableSemaphores[i]) !=
                VK_SUCCESS ||
                vkCreateSemaphore(m_device.getVkDevice(), &semaphoreInfo, nullptr,
                                  &m_vkRenderFinishedSemaphores[i]) !=
                VK_SUCCESS ||
                vkCreateFence(m_device.getVkDevice(), &fenceInfo, nullptr, &m_vkInFlightFences[i]) !=
                VK_SUCCESS) {

                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }
    }

    VkResult SwapChain::acquireNextImage(uint32_t *imageIndex) {
        vkWaitForFences(
                m_device.getVkDevice(),
                1,
                &m_vkInFlightFences[m_currentFrame],
                VK_TRUE,
                std::numeric_limits<uint64_t>::max());

        VkResult result = vkAcquireNextImageKHR(
                m_device.getVkDevice(),
                m_vkSwapChain,
                std::numeric_limits<uint64_t>::max(),
                m_vkImageAvailableSemaphores[m_currentFrame],  // must be a not signaled semaphore
                VK_NULL_HANDLE,
                imageIndex);

        return result;
    }

    VkResult SwapChain::submitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex) {
        if (m_imagesInFlight[*imageIndex] != VK_NULL_HANDLE) {
            vkWaitForFences(m_device.getVkDevice(), 1, &m_vkInFlightFences[*imageIndex], VK_TRUE, UINT64_MAX);
        }
        m_imagesInFlight[*imageIndex] = m_vkInFlightFences[m_currentFrame];

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {m_vkImageAvailableSemaphores[m_currentFrame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = buffers;

        VkSemaphore signalSemaphores[] = {m_vkRenderFinishedSemaphores[m_currentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        vkResetFences(m_device.getVkDevice(), 1, &m_vkInFlightFences[m_currentFrame]);
        if (vkQueueSubmit(m_device.getGraphicsQueue(), 1, &submitInfo, m_vkInFlightFences[m_currentFrame]) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {m_vkSwapChain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = imageIndex;

        auto result = vkQueuePresentKHR(m_device.getPresentQueue(), &presentInfo);

        m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

        return result;
    }

    void SwapChain::cleanupSwapChain() {
        for (auto &m_swapChainFramebuffer: m_swapChainFramebuffers) {
            vkDestroyFramebuffer(m_device.getVkDevice(), m_swapChainFramebuffer, nullptr);
        }

        for (auto &m_swapChainImageView: m_swapChainImageViews) {
            vkDestroyImageView(m_device.getVkDevice(), m_swapChainImageView, nullptr);
        }

        vkDestroySwapchainKHR(m_device.getVkDevice(), m_vkSwapChain, nullptr);
    }

    SwapChain::SwapChain(moonshine::Device &deviceRef, VkExtent2D windowExtent) : m_device{deviceRef},
                                                                                  m_windowExtent(windowExtent) {
        init();
    }

    SwapChain::SwapChain(moonshine::Device &deviceRef, VkExtent2D windowExtent, std::shared_ptr<SwapChain> previous)
            : m_device{deviceRef}, m_windowExtent(windowExtent) {
        init();
        m_oldSwapChain = std::move(previous);
    }

    void SwapChain::init() {
        createSwapChain();
        createImageViews();
        createRenderPass();
        createDepthResources();
        createFramebuffers();
        createSyncObjects();
    }

    SwapChain::~SwapChain() {
        
        for (int i = 0; i < m_depthImages.size(); i++) {
            vkDestroyImageView(m_device.getVkDevice(), m_depthImageViews[i], nullptr);
            vkDestroyImage(m_device.getVkDevice(), m_depthImages[i], nullptr);
            vkFreeMemory(m_device.getVkDevice(), m_depthImageMemorys[i], nullptr);
        }
        
        for (size_t i = 0; i < moonshine::MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(m_device.getVkDevice(), m_vkRenderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(m_device.getVkDevice(), m_vkImageAvailableSemaphores[i], nullptr);
            vkDestroyFence(m_device.getVkDevice(), m_vkInFlightFences[i], nullptr);
        }

        cleanupSwapChain();
        vkDestroyRenderPass(m_device.getVkDevice(), m_vkRenderPass, nullptr);
    }

    void SwapChain::createDepthResources() {
        VkFormat depthFormat = findDepthFormat();
        m_swapChainDepthFormat = depthFormat;
        VkExtent2D swapChainExtent = getSwapChainExtent();

        m_depthImages.resize(imageCount());
        m_depthImageMemorys.resize(imageCount());
        m_depthImageViews.resize(imageCount());

        for (int i = 0; i < m_depthImages.size(); i++) {
            VkImageCreateInfo imageInfo{};
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType = VK_IMAGE_TYPE_2D;
            imageInfo.extent.width = swapChainExtent.width;
            imageInfo.extent.height = swapChainExtent.height;
            imageInfo.extent.depth = 1;
            imageInfo.mipLevels = 1;
            imageInfo.arrayLayers = 1;
            imageInfo.format = depthFormat;
            imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.flags = 0;

            m_device.createImageWithInfo(
                    imageInfo,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                    m_depthImages[i],
                    m_depthImageMemorys[i]);

            VkImageViewCreateInfo viewInfo{};
            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = m_depthImages[i];
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = depthFormat;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(m_device.getVkDevice(), &viewInfo, nullptr, &m_depthImageViews[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create texture image view!");
            }
        }
    }

    VkFormat SwapChain::findDepthFormat() {
        return m_device.findSupportedFormat(
                {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
                VK_IMAGE_TILING_OPTIMAL,
                VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    }


} // moonshine