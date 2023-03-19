//
// Created by william on 2023/3/19.
//
#ifndef GRAPHICRENDERENGINE_FUNCHOOK_H
#define GRAPHICRENDERENGINE_FUNCHOOK_H

#include "commonMacro.h"
#include "fishhook.h"
#include "glCommonDefine.h"
#include <sstream>
#include <execinfo.h>
#include <stdlib.h>
#include <unistd.h>

#define WRAP_FUNC(func) warp_##func
#define ORIGIN_FUNC(func) origin_##func
#define HOOK_IMP(returnType, func, ...)                                   \
    inline static returnType (*ORIGIN_FUNC(func))(__VA_ARGS__) = nullptr; \
    static returnType WRAP_FUNC(func)(__VA_ARGS__)

#define HOOK(func) \
    rebind_symbols((struct rebinding[1]){ { #func, reinterpret_cast<void*>(WRAP_FUNC(func)), reinterpret_cast<void**>(reinterpret_cast<void*>(&ORIGIN_FUNC(func))) } }, 1)

class FuncHook
{
public:
    FuncHook();
    ~FuncHook();

    static void printStackTrace() {
        const int max_frames = 128;
        void *frames[max_frames];
        int num_frames = backtrace(frames, max_frames);
        char **symbols = backtrace_symbols(frames, num_frames);
        std::stringstream os;
        os << "Stack trace:" << std::endl;
        for (int i = 0; i < num_frames; i++) {
            os << symbols[i] << std::endl;
        }
        free(symbols);
        LOG_ERROR("{}", os.str());
    }

public:
    static void init()
    {
        printStackTrace();
        // opengl
        HOOK(glDrawArrays);
        HOOK(glDrawElements);
        HOOK(glTexImage2D);
        HOOK(glViewport);
    }

    HOOK_IMP(void, glDrawArrays, GLenum topology, GLint offset, GLsizei vertexCount)
    {
        ORIGIN_FUNC(glDrawArrays)
        (topology, offset, vertexCount);
        LOG_INFO("glDrawArrays({}, {}, {})", topology, offset, vertexCount);
    }

    HOOK_IMP(void, glDrawElements, GLenum topology, GLsizei count, GLenum type, const GLvoid* indices)
    {
        ORIGIN_FUNC(glDrawElements)
        (topology, count, type, indices);
        LOG_INFO("glDrawElements({}, {}, {}, {})", topology, count, type, indices);
    }

    HOOK_IMP(void, glTexImage2D, GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* data)
    {
        ORIGIN_FUNC(glTexImage2D)
        (target, level, internalFormat, width, height, border, format, type, data);
        LOG_INFO("glTexImage2D({}, {}, {}, {}, {}, {}, {}, {}, {})", target, level, internalFormat, width, height, border, format, type, data);
    }

    HOOK_IMP(void, glViewport, GLint x, GLint y, GLsizei width, GLsizei height)
    {
        ORIGIN_FUNC(glViewport)
        (x, y, width, height);
        LOG_INFO("glViewport({}, {}, {}, {})", x, y, width, height);
    }
};

#endif // GRAPHICRENDERENGINE_FUNCHOOK_H
