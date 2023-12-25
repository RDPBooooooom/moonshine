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
        VkSampler m_vkSampler;

    public:
        explicit TextureSampler(Device &device);

        ~TextureSampler();

        VkSampler getVkSampler() { return m_vkSampler; }
    };

} // moonshine

#endif //MOONSHINE_TEXTURESAMPLER_H
