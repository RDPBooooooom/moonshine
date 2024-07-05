//
// Created by marvin on 04.07.2024.
//

#include "GLProgram.h"

namespace moonshine {

    GLProgram::GLProgram(const GLShader& a)
            : m_handle(glCreateProgram())
    {
        glAttachShader(m_handle, a.getHandle());
        glLinkProgram(m_handle);
    }

    GLProgram::GLProgram(const GLShader& a, const GLShader& b)
            : m_handle(glCreateProgram())
    {
        glAttachShader(m_handle, a.getHandle());
        glAttachShader(m_handle, b.getHandle());
        glLinkProgram(m_handle);
    }

    GLProgram::GLProgram(const GLShader& a, const GLShader& b, const GLShader& c)
            : m_handle(glCreateProgram())
    {
        glAttachShader(m_handle, a.getHandle());
        glAttachShader(m_handle, b.getHandle());
        glAttachShader(m_handle, c.getHandle());
        glLinkProgram(m_handle);
    }

    GLProgram::GLProgram(const GLShader& a, const GLShader& b, const GLShader& c, const GLShader& d, const GLShader& e)
            : m_handle(glCreateProgram())
    {
        glAttachShader(m_handle, a.getHandle());
        glAttachShader(m_handle, b.getHandle());
        glAttachShader(m_handle, c.getHandle());
        glAttachShader(m_handle, d.getHandle());
        glAttachShader(m_handle, e.getHandle());
        glLinkProgram(m_handle);
    }

    GLProgram::~GLProgram()
    {
        glDeleteProgram(m_handle);
    }

    void GLProgram::useProgram() const
    {
        glUseProgram(m_handle);
    }
    
} // moonshine