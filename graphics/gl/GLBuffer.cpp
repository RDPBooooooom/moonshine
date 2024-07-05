//
// Created by marvin on 04.07.2024.
//

#include "GLBuffer.h"

namespace moonshine {
    GLBuffer::GLBuffer(GLsizeiptr size, const void *data, GLbitfield flags) {
        glCreateBuffers(1, &m_handle);
        glNamedBufferStorage(m_handle, size, data, flags);
    }

    GLBuffer::~GLBuffer() {
        glDeleteBuffers(1, &m_handle);
    }

} // moonshine