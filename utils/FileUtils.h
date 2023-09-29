﻿//
// Created by marvin on 29.05.2023.
//

#ifndef MOONSHINE_FILEUTILS_H
#define MOONSHINE_FILEUTILS_H

#include <vector>
#include <fstream>
#include <minwindef.h>
#include <libloaderapi.h>

namespace moonshine {

    class FileUtils {
        
    public:
        static std::vector<char> readFile(const std::string &filename) {
            std::ifstream file(filename, std::ios::ate | std::ios::binary);
            if (!file.is_open()) {
                throw std::runtime_error("failed to open file!");
            }

            size_t fileSize = (size_t) file.tellg();
            std::vector<char> buffer(fileSize);

            file.seekg(0);
            file.read(buffer.data(), fileSize);

            if (file.fail()) {
                throw std::runtime_error("failed to read file!");
            }

            file.close();

            return buffer;
        }

        static std::string getExecutablePath() {
            char buffer[MAX_PATH];
            GetModuleFileName(NULL, buffer, MAX_PATH);
            std::string::size_type pos = std::string(buffer).find_last_of("\\/");
            return std::string(buffer).substr(0, pos);
        }
    };

} // moonshine

#endif //MOONSHINE_FILEUTILS_H
