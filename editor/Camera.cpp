//
// Created by marvi on 08.06.2023.
//

#include "glm/gtc/type_ptr.hpp"
#include "Camera.h"
#include "Time.h"
#include "imgui.h"
#include "../MoonshineApp.h"

#define GLM_FORCE_RADIANS

namespace moonshine {

    Camera::Camera(Window *window) : m_window{window} {
        std::shared_ptr<InputHandler> inputHandler = window->getInputHandler();

        using std::placeholders::_1;
        std::function<void(bool)> handleMovementMode = std::bind(&Camera::handleMovementMode, this, _1);
        m_registeredFunctions.push_back(
                inputHandler->registerKeyEvent(GLFW_MOUSE_BUTTON_RIGHT, handleMovementMode, true));

        std::function<void(bool)> forward = std::bind(&Camera::moveForward, this, _1);
        m_registeredFunctions.push_back(inputHandler->registerKeyEvent(GLFW_KEY_W, forward));

        std::function<void(bool)> backward = std::bind(&Camera::moveBackward, this, _1);
        m_registeredFunctions.push_back(inputHandler->registerKeyEvent(GLFW_KEY_S, backward));

        std::function<void(bool)> right = std::bind(&Camera::moveRight, this, _1);
        m_registeredFunctions.push_back(inputHandler->registerKeyEvent(GLFW_KEY_D, right));

        std::function<void(bool)> left = std::bind(&Camera::moveLeft, this, _1);
        m_registeredFunctions.push_back(inputHandler->registerKeyEvent(GLFW_KEY_A, left));

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

    void Camera::handleMovementMode(bool isReleased) {
        if (isReleased) {
            m_movementModeActive = false;
            m_window->setCursorMode();
        } else if (!m_movementModeActive) {
            m_movementModeActive = true;
            m_window->setNoCursorMode();
        }
    }

    void Camera::move(glm::vec3 toMove, bool ignore_active_mode) {
        if (!m_movementModeActive && !ignore_active_mode) return;

        m_transform.position += toMove;
    }

    void Camera::moveForward(bool isReleased) {
        move(m_transform.getForward() * -1.0f * m_camSpeed * Time::deltaTime);
    }

    void Camera::moveBackward(bool isReleased) {
        move(m_transform.getForward() * m_camSpeed * Time::deltaTime);
    }

    void Camera::moveRight(bool isReleased) {
        move(m_transform.getRight() * -1.0f * m_camSpeed * Time::deltaTime);
    }

    void Camera::moveLeft(bool isReleased) {
        move(m_transform.getRight() * m_camSpeed * Time::deltaTime);
    }

    void Camera::mouseMovement(CursorPosition cPos) {
        if (!m_movementModeActive) return;

        float deltaX = cPos.x - cPos.oldX;
        float deltaY = cPos.oldY - cPos.y;

        deltaX *= Time::deltaTime * m_rotationSpeed;
        deltaY *= Time::deltaTime * m_rotationSpeed;

        glm::quat pitch = glm::angleAxis(-deltaY, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::quat yaw = glm::angleAxis(deltaX, glm::vec3(0.0f, 1.0f, 0.0f));
        m_transform.rotation = pitch * m_transform.rotation * yaw;
    }


    void Camera::look_at(const Transform &target) {
        m_transform.position = target.position;

        move(m_transform.getForward() * 5.0f, true);
    }

    void Camera::show_debug() {
        if (!MoonshineApp::APP_SETTINGS.ENABLE_CAMERA_DEBUG) return;

        ImGui::Begin("CameraDebug");

        ImGui::BeginDisabled(true);
        ImGui::SeparatorText("Transform");
        ImGui::InputFloat3("Position##camera", glm::value_ptr(m_transform.position));
        ImGui::InputFloat4("Rotation##camera", glm::value_ptr(m_transform.rotation));
        ImGui::InputFloat3("Scale##camera", glm::value_ptr(m_transform.scale));
        ImGui::EndDisabled();

        ImGui::End();
    }

} // moonshine