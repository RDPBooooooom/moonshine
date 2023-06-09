//
// Created by marvi on 08.06.2023.
//

#ifndef MOONSHINE_CAMERA_H
#define MOONSHINE_CAMERA_H

#include "Transform.h"
#include<glm/glm.hpp>
#include<glm/gtc/quaternion.hpp>
#include<glm/common.hpp>

namespace moonshine {

    class Camera {

    private:
        Transform m_transform{};

    public:
        Camera();

        glm::mat4 GetViewMat();

        Transform *getTransform() { return &m_transform; }
    };

} // moonshine

#endif //MOONSHINE_CAMERA_H
