//
// Created by marvin on 17.11.2023.
//

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE

#include "ModelLoader.h"
#include "EngineSystems.h"


namespace moonshine {

    bool ModelLoader::loadASCIIModel(tinygltf::Model &model, const char *filepath) {

        std::string err;
        std::string warn;
        tinygltf::TinyGLTF loader;

        auto logger = EngineSystems::getInstance().get_logger();
        bool res = loader.LoadASCIIFromFile(&model, &err, &warn, filepath);
        if (!warn.empty()) {
            logger->warn(LoggerType::Editor, warn);
        }

        if (!err.empty()) {
            logger->error(LoggerType::Editor, err);
        }

        if (!res)
            logger->error(LoggerType::Editor, std::string("Failed to load glTF: ") + filepath);
        else
            logger->info(LoggerType::Editor, std::string("Loaded glTF: ") + filepath);

        return res;
    }
}