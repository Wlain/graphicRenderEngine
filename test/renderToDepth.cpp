//
// Created by cwb on 2022/6/21.
//

#include "engineTestSimple.h"
#include "core/framebuffer.h"
#include "guiCommonDefine.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

class RenderToDepthExample : public CommonInterface
{
public:
    using CommonInterface::CommonInterface;
    ~RenderToDepthExample() override = default;
    void initialize() override
    {
        m_camera.setLookAt({ 0, 0, 3 }, { 0, 0, 0 }, { 0, 1, 0 });
        m_camera.setPerspectiveProjection(60, 1.5f, 10.5f);
        m_depthTexture = Texture::create()
                             .withDepth(1024, 1024, Texture::DepthPrecision::I16)
                             .withName("Create Depth Texture")
                             .build();
        m_texture = Texture::create()
                        .withRGBAData(nullptr, 1024, 1024)
                        .withName("Create Color Texture")
                        .build();
        m_framebuffer = FrameBuffer::create()
                            .withColorTexture(m_texture)
                            .build();
        m_framebufferDepth = FrameBuffer::create()
                                 .withDepthTexture(m_depthTexture)
                                 .build();
        m_materialOffscreen = Shader::getStandardBlinnPhong()->createMaterial();
        m_materialOffscreen->setSpecularity({ 1, 1, 1, 120 });
        m_material = Shader::getStandardBlinnPhong()->createMaterial();
        m_material->setTexture(m_texture);
        m_mesh = Mesh::create().withCube().build();
        m_worldLights = MAKE_UNIQUE(m_worldLights);
        m_worldLights->addLight(Light::create().withPointLight({ 0, 0, 3 }).withColor({ 1, 1, 1 }).withRange(20).build());
    }
    void render() override
    {
        static bool useDepthTex = false;
        auto renderToTexturePass = RenderPass::create()
                                       .withCamera(m_camera)
                                       .withWorldLights(m_worldLights.get())
                                       .withFramebuffer(useDepthTex ? m_framebufferDepth : m_framebuffer)
                                       .withClearColor(true, { 0, 1, 1, 0 })
                                       .withGUI(false)
                                       .build();
        renderToTexturePass.draw(m_mesh, glm::eulerAngleY(glm::radians((float)m_totalTime * 30)), m_materialOffscreen);
        auto renderPass = RenderPass::create() // Create a renderpass which writes to the screen.
                              .withCamera(m_camera)
                              .withWorldLights(m_worldLights.get())
                              .withClearColor(true, { 1, 0, 0, 1 })
                              .withGUI(true)
                              .build();
        static bool useBlit = false;
        ImGui::Checkbox("Use blit", &useBlit);
        ImGui::Checkbox("Use depth tex", &useDepthTex);
        if (useBlit)
        {
            renderPass.blit(useDepthTex ? m_depthTexture : m_texture);
        }
        else
        {
            m_material->setTexture(useDepthTex ? m_depthTexture : m_texture);
            renderPass.draw(m_mesh, glm::eulerAngleY(glm::radians((float)m_totalTime * 30)), m_material);
        }
        m_inspector.update();
        m_inspector.gui();
    }

private:
    std::shared_ptr<Material> m_materialOffscreen;
    std::shared_ptr<Texture> m_texture;
    std::shared_ptr<Texture> m_depthTexture;
    std::shared_ptr<FrameBuffer> m_framebuffer;
    std::shared_ptr<FrameBuffer> m_framebufferDepth;
};

void renderToDepthTest()
{
    GLFWRenderer renderer{};
    EngineTestSimple test(renderer);
    auto sceneNodeEffect = std::make_shared<RenderToDepthExample>(&renderer);
    auto effect = std::make_shared<EffectManager>();
    effect->insertEffect(sceneNodeEffect);
    test.setEffect(effect);
    test.setTitle("RenderToDepthExample");
    test.run();
}