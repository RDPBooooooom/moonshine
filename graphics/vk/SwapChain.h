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

        VkExtent2D m_window_extent;

        VkSwapchainKHR m_vk_swap_chain;
        std::vector<VkImage> m_swap_chain_images;
        std::vector<VkImageView> m_swap_chain_image_views;

        VkFormat m_swap_chain_image_format;
        VkFormat m_swap_chain_depth_format;

        VkExtent2D m_swap_chain_extent;
        VkRenderPass m_vk_render_pass;

        std::vector<VkImage> m_depth_images;
        std::vector<VkDeviceMemory> m_depth_image_memorys;
        std::vector<VkImageView> m_depth_image_views;

        std::vector<VkSemaphore> m_vk_image_available_semaphores;
        std::vector<VkSemaphore> m_vk_render_finished_semaphores;
        std::vector<VkFence> m_vk_in_flight_fences;
        std::vector<VkFence> m_images_in_flight;

        std::vector<VkFramebuffer> m_swap_chain_framebuffers;

        std::shared_ptr<SwapChain> m_old_swap_chain;

        size_t m_current_frame = 0;


    private:

        void create_swap_chain();

        VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR> &available_formats);

        VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR> &available_present_modes);

        VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR &capabilities);

        void create_image_views();

        void create_render_pass();

        void create_framebuffers();

        void create_depth_resources();

        VkFormat find_depth_format();

        void cleanup_swap_chain();

        void init();

        void create_sync_objects();

    public:

        SwapChain(moonshine::Device &device_ref, VkExtent2D window_extent);

        SwapChain(moonshine::Device &device_ref, VkExtent2D window_extent, std::shared_ptr<SwapChain> &previous);

        SwapChain(const SwapChain &) = delete;

        SwapChain &operator=(const SwapChain &) = delete;

        ~SwapChain();

        VkSwapchainKHR get_swap_chain() { return m_vk_swap_chain; }

        VkExtent2D get_swap_chain_extent() { return m_swap_chain_extent; }

        VkRenderPass get_render_pass() { return m_vk_render_pass; }

        std::vector<VkFramebuffer> get_framebuffers() { return m_swap_chain_framebuffers; }

        bool compare_swap_formats(const SwapChain &swap_chain) const {
            return swap_chain.m_swap_chain_image_format == m_swap_chain_image_format;
        }

        VkResult acquire_next_image(uint32_t *image_index);

        VkResult submit_command_buffers(const VkCommandBuffer *buffers, uint32_t *image_index);

        size_t image_count() { return m_swap_chain_images.size(); }
    };

} // moonshine
#endif //MOONSHINE_SWAPCHAIN_H
