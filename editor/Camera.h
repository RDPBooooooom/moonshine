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

        bool m_movementModeActive = false;

        float m_camSpeed = 10;
        float m_rotationSpeed = 0.15f;

        std::vector<int> m_registeredFunctions;

    public:
        explicit Camera(Window *window);

        ~Camera();

        glm::mat4 getViewMat();

        Transform *getTransform() { return &m_transform; }

        void look_at(const Transform &target);
        void show_debug();

    private:
        void handleMovementMode(bool isReleased);

        void move(glm::vec3 toMove, bool ignore_active_mode = false);

        void moveForward(bool isReleased);

        void moveBackward(bool isReleased);

        void moveRight(bool isReleased);

        void moveLeft(bool isReleased);

        void mouseMovement(CursorPosition cPos);

    };

} // moonshine

#endif //MOONSHINE_CAMERA_H
