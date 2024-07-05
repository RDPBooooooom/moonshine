//
// Created by marvin on 04.07.2024.
//

#ifndef MOONSHINE_GLPROGRAM_H
#define MOONSHINE_GLPROGRAM_H

#include "GLShader.h"
#include "../../deps/glad/include/glad/gl.h"

namespace moonshine {

    class GLProgram
    {
    public:
        explicit GLProgram(const GLShader& a);
        GLProgram(const GLShader& a, const GLShader& b);
        GLProgram(const GLShader& a, const GLShader& b, const GLShader& c);
        GLProgram(const GLShader& a, const GLShader& b, const GLShader& c, const GLShader& d, const GLShader& e);
        ~GLProgram();

        void useProgram() const;
        GLuint getHandle() const { return m_handle; }

    private:
        GLuint m_handle;
    };

} // moonshine

#endif //MOONSHINE_GLPROGRAM_H
