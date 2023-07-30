//
// Created by marvi on 08.06.2023.
//

#include "Camera.h"
#include "Time.h"
#include "glm/gtx/string_cast.hpp"
#define GLM_FORCE_RADIANS

namespace moonshine {

    Camera::Camera(Window *window) : m_window{window} {
        InputHandler *inputHandler = window->getInputHandler();

        std::function<void()> forward = std::bind(&Camera::moveForward, this);
        m_registeredFunctions.push_back(inputHandler->registerKeyEvent(GLFW_KEY_W, forward));

        std::function<void()> backward = std::bind(&Camera::moveBackward, this);
        m_registeredFunctions.push_back(inputHandler->registerKeyEvent(GLFW_KEY_S, backward));

        std::function<void()> right = std::bind(&Camera::moveRight, this);
        m_registeredFunctions.push_back(inputHandler->registerKeyEvent(GLFW_KEY_D, right));

        std::function<void()> left = std::bind(&Camera::moveLeft, this);
        m_registeredFunctions.push_back(inputHandler->registerKeyEvent(GLFW_KEY_A, left));

        using std::placeholders::_1;
        std::function<void(CursorPosition)> mouseMovement = std::bind(&Camera::mouseMovement, this, _1);
        m_registeredFunctions.push_back(inputHandler->registerMouseEvent(mouseMovement));
    }

    Camera::~Camera() {
        
        // Unregister input events
        for (auto item: m_registeredFunctions) {
            m_window->getInputHandler()->unregisterKeyEvent(item);
        }
    }

    glm::mat4 Camera::getViewMat() {
        glm::mat4 rotation = glm::mat4_cast(m_transform.rotation);
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), -m_transform.position);
        glm::mat4 view = rotation * translation;
        return view;
    }

    void Camera::move(glm::vec3 toMove) {
        m_transform.position += toMove;
    }

    void Camera::moveForward() {
        move(m_transform.getForward() * -1.0f * m_camSpeed * Time::deltaTime);
    }

    void Camera::moveBackward() {
        move(m_transform.getForward() * m_camSpeed * Time::deltaTime);
    }

    void Camera::moveRight() {
        move(m_transform.getRight() * -1.0f * m_camSpeed * Time::deltaTime);
    }
    void Camera::moveLeft() {
        move(m_transform.getRight() * m_camSpeed * Time::deltaTime);
    }

    void Camera::mouseMovement(CursorPosition cPos)
    {
        m_angleH += m_rotationSpeed * getPercentRotation(cPos.oldY - cPos.y, m_window->m_height) * Time::deltaTime;
        m_angleV += m_rotationSpeed * getPercentRotation(cPos.oldX - cPos.x, m_window->m_width) * Time::deltaTime;
        
        glm::vec3 yAxis = glm::vec3(0, 1, 0);
        
        // Rotate the view vector by the horizontal angle around the vertical axis
        glm::vec3 view = glm::vec3(0.0f, 0.0f, -1.0f);
        view = glm::angleAxis(glm::radians(m_angleV), yAxis) * view;
        view = glm::normalize(view);

        // Rotate the view vector by the vertical angle around the horizontal axis
        glm::vec3 u = glm::cross(yAxis, view);
        u = glm::normalize(u);
        
        m_transform.rotation = glm::angleAxis(glm::radians(-m_angleV), yAxis) * glm::angleAxis(glm::radians(m_angleH), u);
    }

    float Camera::getPercentRotation(float distance, float totalDistance)
    {
        return totalDistance / 100 * distance;
    }
    

} // moonshine