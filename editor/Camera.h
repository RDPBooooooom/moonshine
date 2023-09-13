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
        float m_rotationSpeed = 5;

        float m_angleH;
        float m_angleV;

        std::vector<int> m_registeredFunctions;

    public:
        explicit Camera(Window *window);

        ~Camera();

        glm::mat4 getViewMat();

        Transform *getTransform() { return &m_transform; }


    private:
        void handleMovementMode(bool isReleased);

        void move(glm::vec3 toMove);

        void moveForward(bool isReleased);

        void moveBackward(bool isReleased);

        void moveRight(bool isReleased);

        void moveLeft(bool isReleased);

        void mouseMovement(CursorPosition cPos);

        float getPercentRotation(float distance, float totalDistance);
    };

} // moonshine

#endif //MOONSHINE_CAMERA_H
