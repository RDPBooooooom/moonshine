//
// Created by marvin on 14.09.2023.
//

#ifndef MOONSHINE_SETTINGS_H
#define MOONSHINE_SETTINGS_H

#include <boost/json.hpp>
#include "spdlog/spdlog.h"

struct Settings {
    bool ENABLE_MOUSE_DEBUG = false;

    spdlog::level::level_enum EDITOR_LOG_LEVEL_CONSOLE = spdlog::level::info;
    spdlog::level::level_enum NETWORKING_LOG_LEVEL_CONSOLE = spdlog::level::info;
    spdlog::level::level_enum RENDERING_LOG_LEVEL_CONSOLE = spdlog::level::info;

    spdlog::level::level_enum EDITOR_LOG_LEVEL_UI = spdlog::level::warn;
    spdlog::level::level_enum NETWORKING_LOG_LEVEL_UI = spdlog::level::warn;
    spdlog::level::level_enum RENDERING_LOG_LEVEL_UI = spdlog::level::warn;

    std::string LATEST_WORKSPACE = ".";

    static std::string to_string(spdlog::level::level_enum log_level) {
        static std::map<spdlog::level::level_enum, std::string> level_map = {
                {spdlog::level::trace,    "trace"},
                {spdlog::level::debug,    "debug"},
                {spdlog::level::info,     "info"},
                {spdlog::level::warn,     "warn"},
                {spdlog::level::err,      "err"},
                {spdlog::level::critical, "critical"},
                {spdlog::level::off,      "off"}
        };
        return level_map[log_level];
    }

    static spdlog::level::level_enum to_enum(const std::string& log_level) {
        static std::map<std::string, spdlog::level::level_enum> level_map = {
                {"trace",     spdlog::level::trace},
                {"debug",     spdlog::level::debug},
                {"info",      spdlog::level::info},
                {"warn",      spdlog::level::warn},
                {"err",       spdlog::level::err},
                {"critical",  spdlog::level::critical},
                {"off",       spdlog::level::off}
        };

        return level_map[log_level];
    }

    static std::string serialize(const Settings &settings) {
        boost::json::object settings_json = {
                {"ENABLE_MOUSE_DEBUG", settings.ENABLE_MOUSE_DEBUG},
                {"LOGGING",
                                       {
                                               {"EDITOR",
                                                       {
                                                               {"LOG_LEVEL_CONSOLE", to_string(
                                                                       settings.EDITOR_LOG_LEVEL_CONSOLE)},
                                                               {"LOG_LEVEL_UI", to_string(
                                                                       settings.EDITOR_LOG_LEVEL_UI)}
                                                       }
                                               }, {"NETWORKING",
                                                          {
                                                                  {"LOG_LEVEL_CONSOLE", to_string(
                                                                          settings.NETWORKING_LOG_LEVEL_CONSOLE)},
                                                                  {"LOG_LEVEL_UI", to_string(
                                                                          settings.NETWORKING_LOG_LEVEL_UI)}
                                                          }
                                                  }, {"RENDERING",
                                                             {
                                                                     {"LOG_LEVEL_CONSOLE", to_string(
                                                                             settings.RENDERING_LOG_LEVEL_CONSOLE)},
                                                                     {"LOG_LEVEL_UI", to_string(
                                                                             settings.RENDERING_LOG_LEVEL_UI)}
                                                             }
                                                     }
                                       }
                },
                {"LATEST_WORKSPACE",   settings.LATEST_WORKSPACE}
        };
        return boost::json::serialize(settings_json);
    }

    static Settings deserialize(const std::string& json_data) {
        Settings settings = {};
        boost::json::object settings_json = boost::json::parse(json_data).as_object();

        settings.ENABLE_MOUSE_DEBUG = settings_json["ENABLE_MOUSE_DEBUG"].as_bool();

        boost::json::object log_settings = settings_json["LOGGING"].as_object();

        // Editor settings
        boost::json::object editor_settings = log_settings["EDITOR"].as_object();
        settings.EDITOR_LOG_LEVEL_CONSOLE = to_enum(editor_settings["LOG_LEVEL_CONSOLE"].as_string().c_str());
        settings.EDITOR_LOG_LEVEL_UI = to_enum(editor_settings["LOG_LEVEL_UI"].as_string().c_str());

        // Networking settings
        boost::json::object networking_settings = log_settings["NETWORKING"].as_object();
        settings.NETWORKING_LOG_LEVEL_CONSOLE = to_enum(networking_settings["LOG_LEVEL_CONSOLE"].as_string().c_str());
        settings.NETWORKING_LOG_LEVEL_UI = to_enum(networking_settings["LOG_LEVEL_UI"].as_string().c_str());

        // Rendering settings
        boost::json::object rendering_settings = log_settings["RENDERING"].as_object();
        settings.RENDERING_LOG_LEVEL_CONSOLE = to_enum(rendering_settings["LOG_LEVEL_CONSOLE"].as_string().c_str());
        settings.RENDERING_LOG_LEVEL_UI = to_enum(rendering_settings["LOG_LEVEL_UI"].as_string().c_str());

        settings.LATEST_WORKSPACE = settings_json["LATEST_WORKSPACE"].as_string().c_str();

        return settings;
    }
};


#endif //MOONSHINE_SETTINGS_H
