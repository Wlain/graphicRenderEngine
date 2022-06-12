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
    // Update window with OpenGL rendering
    void swapWindow();
    // return stats of the last rendered frame,only data maintained by re is included
    inline const RenderStats& getRenderStats() const { return m_renderStatsLast; }
    glm::ivec2 getFramebufferSize();
    glm::ivec2 getWindowSize();

public:
    static constexpr int s_maxSceneLights{ 4 };
    static constexpr int s_rgVersionMajor{ 2 };
    static constexpr int s_rgVersionMinor{ 4 };
    static constexpr int s_rgVersionPoint{ 2 };
    inline static Renderer* s_instance{ nullptr };

private:
    RenderStats m_renderStatsLast{};
    RenderStats m_renderStatsCurrent{};
    GLFWwindow* m_window{ nullptr };
    std::vector<FrameBuffer*> m_fbos;
    std::vector<Mesh*> m_meshes;
    std::vector<Shader*> m_shaders;
    std::vector<Texture*> m_textures;
    std::vector<SpriteAtlas*> m_spriteAtlases;

    friend class Mesh;
    friend class Mesh::MeshBuilder;
    friend class Shader;
    friend class Shader;
    friend class Texture;
    friend class Camera;
    friend class RenderPass;
    friend class Profiler;
    friend class FrameBuffer;
    friend class SpriteAtlas;
};
} // namespace re
#endif //SIMPLERENDERENGINE_RENDERER_H