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
    explicit Renderer(GLFWwindow* window,  bool vsync = true);
    ~Renderer();
    // Update window with OpenGL rendering
    void swapWindow();
    // return stats of the last rendered frame,only data maintained by re is included
    inline const RenderStats& getRenderStats() const { return m_renderStatsLast; }
    inline bool usesVSync() const { return m_vsync;};
    glm::ivec2 getFramebufferSize();
    glm::ivec2 getWindowSize();

public:
    static constexpr int s_maxSceneLights{ 4 };
    static constexpr int s_rgVersionMajor{ 3 };
    static constexpr int s_rgVersionMinor{ 0 };
    static constexpr int s_rgVersionPoint{ 0 };
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
    bool m_vsync;

    friend class Mesh;
    friend class Mesh::MeshBuilder;
    friend class Shader;
    friend class Texture;
    friend class Camera;
    friend class RenderPass;
    friend class Inspector;
    friend class FrameBuffer;
    friend class SpriteAtlas;
};
} // namespace re
#endif //SIMPLERENDERENGINE_RENDERER_H