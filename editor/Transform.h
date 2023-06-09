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
};

#endif //MOONSHINE_TRANSFORM_H
