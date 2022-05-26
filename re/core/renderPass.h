//
// Created by william on 2022/5/26.
//

#ifndef SIMPLERENDERENGINE_RENDERPASS_H
#define SIMPLERENDERENGINE_RENDERPASS_H
#include "camera.h"
#include "mesh.h"

#include <string>
namespace re
{
class Renderer;
class Shader;
class RenderStats;
//渲染通道封装了一些渲染状态，并允许添加 drawcall，注意，一次只能有一个渲染传递对象处于活动状态。
class RenderPass
{
public:
    class RenderPassBuilder
    {
    public:
        RenderPassBuilder& withName(const std::string& name);
        RenderPassBuilder& withCamera(const Camera& camera);
        RenderPassBuilder& withWorldLights(WorldLights* worldLights);
        RenderPass build();

    private:
        RenderPassBuilder(RenderStats* renderStats);
        RenderPassBuilder() = default;

    private:
        std::string m_name;
        WorldLights* m_worldLights{ nullptr };
        RenderStats* m_renderStats{ nullptr };
        Camera m_camera;
        friend class Renderer;
        friend class RenderPass;
    };

public:
    RenderPass() = default;
    static RenderPassBuilder create();
    virtual ~RenderPass();
    void clearScreen(glm::vec4 color, bool clearColorBuffer = true, bool clearDepthBuffer = true, bool clearStencil = false);
    void drawLines(const std::vector<glm::vec3>& vertices, glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f }, Mesh::Topology meshTopology = Mesh::Topology::LineStrip);
    void draw(Mesh* mesh, glm::mat4 modelTransform, Shader* shader);

private:
    RenderPass(Camera&& camera, WorldLights* worldLights, RenderStats* renderStats);
    void setupShader(const glm::mat4& modelTransform, Shader* shader);

private:
    Camera m_camera{};
    WorldLights* m_worldLights{ nullptr };
    RenderStats* m_renderStats{ nullptr };
    inline static RenderPass* m_currentRenderPass{ nullptr };
    friend class Renderer;
};
} // namespace re

#endif //SIMPLERENDERENGINE_RENDERPASS_H
