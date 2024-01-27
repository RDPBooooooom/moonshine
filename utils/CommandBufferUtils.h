//
// Created by marvin on 11.06.2023.
//

#ifndef MOONSHINE_COMMANDBUFFERUTILS_H
#define MOONSHINE_COMMANDBUFFERUTILS_H

#include <vulkan/vulkan_core.h>
#include "../graphics/Device.h"

namespace moonshine {

    inline VkCommandBuffer begin_single_time_commands(Device * device, VkCommandPool
    command_pool) {
    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.
    sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.
    level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.
    commandPool = command_pool;
    alloc_info.
    commandBufferCount = 1;

    VkCommandBuffer command_buffer;
    vkAllocateCommandBuffers(device
    ->

    get_vk_device(), &alloc_info, &command_buffer

    );

    VkCommandBufferBeginInfo begin_info{};
    begin_info.
    sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.
    flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(command_buffer, &begin_info
    );

    return
    command_buffer;
}

inline void end_single_time_commands(Device *device, VkCommandBuffer command_buffer, VkCommandPool vk_command_pool) {
    vkEndCommandBuffer(command_buffer);

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;

    vkQueueSubmit(device->get_graphics_queue(), 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(device->get_graphics_queue());

    vkFreeCommandBuffers(device->get_vk_device(), vk_command_pool, 1, &command_buffer);
}

}

#endif //MOONSHINE_COMMANDBUFFERUTILS_H
