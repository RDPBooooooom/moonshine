//
// Created by marvin on 22.06.2023.
//

#ifndef MOONSHINE_MODELLOADER_H
#define MOONSHINE_MODELLOADER_H

#include <string>
#include <iostream>
#include "tiny_gltf.h"


namespace moonshine {

    class ModelLoader {

    public:
        
        static bool load_ascii_model(tinygltf::Model &model, const char* filepath);
    };

} // moonshine

#endif //MOONSHINE_MODELLOADER_H
