//
// Created by marvin on 20.05.2024.
//

#include "GLGuiRenderer.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"

namespace moonshine {


    GLGuiRenderer::~GLGuiRenderer() {
        ImGui::DestroyContext();
    }

    void GLGuiRenderer::new_frame() {
        {
            //EASY_BLOCK("ImGUI New Frame")
            ImGui::NewFrame();
            create_dock_space();
        }
    }

    void GLGuiRenderer::create_dock_space() {
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

        ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                        ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;


        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace", nullptr, window_flags);
        ImGui::PopStyleVar();
        ImGui::PopStyleVar(2);

        //  DockSpace
        ImGuiIO &io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

            static auto first_time = true;
            if (first_time) {
                first_time = false;

                ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
                ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
                ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

                // split the dockspace into 2 nodes -- DockBuilderSplitNode takes in the following args in the following order
                //   window ID to split, direction, fraction (between 0 and 1), the final two setting let's choose which id we want (which ever one we DON'T set as NULL, will be returned by the function)
                //                                                              out_id_at_dir is the id of the node in the direction we specified earlier, out_id_at_opposite_dir is in the opposite direction
                auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.25f, nullptr,
                                                                &dockspace_id);
                auto dock_id_down = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.25f, nullptr,
                                                                &dockspace_id);
                auto dock_id_right = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.25f, nullptr,
                                                                 &dockspace_id);

                // we now dock our windows into the docking node we made above
                ImGui::DockBuilderDockWindow("Scene Graph", dock_id_left);
                ImGui::DockBuilderDockWindow("Inspector", dock_id_right);
                ImGui::DockBuilderDockWindow("Workspace", dock_id_down);
                ImGui::DockBuilderDockWindow("Lobby manager", dock_id_down);
                ImGui::DockBuilderDockWindow("Logs", dock_id_down);
                ImGui::DockBuilderDockWindow("Stats", dock_id_down);

                ImGui::DockBuilderFinish(dockspace_id);
            }
        }

        ImGui::End();

    }

    void GLGuiRenderer::defaultInitImGui() {
        ImGui::CreateContext();

        ImGuiIO &io = ImGui::GetIO();
        io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
        io.BackendFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.BackendFlags |= ImGuiConfigFlags_DockingEnable;

        // Build texture atlas
        io.Fonts->AddFontDefault();

        io.DisplayFramebufferScale = ImVec2(1, 1);
    }

    // learn: Understand this
    void GLGuiRenderer::render_frame(int width, int height, const ImDrawData *draw_data) {
        if (!draw_data)
            return;

        glBindTextures(0, 1, &m_texture);
        glBindVertexArray(m_vao);
        m_program.useProgram();

        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_SCISSOR_TEST);

        const float L = draw_data->DisplayPos.x;
        const float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
        const float T = draw_data->DisplayPos.y;
        const float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
        const glm::mat4 orthoProjection = glm::ortho(L, R, B, T);

        glNamedBufferSubData(m_per_frame_data_buffer.getHandle(), 0, sizeof(glm::mat4), glm::value_ptr(orthoProjection));

        for (int n = 0; n < draw_data->CmdListsCount; n++) {
            const ImDrawList *cmd_list = draw_data->CmdLists[n];
            glNamedBufferSubData(m_vertices.getHandle(), 0, (GLsizeiptr) cmd_list->VtxBuffer.Size * sizeof(ImDrawVert),
                                 cmd_list->VtxBuffer.Data);
            glNamedBufferSubData(m_elements.getHandle(), 0, (GLsizeiptr) cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx),
                                 cmd_list->IdxBuffer.Data);

            for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
                const ImDrawCmd *pcmd = &cmd_list->CmdBuffer[cmd_i];
                const ImVec4 cr = pcmd->ClipRect;
                glScissor((int) cr.x, (int) (height - cr.w), (int) (cr.z - cr.x), (int) (cr.w - cr.y));
                glBindTextureUnit(0, (GLuint) (intptr_t) pcmd->TextureId);
                glDrawElementsBaseVertex(GL_TRIANGLES, (GLsizei) pcmd->ElemCount, GL_UNSIGNED_SHORT,
                                         (void *) (intptr_t) (pcmd->IdxOffset * sizeof(ImDrawIdx)),
                                         (GLint) pcmd->VtxOffset);
            }
        }

        glScissor(0, 0, width, height);
        glDisable(GL_SCISSOR_TEST);
    }
} // moonshine