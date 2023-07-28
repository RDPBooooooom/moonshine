//
// Created by marvi on 27.07.2023.
//

#ifndef MOONSHINE_RENDERER_H
#define MOONSHINE_RENDERER_H

#include "Window.h"
#include "Device.h"
#include "SwapChain.h"

namespace moonshine {

    class Renderer {

        Renderer &operator=(const Renderer &) = delete;

    private:
        Window &m_window;
        Device &m_device;
        std::unique_ptr<SwapChain> m_swapChain;
        uint32_t m_currentImageIndex{0};
        bool isFrameStarted = false;

        std::vector<VkCommandBuffer> m_vkCommandBuffers;

    private:
        
        void createCommandBuffers();

    public:

        Renderer(Window &window, Device &device);

        ~Renderer();

        VkCommandBuffer beginFrame();

        void endFrame();

        void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);

        void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

        bool isFrameInProgress() const { return isFrameStarted; }

        VkCommandBuffer getCurrentCommandBuffer() const {
            assert(isFrameStarted && "Cannot get command buffer when frame is not in progress");
            return m_vkCommandBuffers[m_currentImageIndex];
        }

        VkRenderPass getSwapChainRenderPass() const { return m_swapChain->getRenderPass(); }

        VkExtent2D getSwapChainExtent() { return m_swapChain->getSwapChainExtent();}

        void recreateSwapChain();

        void freeCommandBuffers();
        
        uint32_t getFrameIndex() { return m_currentImageIndex; }
    };

} // moonshine

#endif //MOONSHINE_RENDERER_H
