//
// Created by william on 2022/5/22.
//

#ifndef SIMPLERENDERENGINE_RENDERER_H
#define SIMPLERENDERENGINE_RENDERER_H
#include "camera.h"
#include "glCommonDefine.h"
#include "light.h"
#include "mesh.h"
#include "renderPass.h"
#include "renderStats.h"

#include <algorithm>
#include <glm/glm.hpp>
namespace re
{
// 前向声明
class Mesh;
class Shader;
class ParticleMesh;

class Renderer
{
public:
    explicit Renderer(GLFWwindow* window);
    ~Renderer();
    RenderPass::RenderPassBuilder createRenderPass();
    // Update window with OpenGL rendering
    void swapWindow();
    // flush GPU command buffer (must be called when profiling GPU time - should not be called when not profiling)
    inline void finishGPUCommandBuffer() const
    {
        glFlush();
    }
    // return stats of the last rendered frame,only data maintained by re is included
    inline const RenderStats& getRenderStats() const { return m_renderStatsLast; }
    glm::ivec2 getWindowSize();

public:
    static constexpr int s_maxSceneLights{ 4 };
    static constexpr int s_rgVersionMajor{ 2 };
    static constexpr int s_rgVersionMinor{ 2 };
    static constexpr int s_rgVersionPoint{ 0 };
    inline static Renderer* s_instance{ nullptr };

private:
    RenderStats m_renderStatsLast{};
    RenderStats m_renderStatsCurrent{};
    GLFWwindow* m_window{ nullptr };

    friend class Mesh;
    friend class Mesh::MeshBuilder;
    friend class Shader;
    friend class Shader;
    friend class Texture;
    friend class Camera;
    friend class RenderPass;
};
} // namespace re
#endif //SIMPLERENDERENGINE_RENDERER_H