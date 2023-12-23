//
// Created by marvin on 16.11.2023.
//

#ifndef MOONSHINE_GLTFLOADER_H
#define MOONSHINE_GLTFLOADER_H

#include <algorithm>
#include "SceneObject.h"
#include "ModelLoader.h"
#include "Node.h"

namespace moonshine {

    class GltfLoader {

    private:
        static int nr_loaded_objects;
        
        static std::vector<std::shared_ptr<Node>>
        getSubmeshes(tinygltf::Model &model, tinygltf::Node &node, std::shared_ptr<Node> parent, std::string &path);

    public:
        static std::vector<std::shared_ptr<SceneObject>>
        load_gltf(std::string &filepath, std::string &filename, boost::uuids::uuid &uuid);

    };

} // moonshine

#endif //MOONSHINE_GLTFLOADER_H
