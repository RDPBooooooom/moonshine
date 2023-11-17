//
// Created by marvin on 22.06.2023.
//

#ifndef MOONSHINE_MODELLOADER_H
#define MOONSHINE_MODELLOADER_H

#include <string>
#include <iostream>
#include "../external/stb/stb_image.h"
#include "../external/stb/stb_image_write.h"
#include "../external/tinygltf/tiny_gltf.h"


namespace moonshine {

    class ModelLoader {

    public:
        
        static bool loadASCIIModel(tinygltf::Model &model, const char* filepath);
    };

} // moonshine

#endif //MOONSHINE_MODELLOADER_H
