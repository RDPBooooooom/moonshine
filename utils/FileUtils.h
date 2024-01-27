//
// Created by marvin on 29.05.2023.
//

#ifndef MOONSHINE_FILEUTILS_H
#define MOONSHINE_FILEUTILS_H

#include <vector>
#include <fstream>
#include <minwindef.h>
#include <libloaderapi.h>
#include <filesystem>

namespace moonshine {

    class FileUtils {

    public:
        static std::vector<char> read_file(const std::string &filename) {
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

        static std::string get_executable_path() {
            char buffer[MAX_PATH];
            GetModuleFileName(NULL, buffer, MAX_PATH);
            std::string::size_type pos = std::string(buffer).find_last_of("\\/");
            return std::string(buffer).substr(0, pos);
        }

        static std::string get_relative_path(const std::string &file_path, const std::string &workspace_path) {
            // Check if the file path actually starts with the workspace path
            if (file_path.substr(0, workspace_path.size()) == workspace_path) {
                // Return the substring of filePath that comes after workspacePath
                // Adding 1 to remove the leading '/' or '\' character
                return file_path.substr(workspace_path.size() + 1);
            } else {
                // The filePath does not start with the workspacePath, return the original filePath
                return file_path;
            }
        }

        static std::filesystem::path get_user_root_directory() {
            const char *home_dir = std::getenv("USERPROFILE");
            if (home_dir != nullptr)
                return std::filesystem::path(home_dir);
            else
                throw std::runtime_error("Cannot find user root directory");
        }

        static std::filesystem::path get_downloads_directory() {
            const char *home_dir = std::getenv("USERPROFILE");
            if (home_dir != nullptr)
                return std::filesystem::path(home_dir) / "Downloads";
            else
                throw std::runtime_error("Cannot find downloads directory");
        }
    };

} // moonshine

#endif //MOONSHINE_FILEUTILS_H
