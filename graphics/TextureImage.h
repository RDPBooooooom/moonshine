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
        VkCommandPool m_vkCommandPool;

        VkImage m_vkImage;
        VkDeviceMemory m_vkImageMemory;

        VkImageView m_vkImageView;

    public:
        TextureImage(const char *filepath, Device *device, VkCommandPool commandPool);

        ~TextureImage();

        void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

        VkImageView getImageView() { return m_vkImageView; }

    private:
        void createImageView();
    };

} // moonshine

#endif //MOONSHINE_TEXTUREIMAGE_H
