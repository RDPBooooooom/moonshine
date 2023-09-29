//
// Created by marvin on 28.07.2023.
//

#ifndef MOONSHINE_FRAMEINFO_H
#define MOONSHINE_FRAMEINFO_H

#include <vulkan/vulkan.h>
#include "../editor/Camera.h"

namespace moonshine {
// lib

    struct FrameInfo {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        Camera &camera;
        VkDescriptorSet globalDescriptorSet;
        std::vector<VkDescriptorSet> materialDescriptorSets;
    };
   
}
#endif //MOONSHINE_FRAMEINFO_H
