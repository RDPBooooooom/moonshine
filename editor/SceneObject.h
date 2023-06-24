//
// Created by marvin on 22.06.2023.
//

#ifndef MOONSHINE_SCENEOBJECT_H
#define MOONSHINE_SCENEOBJECT_H


#include "../external/tinygltf/tiny_gltf.h"
#include "Transform.h"
#include "../utils/VkUtils.h"

namespace moonshine {
    class SceneObject {

    private:
        tinygltf::Model model;
        Transform transform;
        std::vector<Vertex> vertices;
        std::vector<uint16_t> indices;

    public:
        SceneObject(const char *filepath);


    };

}
#endif //MOONSHINE_SCENEOBJECT_H
