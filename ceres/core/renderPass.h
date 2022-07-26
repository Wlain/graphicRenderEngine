
//
// Created by william on 2022/5/26.
//

#ifndef GRAPHICRENDERENGINE_RENDERPASS_H
#define GRAPHICRENDERENGINE_RENDERPASS_H
#include "camera.h"
#include "color.h"
#include "framebuffer.h"
#include "mesh.h"
#include "spriteBatch.h"

#include <set>
#include <string>
namespace ceres
{
class Renderer;
class Shader;
class RenderStats;
class Material;
class FrameBuffer;
class WorldLights;
class Skybox;
//渲染通道封装了一些渲染状态，并允许添加 drawCall，注意，一次只能有一个渲染传递对象处于活动状态。
class RenderPass
{
private:
    struct RenderQueueObj
    {
        std::shared_ptr<Mesh> mesh;
        glm::mat4 modelTransform;
        std::shared_ptr<Material> material;
        int subMesh = 0;
    };

    struct GlobalUniforms
    {
        glm::mat4* g_view;
        glm::mat4* g_projection;
        glm::vec4* g_viewport;
        glm::vec4* g_cameraPos;
        glm::vec4* g_ambientLight;
        glm::vec4* g_lightColorRange;
        glm::vec4* g_lightPosType;
    };

public:
    class RenderPassBuilder
    {
    public:
        RenderPassBuilder& withName(const std::string& name);
        RenderPassBuilder& withCamera(const Camera& camera);
        RenderPassBuilder& withWorldLights(WorldLights* worldLights);
        // Set the clear color.Default enabled with the color value {1.0,0.0,0.0,1.0}
        RenderPassBuilder& withClearColor(bool enabled = true, Color color = { 0, 0, 0, 1 });
        // Set the clear depth. Value is clamped between [0.0;1.0], Default: enabled with depth value 1.0
        RenderPassBuilder& withClearDepth(bool enabled = true, float value = 1);
        // Set the clear depth. Value is clamped between, Default: disabled
        RenderPassBuilder& withClearStencil(bool enabled = false, int value = 0);
        // calls ImGui::Render() in the end of the renderpass
        RenderPassBuilder& withGUI(bool enabled = true);
        RenderPassBuilder& withFramebuffer(std::shared_ptr<FrameBuffer> framebuffer);
        RenderPassBuilder& withSkybox(const std::shared_ptr<Skybox>& skybox);
        RenderPass build();

    private:
        RenderPassBuilder() = default;
        explicit RenderPassBuilder(RenderStats* renderStats);

    private:
        glm::vec4 m_clearColorValue = { 1, 0, 0, 1 };
        float m_clearDepthValue{ 1.0f };
        int m_clearStencilValue{ 0 };
        bool m_clearDepth{ true };
        bool m_clearStencil{ false };
        bool m_clearColor{ true };
        bool m_gui{ true };
        std::string m_name;
        WorldLights* m_worldLights{ nullptr };
        RenderStats* m_renderStats{ nullptr };
        std::shared_ptr<FrameBuffer> m_framebuffer;
        Camera m_camera;
        std::shared_ptr<Skybox> m_skybox;
        friend class Renderer;
        friend class RenderPass;
    };

public:
    RenderPass() = default;
    static RenderPassBuilder create(); // Create a RenderPass
    RenderPass(const RenderPass&) = delete;
    RenderPass(RenderPass&& rp) noexcept;
    RenderPass& operator=(RenderPass&& other) noexcept;
    virtual ~RenderPass();
    void drawLines(const std::vector<glm::vec3>& vertices, Color color = { 1.0f, 1.0f, 1.0f, 1.0f }, Mesh::Topology meshTopology = Mesh::Topology::Lines);
    void draw(const std::shared_ptr<Mesh>& mesh, glm::mat4 modelTransform, std::shared_ptr<Material>& material);
    void draw(std::shared_ptr<Mesh>& mesh, glm::mat4 modelTransform, std::vector<std::shared_ptr<Material>>& materials);
    void draw(std::shared_ptr<SpriteBatch>& spriteBatch, glm::mat4 modelTransform = glm::mat4(1));
    void draw(std::shared_ptr<SpriteBatch>&& spriteBatch, glm::mat4 modelTransform = glm::mat4(1));
    void blit(std::shared_ptr<Texture> texture, glm::mat4 transformation = glm::mat4(1.0f));
    void blit(std::shared_ptr<Material> material, glm::mat4 transformation = glm::mat4(1.0f));

    std::vector<Color> readPixels(unsigned int x, unsigned int y, unsigned int width = 1, unsigned int height = 1);
    // flush GPU command buffer (must be called when profiling GPU time - should not be called when not profiling)
    void finishGPUCommandBuffer() const;
    void finish();
    bool isFinished();

private:
    explicit RenderPass(RenderPass::RenderPassBuilder& builder);
    void setupShaderRenderPass(Shader* shader);
    void setupShaderRenderPass(const GlobalUniforms& globalUniforms);
    void setupGlobalShaderUniforms();
    void setupShader(const glm::mat4& modelTransform, Shader* shader);
    void drawInstance(RenderQueueObj& rqObj); // perform the actual rendering

private:
    RenderPass::RenderPassBuilder m_builder;
    Shader* m_lastBoundShader = { nullptr };
    Material* m_lastBoundMaterial = { nullptr };
    int64_t m_lastBoundMeshId = { -1 };
    glm::mat4 m_projection{};
    glm::uvec2 m_viewportOffset{};
    glm::uvec2 m_viewportSize{};
    std::vector<RenderQueueObj> m_renderQueue;
    std::set<Shader*> m_shaders;
    bool m_isFinished = false;
    friend class Renderer;
};
} // namespace ceres

#endif // GRAPHICRENDERENGINE_RENDERPASS_H
