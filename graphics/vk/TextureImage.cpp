//
// Created by marvin on 11.06.2023.
//

#include "TextureImage.h"
#include "../utils/BufferUtils.h"

// Only include the define once in the project
#define STB_IMAGE_IMPLEMENTATION

#include<stb_image.h>
#include <vulkan/vulkan_core.h>
#include <stdexcept>

namespace moonshine {


    TextureImage::TextureImage(const char *filepath, Device *device, VkCommandPool command_pool) : m_device{device},
                                                                                                   m_vk_command_pool{
                                                                                                           command_pool} {
        int tex_width, tex_height, tex_channels;
        stbi_uc *pixels = stbi_load(filepath, &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
        VkDeviceSize image_size = tex_width * tex_height * 4;

        if (!pixels) {
            throw std::runtime_error("failed to load texture image! " + std::string(filepath));
        }

        VkBuffer staging_buffer;
        VkDeviceMemory staging_buffer_memory;

        create_buffer(image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer,
                      staging_buffer_memory, m_device);

        void *data;
        vkMapMemory(m_device->get_vk_device(), staging_buffer_memory, 0, image_size, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(image_size));
        vkUnmapMemory(m_device->get_vk_device(), staging_buffer_memory);

        stbi_image_free(pixels);

        VkImageCreateInfo image_info{};
        image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_info.imageType = VK_IMAGE_TYPE_2D;
        image_info.extent.width = static_cast<uint32_t>(tex_width);
        image_info.extent.height = static_cast<uint32_t>(tex_height);
        image_info.extent.depth = 1;
        image_info.mipLevels = 1;
        image_info.arrayLayers = 1;
        image_info.format = VK_FORMAT_R8G8B8A8_SRGB;
        image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
        image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        image_info.samples = VK_SAMPLE_COUNT_1_BIT;
        image_info.flags = 0; // Optional

        if (vkCreateImage(m_device->get_vk_device(), &image_info, nullptr, &m_vk_image) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image!");
        }

        VkMemoryRequirements mem_requirements;
        vkGetImageMemoryRequirements(m_device->get_vk_device(), m_vk_image, &mem_requirements);

        VkMemoryAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = mem_requirements.size;
        alloc_info.memoryTypeIndex = find_memory_type(mem_requirements.memoryTypeBits,
                                                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                      m_device);

        if (vkAllocateMemory(m_device->get_vk_device(), &alloc_info, nullptr, &m_vk_image_memory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate image memory!");
        }

        vkBindImageMemory(m_device->get_vk_device(), m_vk_image, m_vk_image_memory, 0);

        transition_image_layout(m_vk_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
                                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        copy_buffer_to_image(staging_buffer, m_vk_image, static_cast<uint32_t>(tex_width),
                             static_cast<uint32_t>(tex_height),
                             m_device, m_vk_command_pool);

        transition_image_layout(m_vk_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        vkDestroyBuffer(m_device->get_vk_device(), staging_buffer, nullptr);
        vkFreeMemory(m_device->get_vk_device(), staging_buffer_memory, nullptr);

        create_image_view();
    }

    // TODO: Abstract to make Pipeline and this use the same createImageView function
    void TextureImage::create_image_view() {

        VkImageViewCreateInfo view_info{};
        view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image = m_vk_image;
        view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format = VK_FORMAT_R8G8B8A8_SRGB;
        view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = 1;

        if (vkCreateImageView(m_device->get_vk_device(), &view_info, nullptr, &m_vk_image_view) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture image view!");
        }
    }

    TextureImage::~TextureImage() {
        vkDestroyImageView(m_device->get_vk_device(), m_vk_image_view, nullptr);
        vkDestroyImage(m_device->get_vk_device(), m_vk_image, nullptr);
        vkFreeMemory(m_device->get_vk_device(), m_vk_image_memory, nullptr);
    }

    void TextureImage::transition_image_layout(VkImage image, VkFormat format, VkImageLayout old_layout,
                                               VkImageLayout new_layout) {
        VkCommandBuffer command_buffer = begin_single_time_commands(m_device, m_vk_command_pool);

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = old_layout;
        barrier.newLayout = new_layout;

        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags source_stage;
        VkPipelineStageFlags destination_stage;

        if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
                   new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else {
            throw std::invalid_argument("unsupported layout transition!");
        }

        vkCmdPipelineBarrier(
                command_buffer,
                source_stage, destination_stage,
                0,
                0, nullptr,
                0, nullptr,
                1, &barrier
        );

        end_single_time_commands(m_device, command_buffer, m_vk_command_pool);
    }


} // moonshine