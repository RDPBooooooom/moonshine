//
// Created by marvin on 29.05.2023.
//

#include "Window.h"

#include <utility>
#include <stdexcept>
#include "GLFW/glfw3.h"

namespace moonshine {

    Window::Window(std::string name, int w, int h) : m_width{w}, m_height{h}, m_window(createWindow(w, h, name)),
                                                     m_inputHandler(m_window) {
        m_name = std::move(name);

        glfwSetWindowUserPointer(m_window, this);
        glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);
        glfwSetKeyCallback(m_window, keyCallback);

        m_inputHandler = InputHandler(m_window);
    }

    Window::~Window() {
        glfwDestroyWindow(m_window);

        glfwTerminate();
    }

    void Window::framebufferResizeCallback(GLFWwindow *window, int width, int height) {
        auto moonWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
        moonWindow->m_framebufferResized = true;
        moonWindow->m_width = width;
        moonWindow->m_height = height;
    }

    void Window::createSurface(VkInstance instance, VkSurfaceKHR *surface) {
        if (glfwCreateWindowSurface(instance, m_window, nullptr, surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
    }

    bool Window::shouldClose() {
        return glfwWindowShouldClose(m_window);
    }

    void Window::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
        auto moonWindow = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
        moonWindow->getInputHandler()->onKeypress(key, scancode, action, mods);
    }
} // moonshine