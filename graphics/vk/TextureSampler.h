//
// Created by marvin on 11.06.2023.
//

#ifndef MOONSHINE_TEXTURESAMPLER_H
#define MOONSHINE_TEXTURESAMPLER_H

#include "Device.h"

namespace moonshine {

    class TextureSampler {

    private:
        Device &m_device;
        VkSampler m_vk_sampler;

    public:
        explicit TextureSampler(Device &device);

        ~TextureSampler();

        VkSampler get_vk_sampler() { return m_vk_sampler; }
    };

} // moonshine

#endif //MOONSHINE_TEXTURESAMPLER_H
