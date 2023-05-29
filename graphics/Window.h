//
// Created by marvin on 29.05.2023.
//

#ifndef MOONSHINE_WINDOW_H
#define MOONSHINE_WINDOW_H

#include <string>
#include <vulkan/vulkan_core.h>
#include "GLFW/glfw3.h"

namespace moonshine {

    class Window {

    private:
        GLFWwindow *m_window;

        std::string m_name;

    public:
        int width;
        int height;
        bool m_framebufferResized;
        
    public:
        Window(std::string name, int w, int h);

        ~Window();

        Window(const Window &) = delete;

        Window &operator=(const Window &) = delete;

        static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
        
        bool shouldClose();

        void createSurface(VkInstance instance, VkSurfaceKHR *surface);


    };

} // moonshine

#endif //MOONSHINE_WINDOW_H
