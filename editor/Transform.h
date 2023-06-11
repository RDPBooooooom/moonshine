//
// Created by marvi on 08.06.2023.
//

#ifndef MOONSHINE_TRANSFORM_H
#define MOONSHINE_TRANSFORM_H


#include<glm/glm.hpp>
#include<glm/gtc/quaternion.hpp>
#include<glm/common.hpp>

struct Transform{
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scaling;

    glm::vec3 getForward()
    {
        glm::vec3 forward = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f) * mat4_cast(rotation);
        return forward;
    }

    glm::vec3 getRight()
    {
        glm::vec3 right = glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f) * mat4_cast(rotation) ;
        return right;
    }

    glm::vec3 getUp()
    {
        glm::vec3 up = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f) * mat4_cast(rotation);
        return up;
    }
};

#endif //MOONSHINE_TRANSFORM_H
