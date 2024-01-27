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
        std::shared_ptr<InputHandler> input_handler = window->get_input_handler();

        using std::placeholders::_1;
        std::function<void(bool)> handle_movement_mode_func = std::bind(&Camera::handle_movement_mode, this, _1);
        m_registered_functions.push_back(
                input_handler->register_key_event(GLFW_MOUSE_BUTTON_RIGHT, handle_movement_mode_func, true));

        std::function<void(bool)> forward = std::bind(&Camera::move_forward, this, _1);
        m_registered_functions.push_back(input_handler->register_key_event(GLFW_KEY_W, forward));

        std::function<void(bool)> backward = std::bind(&Camera::move_backward, this, _1);
        m_registered_functions.push_back(input_handler->register_key_event(GLFW_KEY_S, backward));

        std::function<void(bool)> right = std::bind(&Camera::move_right, this, _1);
        m_registered_functions.push_back(input_handler->register_key_event(GLFW_KEY_D, right));

        std::function<void(bool)> left = std::bind(&Camera::move_left, this, _1);
        m_registered_functions.push_back(input_handler->register_key_event(GLFW_KEY_A, left));

        std::function<void(CursorPosition)> camera_mouse_movement = std::bind(&Camera::mouse_movement, this, _1);
        m_registered_functions.push_back(input_handler->register_mouse_event(camera_mouse_movement));
    }

    Camera::~Camera() {

        // Unregister input events
        for (auto item: m_registered_functions) {
            m_window->get_input_handler()->unregister_key_event(item);
        }
    }

    glm::mat4 Camera::get_view_mat() {
        glm::mat4 rotation = glm::mat4_cast(m_transform.rotation);
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), -m_transform.position);
        glm::mat4 view = rotation * translation;
        return view;
    }

    void Camera::handle_movement_mode(bool is_released) {
        if (is_released) {
            m_movement_mode_active = false;
            m_window->set_cursor_mode();
        } else if (!m_movement_mode_active) {
            m_movement_mode_active = true;
            m_window->set_no_cursor_mode();
        }
    }

    void Camera::move(glm::vec3 to_move, bool ignore_active_mode) {
        if (!m_movement_mode_active && !ignore_active_mode) return;

        m_transform.position += to_move;
    }

    void Camera::move_forward(bool is_released) {
        move(m_transform.get_forward_vector() * -1.0f * m_cam_speed * Time::s_delta_time);
    }

    void Camera::move_backward(bool is_released) {
        move(m_transform.get_forward_vector() * m_cam_speed * Time::s_delta_time);
    }

    void Camera::move_right(bool is_released) {
        move(m_transform.get_right_vector() * -1.0f * m_cam_speed * Time::s_delta_time);
    }

    void Camera::move_left(bool is_released) {
        move(m_transform.get_right_vector() * m_cam_speed * Time::s_delta_time);
    }

    void Camera::mouse_movement(CursorPosition cPos) {
        if (!m_movement_mode_active) return;

        float deltaX = cPos.x - cPos.oldX;
        float deltaY = cPos.oldY - cPos.y;

        deltaX *= Time::s_delta_time * m_rotation_speed;
        deltaY *= Time::s_delta_time * m_rotation_speed;

        glm::quat pitch = glm::angleAxis(-deltaY, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::quat yaw = glm::angleAxis(deltaX, glm::vec3(0.0f, 1.0f, 0.0f));
        m_transform.rotation = pitch * m_transform.rotation * yaw;
    }


    void Camera::look_at(const Transform &target) {
        m_transform.position = target.position;

        move(m_transform.get_forward_vector() * 5.0f, true);
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