//
// Created by cwb on 2022/8/25.
//

#ifndef GRAPHICRENDERENGINE_CONTEXTGL_H
#define GRAPHICRENDERENGINE_CONTEXTGL_H
#include "context.h"
#include "glCommonDefine.h"
namespace ceres
{
class ContextGL : public Context
{
public:
    struct GLInfo
    {
        GLint maxDrawBuffers;
        GLint maxRenderbufferSize;
        GLint maxSamples;
        GLint maxUniformBlockSize;
        GLint maxTextureImageUnits;
        GLint uniformBufferOffsetAlignment;
    };
};
} // namespace ceres

#endif // GRAPHICRENDERENGINE_CONTEXTGL_H
