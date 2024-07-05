//
// Created by marvin on 20.05.2024.
//

#ifndef MOONSHINE_GLGuiRenderer_H
#define MOONSHINE_GLGuiRenderer_H

#include "imgui.h"
#include "imgui_internal.h"
#include "../../deps/glad/include/glad/gl.h"
#include "GLBuffer.h"
#include "GLShader.h"
#include "GLProgram.h"
#include "glm/glm.hpp"

namespace moonshine {

    // MoonshineTodo: Place in shaders folder
    const GLchar *shaderCodeImGuiVertex = R"(
	#version 460 core
	layout (location = 0) in vec2 Position;
	layout (location = 1) in vec2 UV;
	layout (location = 2) in vec4 Color;
	layout(std140, binding = 7) uniform PerFrameData
	{
		uniform mat4 MVP;
	};
	out vec2 Frag_UV;
	out vec4 Frag_Color;
	void main()
	{
		Frag_UV = UV;
		Frag_Color = Color;
		gl_Position = MVP * vec4(Position.xy,0,1);
	}
)";

    const GLchar *shaderCodeImGuiFragment = R"(
	#version 460 core
	in vec2 Frag_UV;
	in	vec4 Frag_Color;
	layout (binding = 0) uniform sampler2D Texture;
	layout (location = 0) out vec4 out_FragColor;
	void main()
	{
		out_FragColor = Frag_Color * texture(Texture, Frag_UV.st);
	}
)";

    class GLGuiRenderer {

        // Learn: Read about how this works
    private:
        GLuint m_texture = 0;
        GLuint m_vao = 0;
        GLBuffer m_vertices = GLBuffer(128 * 1024, nullptr, GL_DYNAMIC_STORAGE_BIT);
        GLBuffer m_elements = GLBuffer(256 * 1024, nullptr, GL_DYNAMIC_STORAGE_BIT);
        GLShader m_vertex = GLShader(GL_VERTEX_SHADER, shaderCodeImGuiVertex);
        GLShader m_fragment = GLShader(GL_FRAGMENT_SHADER, shaderCodeImGuiFragment);
        GLProgram m_program = GLProgram(m_vertex, m_fragment);
        GLBuffer m_per_frame_data_buffer = GLBuffer(sizeof(glm::mat4), nullptr, GL_DYNAMIC_STORAGE_BIT);


    public:
        explicit GLGuiRenderer() {
            glCreateVertexArrays(1, &m_vao);

            glVertexArrayElementBuffer(m_vao, m_elements.getHandle());
            glVertexArrayVertexBuffer(m_vao, 0, m_vertices.getHandle(), 0, sizeof(ImDrawVert));

            glEnableVertexArrayAttrib(m_vao, 0);
            glEnableVertexArrayAttrib(m_vao, 1);
            glEnableVertexArrayAttrib(m_vao, 2);

            glVertexArrayAttribFormat(m_vao, 0, 2, GL_FLOAT, GL_FALSE, IM_OFFSETOF(ImDrawVert, pos));
            glVertexArrayAttribFormat(m_vao, 1, 2, GL_FLOAT, GL_FALSE, IM_OFFSETOF(ImDrawVert, uv));
            glVertexArrayAttribFormat(m_vao, 2, 4, GL_UNSIGNED_BYTE, GL_TRUE, IM_OFFSETOF(ImDrawVert, col));

            glVertexArrayAttribBinding(m_vao, 0, 0);
            glVertexArrayAttribBinding(m_vao, 1, 0);
            glVertexArrayAttribBinding(m_vao, 2, 0);

            glBindBufferBase(GL_UNIFORM_BUFFER, 7, m_per_frame_data_buffer.getHandle());

            defaultInitImGui();
        }

        ~GLGuiRenderer();

        void new_frame();

        void render_frame(int width, int height, const ImDrawData *draw_data);

        void create_dock_space();

    private:
        void defaultInitImGui();
    };

} // moonshine

#endif //MOONSHINE_GLGuiRenderer_H
