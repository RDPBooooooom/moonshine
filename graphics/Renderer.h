//
// Created by marvi on 27.07.2023.
//

#ifndef MOONSHINE_RENDERER_H
#define MOONSHINE_RENDERER_H

#include "Window.h"
#include "Device.h"

namespace moonshine {

    class Renderer {

        Renderer &operator=(const Renderer &) = delete;

    private:
        Window &m_window;
        Device &m_device;
        uint32_t m_currentFrame;
        bool isFrameStarted;

        std::vector<VkCommandBuffer> m_vkCommandBuffers;

        void createCommandBuffer();

    public:

        Renderer(Window &window, Device &device);

        VkCommandBuffer beginFrame();

        void endFrame();

        void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);

        void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

        bool isFrameInProgress() const { return isFrameStarted; }
        
        VkCommandBuffer getCurrentCommandBuffer() const {
            assert(isFrameStarted && "Cannot get command buffer when frame is not in progress");
            return m_vkCommandBuffers[m_currentFrame]; }

        VkRenderPass getSwapChainRenderPass() const { return}
    };

} // moonshine

#endif //MOONSHINE_RENDERER_H
