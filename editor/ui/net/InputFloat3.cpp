//
// Created by marvin on 01.12.2023.
//

#include "InputFloat3.h"

namespace moonshine::net::ui {

    bool InputFloat3(const char *label, float *v) {
        {
            bool should_disable = EngineSystems::get_instance().get_ui_manager()->is_locked(label);
            ImGui::BeginDisabled(should_disable);
            
            bool is_changed = ImGui::InputFloat3(label, v);

            if (ImGui::IsItemActive()) {
                std::string name = std::string(label);
                EngineSystems::get_instance().get_ui_manager()->register_field(name, element_owner::self);
            }

            ImGui::EndDisabled();

            return is_changed;
        }
    }
} // ui