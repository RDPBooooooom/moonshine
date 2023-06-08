//
// Created by marvi on 08.06.2023.
//

#ifndef MOONSHINE_CAMERA_H
#define MOONSHINE_CAMERA_H

#include "Transform.h"
#include "ext/matrix_float4x4.hpp"

namespace moonshine {

    class Camera {

    private:
        Transform m_transform{};
        
    public:
        Camera();
        
        glm::mat4 GetViewmat();
    };

} // moonshine

#endif //MOONSHINE_CAMERA_H
