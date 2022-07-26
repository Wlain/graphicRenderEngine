//
// Created by william on 2022/5/26.
//

#ifndef GRAPHICRENDERENGINE_RENDERSTATS_H
#define GRAPHICRENDERENGINE_RENDERSTATS_H

namespace ceres
{
struct RenderStats
{
    int frame{ 0 };
    int meshCount{ 0 };
    int meshBytes{ 0 };
    int textureCount{ 0 };
    int textureBytes{ 0 };
    int shaderCount{ 0 };
    int drawCalls{ 0 };
    int stateChangesShader{ 0 };
    int stateChangesMaterial{ 0 };
    int stateChangesMesh{ 0 };
    int textureBytesAllocated = 0;   // size of allocated textures in bytes this frame
    int textureBytesDeallocated = 0; // size of deallocated textures in bytes this frame
    int meshBytesAllocated = 0;      // size of allocated meshes in bytes this frame
    int meshBytesDeallocated = 0;    // size of deallocated meshes in bytes this frame
};
} // namespace ceres
#endif // GRAPHICRENDERENGINE_RENDERSTATS_H
