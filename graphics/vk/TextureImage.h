//
// Created by marvin on 11.06.2023.
//

#ifndef MOONSHINE_TEXTUREIMAGE_H
#define MOONSHINE_TEXTUREIMAGE_H

#include <string>
#include "Device.h"

namespace moonshine {

    class TextureImage {

    private:
        Device *m_device;
        VkCommandPool m_vk_command_pool;

        VkImage m_vk_image;
        VkDeviceMemory m_vk_image_memory;

        VkImageView m_vk_image_view;

    public:
        TextureImage(const char *filepath, Device *device, VkCommandPool command_pool);

        ~TextureImage();

        void transition_image_layout(VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout);

        VkImageView get_image_view() { return m_vk_image_view; }

    private:
        void create_image_view();
    };

} // moonshine

#endif //MOONSHINE_TEXTUREIMAGE_H
