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
    glm::quat rotation = glm::quat(0, 0, 0, 1);
    glm::vec3 scale = glm::vec3(1, 1, 1);

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
    
    void deserialize(boost::json::object obj){
        auto posObj = obj.at("position").as_object();
        position.x = posObj.at("x").as_double();
        position.y = posObj.at("y").as_double();
        position.z = posObj.at("z").as_double();

        // Deserialize rotation
        auto rotObj = obj.at("rotation").as_object();
        rotation.w = rotObj.at("w").as_double();
        rotation.x = rotObj.at("x").as_double();
        rotation.y = rotObj.at("y").as_double();
        rotation.z = rotObj.at("z").as_double();

        // Deserialize scale
        auto scaleObj = obj.at("scale").as_object();
        scale.x = scaleObj.at("x").as_double();
        scale.y = scaleObj.at("y").as_double();
        scale.z = scaleObj.at("z").as_double();
    }
};

#endif //MOONSHINE_TRANSFORM_H
