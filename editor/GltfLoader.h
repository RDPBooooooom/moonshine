//
// Created by marvin on 16.11.2023.
//

#ifndef MOONSHINE_GLTFLOADER_H
#define MOONSHINE_GLTFLOADER_H

#include <algorithm>
#include "SceneObject.h"

namespace moonshine {
    
    class GltfLoader {

    private:
        static int nr_loaded_objects;
    public:
        static std::vector<std::shared_ptr<SceneObject>> load_gltf(std::string filepath, std::string filename);
    };

} // moonshine

#endif //MOONSHINE_GLTFLOADER_H
