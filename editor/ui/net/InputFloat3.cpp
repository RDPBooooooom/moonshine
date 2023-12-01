//
// Created by marvin on 01.12.2023.
//

#include "InputFloat3.h"

namespace moonshine::net::ui {

    bool InputFloat3(const char *label, float *v) {
        {
            bool shouldDisable = EngineSystems::getInstance().get_ui_manager()->is_locked(label);
            ImGui::BeginDisabled(shouldDisable);
            
            bool isChanged = ImGui::InputFloat3(label, v);

            if (ImGui::IsItemActive()) {
                std::string name = std::string(label);
                EngineSystems::getInstance().get_ui_manager()->register_field(name, element_owner::self);
            }

            ImGui::EndDisabled();

            return isChanged;
        }
    }
} // ui