
#ifndef MOONSHINE_GLShader_H
#define MOONSHINE_GLShader_H


#include "../../deps/glad/include/glad/gl.h"

namespace moonshine{

    class GLShader {
    public:
        explicit GLShader(const char *fileName);

        GLShader(GLenum type, const char *text, const char *debugFileName = "");

        ~GLShader();

        GLenum getType() const { return m_type; }

        GLuint getHandle() const { return m_handle; }

    private:
        GLenum m_type;
        GLuint m_handle;
    };
}

#endif