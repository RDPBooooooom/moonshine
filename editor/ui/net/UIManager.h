//
// Created by marvin on 01.12.2023.
//

#ifndef MOONSHINE_UIMANAGER_H
#define MOONSHINE_UIMANAGER_H

#include <string>
#include <boost/json/value.hpp>
#include "unordered_map"
#include "mutex"

namespace moonshine {

    enum element_owner {
        self,
        other,
        none
    };

    struct element_locker {
        std::chrono::high_resolution_clock::time_point last_update = std::chrono::high_resolution_clock::now();
        element_owner owner = element_owner::none;
        bool lock = true;
        std::chrono::high_resolution_clock::time_point last_replication = std::chrono::high_resolution_clock::time_point::min();
    };

    class UIManager {

    private:

        std::unordered_map<std::string, element_locker> uiElements;
        std::mutex uiMap;

        float since_last_updated = 0;
        int rate_limit = 150;

    public:

        void update();

        void register_field(std::string &lable, element_owner owner);

        void register_field(std::string &lable, element_owner owner, bool notify);

        void register_field(std::string &label, element_locker locker, bool notify);

        bool is_locked(const std::string &label);

    private:
        void register_known_field(const std::string &label, element_locker locker);

        bool check_rate_limit(element_locker &locker);
    };

} // moonshine

namespace boost::json {
    value to_value(const moonshine::element_locker &locker);

    moonshine::element_locker from_value(const value &jval);
}

#endif //MOONSHINE_UIMANAGER_H
