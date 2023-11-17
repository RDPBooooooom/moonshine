//
// Created by marvin on 17.11.2023.
//

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE

#include "ModelLoader.h"


namespace moonshine {

    bool ModelLoader::loadASCIIModel(tinygltf::Model &model, const char *filepath) {

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
}