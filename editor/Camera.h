//
// Created by marvi on 08.06.2023.
//

#ifndef MOONSHINE_CAMERA_H
#define MOONSHINE_CAMERA_H

#include "Transform.h"
#include "../graphics/Window.h"
#include<glm/glm.hpp>
#include<glm/gtc/quaternion.hpp>
#include<glm/common.hpp>

namespace moonshine {

    class Camera {

    private:
        Window *m_window;
        Transform m_transform{};

        bool m_movement_mode_active = false;

        float m_cam_speed = 10;
        float m_rotation_speed = 0.15f;

        std::vector<int> m_registered_functions;

    public:
        explicit Camera(Window *window);

        ~Camera();

        glm::mat4 get_view_mat();

        Transform *get_transform() { return &m_transform; }

        void look_at(const Transform &target);

        void show_debug();

    private:
        void handle_movement_mode(bool is_released);

        void move(glm::vec3 to_move, bool ignore_active_mode = false);

        void move_forward(bool is_released);

        void move_backward(bool is_released);

        void move_right(bool is_released);

        void move_left(bool is_released);

        void mouse_movement(CursorPosition cPos);

    };

} // moonshine

#endif //MOONSHINE_CAMERA_H
