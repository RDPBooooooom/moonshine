#include "GLShader.h"
#include "../../editor/EngineSystems.h"
#include "GLUtils.h"
#include <string>
#include <cassert>

namespace moonshine {

    //MoonshineTodo: Integrate readShaderFile with existing methods
    GLShader::GLShader(const char *fileName)
            : GLShader(GLShaderTypeFromFileName(fileName), readShaderFile(fileName).c_str(), fileName) {}

    GLShader::GLShader(GLenum type, const char *text, const char *debugFileName)
            : m_type(type), m_handle(glCreateShader(type)) {
        glShaderSource(m_handle, 1, &text, nullptr);
        glCompileShader(m_handle);

        char buffer[8192];
        GLsizei length = 0;
        glGetShaderInfoLog(m_handle, sizeof(buffer), &length, buffer);

        if (length) {
            EngineSystems::get_instance().get_logger()->error(LoggerType::Rendering,
                                                              "Unable to load program (shader) %s", debugFileName);
            assert(false);
        }
    }

    GLShader::~GLShader() {
        glDeleteShader(m_handle);
    }
}


