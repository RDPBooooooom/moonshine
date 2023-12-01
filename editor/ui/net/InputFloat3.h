//
// Created by marvin on 01.12.2023.
//

#ifndef MOONSHINE_INPUTFLOAT3_H
#define MOONSHINE_INPUTFLOAT3_H

#include "imgui.h"
#include "glm/gtc/type_ptr.hpp"
#include "../../EngineSystems.h"


namespace moonshine::net::ui {
    bool InputFloat3(const char *label, float v[3]);
} // moonshine::net::ui

#endif //MOONSHINE_INPUTFLOAT3_H
