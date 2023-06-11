//
// Created by marvin on 11.06.2023.
//

#ifndef MOONSHINE_COMMANDBUFFERUTILS_H
#define MOONSHINE_COMMANDBUFFERUTILS_H

#include <vulkan/vulkan_core.h>
#include "../graphics/Device.h"

namespace moonshine {
    
    inline VkCommandBuffer beginSingleTimeCommands(Device *device, VkCommandPool commandPool) {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = commandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(device->getVkDevice(), &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        return commandBuffer;
    }

    inline void endSingleTimeCommands(Device *device, VkCommandBuffer commandBuffer, VkCommandPool vkCommandPool) {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(device->getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(device->getGraphicsQueue());

        vkFreeCommandBuffers(device->getVkDevice(), vkCommandPool, 1, &commandBuffer);
    }
}

#endif //MOONSHINE_COMMANDBUFFERUTILS_H
