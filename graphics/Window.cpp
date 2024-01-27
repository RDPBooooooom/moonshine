//
// Created by marvin on 29.05.2023.
//

#include "Window.h"

#include <utility>
#include <stdexcept>
#include "GLFW/glfw3.h"

namespace moonshine {

    Window::Window(std::string name, int w, int h) : m_width{w}, m_height{h}, m_window(create_window(w, h, name)) {
        m_name = std::move(name);

        m_input_handler = std::make_shared<InputHandler>(m_window);

        glfwSetWindowUserPointer(m_window, this);
        glfwSetFramebufferSizeCallback(m_window, framebuffer_resize_callback);
        glfwSetKeyCallback(m_window, key_callback);
        glfwSetMouseButtonCallback(m_window, mouse_btn_callback);

        set_cursor_mode();
    }

    Window::~Window() {
        glfwDestroyWindow(m_window);

        glfwTerminate();
    }

    void Window::framebuffer_resize_callback(GLFWwindow *window, int width, int height) {
        auto moon_window = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
        moon_window->m_framebuffer_resized = true;
        moon_window->m_width = width;
        moon_window->m_height = height;
    }

    void Window::create_surface(VkInstance instance, VkSurfaceKHR *surface) {
        if (glfwCreateWindowSurface(instance, m_window, nullptr, surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
    }

    bool Window::should_close() {
        return glfwWindowShouldClose(m_window);
    }

    void Window::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
        auto moon_window = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
        moon_window->get_input_handler()->on_keypress(key, scancode, action, mods);
    }

    void Window::mouse_btn_callback(GLFWwindow *window, int key, int action, int mods) {
        auto moon_window = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
        moon_window->get_input_handler()->on_mouse_press(key, action, mods);
    }

    void Window::set_no_cursor_mode() {
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    void Window::set_cursor_mode() {
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
} // moonshine