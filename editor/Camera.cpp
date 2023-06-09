//
// Created by marvi on 08.06.2023.
//

#include "Camera.h"

namespace moonshine {
    glm::mat4 Camera::GetViewMat() {
        glm::mat4 rotation = glm::mat4_cast(m_transform.rotation);
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), -m_transform.position);
        glm::mat4 view = translation * rotation;
        return view;
    }

    Camera::Camera() {

    }
} // moonshine