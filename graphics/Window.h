//
// Created by marvin on 29.05.2023.
//

#ifndef MOONSHINE_WINDOW_H
#define MOONSHINE_WINDOW_H

#include <string>
#include <vulkan/vulkan_core.h>
#include "GLFW/glfw3.h"
#include "../editor/InputHandler.h"

namespace moonshine {

    class Window {

    private:
        GLFWwindow *m_window;
        InputHandler m_inputHandler;

        std::string m_name;

    public:
        int m_width;
        int m_height;
        bool m_framebufferResized;

    public:
        Window(std::string name, int w, int h);

        ~Window();

        Window &operator=(const Window &) = delete;

        InputHandler *getInputHandler() { return &m_inputHandler; }

        static void framebufferResizeCallback(GLFWwindow *window, int width, int height);

        static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

        bool shouldClose();

        void createSurface(VkInstance instance, VkSurfaceKHR *surface);

    private:
        static GLFWwindow *createWindow(int width, int height, const std::string &name) {
            glfwInit();
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            return glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
        }
    };

} // moonshine

#endif //MOONSHINE_WINDOW_H
