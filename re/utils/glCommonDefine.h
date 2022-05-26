//
// Created by william on 2022/5/24.
//

#ifndef SIMPLERENDERENGINE_GLCOMMONDEFINE_H
#define SIMPLERENDERENGINE_GLCOMMONDEFINE_H
#include "commonMacro.h"
#if defined __APPLE__
    #include <OpenGL/gl3.h>
#else

#endif

// For internal debugging of gl errors
inline void checkGLError()
{
    for (GLenum err; (err = glGetError()) != GL_NO_ERROR;)
    {
        switch (err)
        {
        case GL_INVALID_ENUM:
            LOG_ERROR("GL_INVALID_ENUM");
            break;
        case GL_INVALID_VALUE:
            LOG_ERROR("GL_INVALID_VALUE");
            break;
        case GL_INVALID_OPERATION:
            LOG_ERROR("GL_INVALID_OPERATION");
            break;
        case GL_OUT_OF_MEMORY:
            LOG_ERROR("GL_OUT_OF_MEMORY");
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            LOG_ERROR("GL_INVALID_FRAMEBUFFER_OPERATION");
            break;
        }
    }
}

#endif //SIMPLERENDERENGINE_GLCOMMONDEFINE_H
