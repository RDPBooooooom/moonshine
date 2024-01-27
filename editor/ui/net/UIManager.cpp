//
// Created by marvin on 01.12.2023.
//

#include "UIManager.h"
#include "../../Time.h"
#include "../../EngineSystems.h"

namespace boost::json {

    std::chrono::high_resolution_clock::time_point from_string(const value &jval) {
        std::chrono::high_resolution_clock::time_point tp{std::chrono::milliseconds(jval.as_int64())};
        return tp;
    }

    value to_string(std::chrono::high_resolution_clock::time_point tp) {
        auto sec = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()).count();
        return value(sec);
    }

// Conversions for element_owner
    value to_value(const moonshine::element_owner &owner) {
        return value(static_cast<int>(owner));
    }

    moonshine::element_owner from_value_to_element_owner(const value &jval) {
        return static_cast<moonshine::element_owner>(jval.get_int64());
    }

// Conversions for element_locker
    value to_value(const moonshine::element_locker &locker) {
        object obj;
        obj["last_update"] = to_string(locker.last_update);
        obj["owner"] = to_value(locker.owner);
        obj["lock"] = value(locker.lock);
        return value(obj);
    }

    moonshine::element_locker from_value(const value &jval) {
        auto obj = jval.get_object();
        moonshine::element_locker locker;
        locker.last_update = from_string(obj["last_update"]);
        locker.owner = from_value_to_element_owner(obj["owner"]);
        locker.lock = obj["lock"].get_bool();
        return locker;
    }

}

namespace moonshine {
    void UIManager::update() {

        m_since_last_updated += Time::s_delta_time;

        if (m_since_last_updated > 0.4f) {
            m_since_last_updated = 0;

            auto lock = std::scoped_lock<std::mutex>(m_ui_map);
            auto end = std::chrono::system_clock::now();

            for (std::pair<const std::basic_string<char>, element_locker> &item: m_ui_elements) {
                auto duration = end - item.second.last_update;
                auto duration_since_update = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
                if (item.second.lock && duration_since_update.count() > 1000) {
                    EngineSystems::get_instance().get_logger()->debug(LoggerType::Editor,
                                                                      "{} was unlocked", item.first);
                    item.second.lock = false;
                }
            }
        }
    }

    void UIManager::register_field(std::string &label, element_owner owner) {
        register_field(label, owner, true);
    }

    void UIManager::register_field(std::string &label, element_owner owner, bool notify) {

        auto locker = element_locker();
        locker.owner = owner;

        register_field(label, locker, notify);
    }

    void UIManager::register_field(std::string &label, element_locker locker, bool notify) {
        auto lock = std::scoped_lock<std::mutex>(m_ui_map);
        if (m_ui_elements.find(label) != m_ui_elements.end()) {
            register_known_field(label, locker);
        } else {
            m_ui_elements[label] = locker;
        }

        if (notify && check_rate_limit(m_ui_elements[label])) {
            EngineSystems::get_instance().get_lobby_manager()->replicate_ui(label, m_ui_elements[label]);
        }

        EngineSystems::get_instance().get_logger()->debug(LoggerType::Editor, "Locked {}", label);
    }

    void UIManager::register_known_field(const std::string &label, element_locker locker) {

        element_locker &current = m_ui_elements.at(label);

        if (current.owner == self && current.lock) return;

        if (current.lock) {
            if (current.last_update < locker.last_update) {
                current.last_update = locker.last_update;
            }
        } else {
            m_ui_elements[label] = locker;
        }
    }

    bool UIManager::is_locked(const std::string &label) {
        if (m_ui_elements.find(label) == m_ui_elements.end()) return false;

        element_locker locker = m_ui_elements.at(label);

        return locker.lock && locker.owner != self;
    }

    bool UIManager::check_rate_limit(element_locker &locker) {

        if(locker.last_replication == std::chrono::high_resolution_clock::time_point::min()){
            return true;
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - locker.last_replication);

        EngineSystems::get_instance().get_logger()->debug(LoggerType::Editor, "Replication duration {}", std::to_string(duration.count()));
        
        return duration.count() > m_rate_limit;
    }

} // moonshine