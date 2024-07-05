//
// Created by marvin on 04.07.2024.
//

#ifndef MOONSHINE_GLBUFFER_H
#define MOONSHINE_GLBUFFER_H


#include "../../deps/glad/include/glad/gl.h"

namespace moonshine {

    class GLBuffer {
    public:
        GLBuffer(GLsizeiptr size, const void *data, GLbitfield flags);

        ~GLBuffer();

        GLuint getHandle() const { return m_handle; }

    private:
        GLuint m_handle;
    };

} // moonshine

#endif //MOONSHINE_GLBUFFER_H
