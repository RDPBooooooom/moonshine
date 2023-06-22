//
// Created by marvin on 22.06.2023.
//

#ifndef MOONSHINE_MODELLOADER_H
#define MOONSHINE_MODELLOADER_H

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE


#include <string>
#include <iostream>
#include "../external/stb/stb_image.h"
#include "../external/stb/stb_image_write.h"
#include "../external/tinygltf/tiny_gltf.h"

namespace moonshine {

    class ModelLoader {

    public:
        
        static bool loadASCIIModel(tinygltf::Model &model, const char* filepath){
            std::string err;
            std::string warn;
            tinygltf::TinyGLTF loader;

            bool res = loader.LoadASCIIFromFile(&model, &err, &warn, filepath);
            if (!warn.empty()) {
                std::cout << "WARN: " << warn << std::endl;
            }

            if (!err.empty()) {
                std::cout << "ERR: " << err << std::endl;
            }

            if (!res)
                std::cout << "Failed to load glTF: " << filepath << std::endl;
            else
                std::cout << "Loaded glTF: " << filepath << std::endl;

            return res;
        }
    };

} // moonshine

#endif //MOONSHINE_MODELLOADER_H
