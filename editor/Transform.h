//
// Created by marvi on 08.06.2023.
//

#ifndef MOONSHINE_TRANSFORM_H
#define MOONSHINE_TRANSFORM_H


#include<glm/glm.hpp>
#include<glm/gtc/quaternion.hpp>
#include<glm/common.hpp>

struct Transform {
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scaling = glm::vec3(1,1,1);

    glm::vec3 getForward() {
        glm::vec3 forward = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f) * mat4_cast(rotation);
        return forward;
    }

    glm::vec3 getRight() {
        glm::vec3 right = glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f) * mat4_cast(rotation);
        return right;
    }

    glm::vec3 getUp() {
        glm::vec3 up = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f) * mat4_cast(rotation);
        return up;
    }

    glm::mat4 getMatrix() {
        glm::mat4 model = glm::mat4(1.0f);

        // Apply the scale, rotation, and translation transformations.
// The order of multiplication is important due to the non-commutative nature of matrix multiplication.
        model = glm::translate(model, position);  // Apply translation.

// Apply rotation. Note that glm::rotate takes rotation in radians, not degrees.
        model = glm::rotate(model, glm::radians(rotation.x),
                            glm::vec3(1.0f, 0.0f, 0.0f));  // Apply rotation around x-axis.
        model = glm::rotate(model, glm::radians(rotation.y),
                            glm::vec3(0.0f, 1.0f, 0.0f));  // Apply rotation around y-axis.
        model = glm::rotate(model, glm::radians(rotation.z),
                            glm::vec3(0.0f, 0.0f, 1.0f));  // Apply rotation around z-axis.

        model = glm::scale(model, scaling);  // Apply scale.
        return model;
    }
};

#endif //MOONSHINE_TRANSFORM_H
