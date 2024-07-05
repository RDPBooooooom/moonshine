//
// Created by marvin on 29.05.2023.
//

#ifndef MOONSHINE_WINDOW_H
#define MOONSHINE_WINDOW_H

#include <string>
#include <vulkan/vulkan_core.h>
#include <memory>
#include "GLFW/glfw3.h"
#include "../../editor/InputHandler.h"

namespace moonshine {

    class Window {

    private:
        GLFWwindow *m_window;
        std::shared_ptr<InputHandler> m_input_handler;

        std::string m_name;

    public:
        int m_width;
        int m_height;
        bool m_framebuffer_resized;

    public:
        Window(std::string name, int w, int h);

        ~Window();

        Window &operator=(const Window &) = delete;

        std::shared_ptr<InputHandler> get_input_handler() { return m_input_handler; }

        static void framebuffer_resize_callback(GLFWwindow *window, int width, int height);

        static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

        static void mouse_btn_callback(GLFWwindow *window, int key, int action, int mods);

        bool should_close();

        void create_surface(VkInstance instance, VkSurfaceKHR *surface);

        void set_no_cursor_mode();

        void set_cursor_mode();

        VkExtent2D get_extent() { return {static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height)}; }

        GLFWwindow *get_glfw_window() { return m_window; }

    private:
        static GLFWwindow *create_window(int width, int height, const std::string &name) {
            glfwInit();
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            return glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
        }
    };

} // moonshine

#endif //MOONSHINE_WINDOW_H
