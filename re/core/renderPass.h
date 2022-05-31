//
// Created by william on 2022/5/26.
//

#ifndef SIMPLERENDERENGINE_RENDERPASS_H
#define SIMPLERENDERENGINE_RENDERPASS_H
#include "camera.h"
#include "framebuffer.h"
#include "mesh.h"

#include <string>
namespace re
{
class Renderer;
class Shader;
class RenderStats;
class Material;
class FrameBuffer;
//渲染通道封装了一些渲染状态，并允许添加 drawcall，注意，一次只能有一个渲染传递对象处于活动状态。
class RenderPass
{
public:
    class RenderPassBuilder
    {
    public:
        RenderPassBuilder(const RenderPassBuilder& r) = delete;
        RenderPassBuilder& withName(const std::string& name);
        RenderPassBuilder& withCamera(const Camera& camera);
        RenderPassBuilder& withWorldLights(WorldLights* worldLights);
        // Set the clear color.Default enabled with the color value {1.0,0.0,0.0,1.0}
        RenderPassBuilder& withClearColor(bool enabled = true, glm::vec4 color = { 0, 0, 0, 1 });
        // Set the clear depth. Value is clamped between [0.0;1.0], Default: enabled with depth value 1.0
        RenderPassBuilder& withClearDepth(bool enabled = true, float value = 1);
        // Set the clear depth. Value is clamped between, Default: disabled
        RenderPassBuilder& withClearStencil(bool enabled = false, int value = 0);
        // calls ImGui::Render() in the end of the renderpass
        RenderPassBuilder& withGUI(bool enabled = true);
        RenderPassBuilder& withFramebuffer(const std::shared_ptr<FrameBuffer>& framebuffer);
        RenderPass build();

    private:
        RenderPassBuilder() = default;
        RenderPassBuilder(RenderStats* renderStats);

    private:
        glm::vec4 m_clearColorValue = { 1, 0, 0, 1 };
        float m_clearDepthValue{ 1.0f };
        int m_clearStencilValue{ 0 };
        bool m_clearDepth{ true };
        bool m_clearStencil{ false };
        bool m_clearColor{ true };
        bool m_gui{ false };
        std::string m_name;
        WorldLights* m_worldLights{ nullptr };
        RenderStats* m_renderStats{ nullptr };
        std::shared_ptr<FrameBuffer> m_framebuffer;
        Camera m_camera;
        friend class Renderer;
        friend class RenderPass;
    };

public:
    RenderPass() = default;
    static RenderPassBuilder create(); // Create a RenderPass
    virtual ~RenderPass();
    void drawLines(const std::vector<glm::vec3>& vertices, glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f }, Mesh::Topology meshTopology = Mesh::Topology::Lines);
    void draw(const std::shared_ptr<Mesh>& mesh, glm::mat4 modelTransform, std::shared_ptr<Material>& material);
    std::vector<glm::vec4> readPixels(unsigned int x, unsigned int y, unsigned int width = 1, unsigned int height = 1);

private:
    RenderPass(RenderPass::RenderPassBuilder& builder);
    void setupShader(const glm::mat4& modelTransform, Shader* shader);
    static void finish();

private:
    inline static bool s_instance{ false };
    inline static bool s_lastGui{ false };
    inline static std::shared_ptr<FrameBuffer> s_lastFramebuffer;

private:
    std::shared_ptr<FrameBuffer> m_framebuffer;
    Shader* m_lastBoundShader = { nullptr };
    Material* m_lastBoundMaterial = { nullptr };
    Mesh* m_lastBoundMesh = { nullptr };
    Camera m_camera{};
    WorldLights* m_worldLights{ nullptr };
    RenderStats* m_renderStats{ nullptr };
    glm::mat4 m_projection;
    glm::uvec2 m_viewportOffset;
    glm::uvec2 m_viewportSize;
    friend class Renderer;
};
} // namespace re

#endif //SIMPLERENDERENGINE_RENDERPASS_H
