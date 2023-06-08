//
// Created by marvi on 08.06.2023.
//

#include "Camera.h"
#include "ext/matrix_transform.hpp"

namespace moonshine {
    glm::mat4 Camera::GetViewmat() {
        glm::mat4 view = glm::mat4(1.0f);;
        view = glm::translate(view, m_transform.position);
        view = glm::rotate(view, m_transform.rotation);
        return glm::translate() ;
    }
} // moonshine