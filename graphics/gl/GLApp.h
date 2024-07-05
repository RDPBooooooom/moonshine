//
// Created by marvin on 25.06.2024.
//

#ifndef MOONSHINE_GLAPP_H
#define MOONSHINE_GLAPP_H

#include <GLFW/glfw3.h>
#include "../../editor/EngineSystems.h"
#include "../../deps/glad/include/glad/gl.h"
#include <glm/glm.hpp>


using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;

namespace moonshine {

    class GLApp {
    public:
        GLApp();

        ~GLApp() {
            glfwDestroyWindow(m_window);
            glfwTerminate();
        }

        GLFWwindow *getWindow() const { return m_window; }

        float getDeltaSeconds() const { return m_deltaSeconds; }

        void swapBuffers();

        std::shared_ptr<InputHandler> get_input_handler() { return m_input_handler; }

        bool should_close();

    private:
        GLFWwindow *m_window = nullptr;
        std::shared_ptr<InputHandler> m_input_handler;
        double m_timeStamp = glfwGetTime();
        float m_deltaSeconds = 0;
    };

} // moonshine

#endif //MOONSHINE_GLAPP_H
