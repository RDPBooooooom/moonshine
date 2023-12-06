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

    std::string LOBBY_SERVER_ADDRESS = "booooooom.ch";
    std::string LOBBY_SERVER_PORT = "12000";

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

    static spdlog::level::level_enum to_enum(const std::string &log_level) {
        static std::map<std::string, spdlog::level::level_enum> level_map = {
                {"trace",    spdlog::level::trace},
                {"debug",    spdlog::level::debug},
                {"info",     spdlog::level::info},
                {"warn",     spdlog::level::warn},
                {"err",      spdlog::level::err},
                {"critical", spdlog::level::critical},
                {"off",      spdlog::level::off}
        };

        return level_map[log_level];
    }

    static std::string serialize(const Settings &settings) {
        boost::json::object settings_json = {
                {"ENABLE_MOUSE_DEBUG",   settings.ENABLE_MOUSE_DEBUG},
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
                {"LATEST_WORKSPACE",     settings.LATEST_WORKSPACE},
                {"LOBBY_SERVER_ADDRESS", settings.LOBBY_SERVER_ADDRESS},
                {"LOBBY_SERVER_PORT",    settings.LOBBY_SERVER_PORT}
        };
        return boost::json::serialize(settings_json);
    }

    static Settings deserialize(const std::string &json_data) {
        Settings settings = {};
        boost::json::object settings_json = boost::json::parse(json_data).as_object();

        load_bool(settings.ENABLE_MOUSE_DEBUG, settings_json, "ENABLE_MOUSE_DEBUG");

        if (settings_json.contains("LOGGING")) {
            boost::json::object log_settings = settings_json["LOGGING"].as_object();

            if (log_settings.contains("EDITOR")) {
                // Editor settings
                boost::json::object editor_settings = log_settings["EDITOR"].as_object();
                load_enum(settings.EDITOR_LOG_LEVEL_CONSOLE, editor_settings, "LOG_LEVEL_CONSOLE");
                load_enum(settings.EDITOR_LOG_LEVEL_UI, editor_settings, "LOG_LEVEL_UI");
            }

            if (log_settings.contains("NETWORKING")) {
                // Networking settings
                boost::json::object networking_settings = log_settings["NETWORKING"].as_object();
                load_enum(settings.NETWORKING_LOG_LEVEL_CONSOLE, networking_settings, "LOG_LEVEL_CONSOLE");
                load_enum(settings.NETWORKING_LOG_LEVEL_UI, networking_settings, "LOG_LEVEL_UI");
            }

            if (log_settings.contains("RENDERING")) {
                // Rendering settings
                boost::json::object rendering_settings = log_settings["RENDERING"].as_object();
                load_enum(settings.RENDERING_LOG_LEVEL_CONSOLE, rendering_settings, "LOG_LEVEL_CONSOLE");
                load_enum(settings.RENDERING_LOG_LEVEL_UI, rendering_settings, "LOG_LEVEL_UI");
            }
        }

        load_string(settings.LATEST_WORKSPACE, settings_json, "LATEST_WORKSPACE");

        load_string(settings.LOBBY_SERVER_ADDRESS, settings_json, "LOBBY_SERVER_ADDRESS");
        load_string(settings.LOBBY_SERVER_PORT, settings_json, "LOBBY_SERVER_PORT");

        return settings;
    }

    static void load_bool(bool &setting, boost::json::object &settings_json, std::string setting_name) {
        if (settings_json.contains(setting_name)) {
            setting = settings_json[setting_name].as_bool();
        }
    }

    static void load_string(std::string &setting, boost::json::object &settings_json, std::string setting_name) {
        if (settings_json.contains(setting_name)) {
            setting = settings_json[setting_name].as_string().c_str();
        }
    }

    static void
    load_enum(spdlog::level::level_enum &setting, boost::json::object &settings_json, std::string setting_name) {
        if (settings_json.contains(setting_name)) {
            setting = to_enum(
                    settings_json[setting_name].as_string().c_str());
        }
    }
};


#endif //MOONSHINE_SETTINGS_H
