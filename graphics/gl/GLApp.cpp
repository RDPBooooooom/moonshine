//
// Created by marvin on 25.06.2024.
//

#include "GLApp.h"

namespace moonshine {
    GLApp::GLApp() {
        glfwSetErrorCallback(
                [](int error, const char *description) {
                    moonshine::EngineSystems::get_instance().get_logger()->critical(LoggerType::Editor,
                                                                                    "Error: %s\n", description);
                }
        );

        if (!glfwInit())
            exit(EXIT_FAILURE);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

        const GLFWvidmode *info = glfwGetVideoMode(glfwGetPrimaryMonitor());

        //MoonshineTodo: Change Window name
        m_window = glfwCreateWindow(info->width, info->height, "Simple example", nullptr, nullptr);

        if (!m_window) {
            glfwTerminate();
            exit(EXIT_FAILURE);
        }

        glfwMakeContextCurrent(m_window);
        gladLoadGL(glfwGetProcAddress);
        glfwSwapInterval(0);

        initDebug();

        m_input_handler = std::make_shared<InputHandler>(m_window);
        m_input_handler->set_cursor_mode()
    }

    void GLApp::swapBuffers() {
        glfwSwapBuffers(m_window);
        glfwPollEvents();
        assert(glGetError() == GL_NO_ERROR);

        const double newTimeStamp = glfwGetTime();
        m_deltaSeconds = static_cast<float>(newTimeStamp - m_timeStamp);
        m_timeStamp = newTimeStamp;
    }

    bool GLApp::should_close() {
        return glfwWindowShouldClose(m_window);
    }
} // moonshine