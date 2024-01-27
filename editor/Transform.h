//
// Created by marvi on 08.06.2023.
//

#ifndef MOONSHINE_TRANSFORM_H
#define MOONSHINE_TRANSFORM_H


#include<glm/glm.hpp>
#include<glm/gtc/quaternion.hpp>
#include<glm/common.hpp>
#include <boost/json/object.hpp>

struct Transform {
    glm::vec3 position;
    glm::quat rotation = glm::quat(1, 0, 0, 0);
    glm::vec3 scale = glm::vec3(1, 1, 1);

    glm::vec3 get_forward_vector() {
        glm::vec3 forward = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f) * mat4_cast(rotation);
        return forward;
    }

    glm::vec3 get_right_vector() {
        glm::vec3 right = glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f) * mat4_cast(rotation);
        return right;
    }

    glm::vec3 get_up_vector() {
        glm::vec3 up = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f) * mat4_cast(rotation);
        return up;
    }

    glm::mat4 get_model_matrix() {

        glm::mat4 model = glm::mat4(1.0f);

        // Apply the scale, rotation, and translation transformations.
// The order of multiplication is important due to the non-commutative nature of matrix multiplication.
        model = glm::translate(model, position);  // Apply translation.

// Apply rotation. Note that glm::rotate takes rotation in radians, not degrees.
        rotation = normalize(rotation);
        model *= glm::mat4_cast(rotation);

        model = glm::scale(model, scale);  // Apply scale.
        return model;
    }

    boost::json::object serialize() {
        return {
                {"position", {
                                     {"x", position.x},
                                     {"y", position.y},
                                     {"z", position.z}
                             }},
                {"rotation", {
                                     {"w", rotation.w},
                                     {"x", rotation.x},
                                     {"y", rotation.y},
                                     {"z", rotation.z}
                             }},
                {"scale",    {
                                     {"x", scale.x},
                                     {"y", scale.y},
                                     {"z", scale.z}
                             }}
        };
    }

    void deserialize(boost::json::object obj) {
        auto pos_obj = obj.at("position").as_object();
        position.x = pos_obj.at("x").as_double();
        position.y = pos_obj.at("y").as_double();
        position.z = pos_obj.at("z").as_double();

        // Deserialize rotation
        auto rot_obj = obj.at("rotation").as_object();
        rotation.w = rot_obj.at("w").as_double();
        rotation.x = rot_obj.at("x").as_double();
        rotation.y = rot_obj.at("y").as_double();
        rotation.z = rot_obj.at("z").as_double();

        // Deserialize scale
        auto scale_obj = obj.at("scale").as_object();
        scale.x = scale_obj.at("x").as_double();
        scale.y = scale_obj.at("y").as_double();
        scale.z = scale_obj.at("z").as_double();
    }
};

#endif //MOONSHINE_TRANSFORM_H
