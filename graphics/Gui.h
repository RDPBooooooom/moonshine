//
// Created by marvin on 20.05.2024.
//

#ifndef MOONSHINE_GUI_H
#define MOONSHINE_GUI_H

#include <vulkan/vulkan_core.h>
#include <cstdlib>
#include <cstdio>
#include "Device.h"
#include "Renderer.h"

namespace moonshine {

    class Gui {

    private:
        Window &m_window;
        Device &m_device;
        VkDescriptorPool m_imGuiPool;

    public:
        explicit Gui(Device &device, Window &window) : m_device{device}, m_window{window} {

        }

        ~Gui();

        void init_im_gui(Renderer &m_renderer);

        void new_frame();

        void render_frame(VkCommandBuffer command_buffer);

        void create_dock_space();

    private:
        static void check_vk_result(VkResult err) {
            if (err == 0)
                return;
            fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
            if (err < 0)
                abort();
        }
    };

} // moonshine

#endif //MOONSHINE_GUI_H
