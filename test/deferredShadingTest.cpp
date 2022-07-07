//
// Created by cwb on 2022/7/7.
//

#include "basicProject.h"
#include "guiCommonDefine.h"
#include "core/modelImporter.h"

class DeferredShadingExample : public BasicProject
{
public:
    ~DeferredShadingExample() override = default;
    void initialize() override
    {
        m_camera.setLookAt(m_eye, m_at, m_up);
        m_camera.setPerspectiveProjection(m_fieldOfViewY, m_near, m_far);

        m_mesh = ModelImporter::importObj("resources/objFiles/sponza/sponza.obj", m_materials);

        m_worldLights = MAKE_UNIQUE(m_worldLights);
        m_worldLights->setAmbientLight(glm::vec3{ 0.02f });
        m_lightDirection = glm::normalize(glm::vec3{ 1, 1, 1 });
        m_worldLights->addLight(Light::create().withDirectionalLight(m_lightDirection).withColor(Color(1, 1, 1), 7).build());
    }
    void render() override
    {
        // render pass - render world with shadow lookup
        auto rp = RenderPass::create()
                      .withCamera(m_camera)
                      .withClearColor(true, { 0, 0, 0, 1 })
                      .withWorldLights(m_worldLights.get())
                      .build();

        rp.draw(m_mesh, glm::mat4(1), m_materials);

        ImGui::DragFloat3("Light pos", &m_worldLights->getLight(0)->position.x, 0.1f);
        ImGui::DragFloat3("eye", &m_eye.x);
        ImGui::DragFloat3("at", &m_at.x);
        ImGui::DragFloat3("up", &m_up.x);
        m_camera.setLookAt(m_eye, m_at, m_up);
    }
    void setTitle() override
    {
        m_title = "DeferredShadingExample";
    }

    void initFramebufferObject(int width, int height)
    {
        m_positionTexture = Texture::create().withRGBAData(nullptr, width, height).withName("position").build();
        m_depthTexture = Texture::create().withDepth(width, height).withName("depth").build();
        m_normalTexture = Texture::create().withRGBAData(nullptr, width, height).withName("normal").build();
        m_colorTexture = Texture::create().withRGBAData(nullptr, width, height).withName("color").build();
        m_framebuffer = FrameBuffer::create()
                            .withColorTexture(m_positionTexture)
                            .withDepthTexture(m_depthTexture)
                            .withColorTexture(m_normalTexture)
                            .withColorTexture(m_colorTexture)
                            .build();
    }

private:
    Camera m_shadowMapCamera;
    std::shared_ptr<FrameBuffer> m_framebuffer;
    std::shared_ptr<Texture> m_shadowMapTexture;
    std::shared_ptr<Texture> m_positionTexture;
    std::shared_ptr<Texture> m_depthTexture;
    std::shared_ptr<Texture> m_normalTexture;
    std::shared_ptr<Texture> m_colorTexture;
    std::vector<std::shared_ptr<Material>> m_materials;
    glm::vec3 m_lightDirection{};
    glm::vec3 m_eye{ 0, 0, 3.0f };
    glm::vec3 m_at{ 0, 0, 0 };
    glm::vec3 m_up{ 0, 1, 0 };
    unsigned int m_shadowMapSize = 2048;
    float m_fieldOfViewY = 45;
    float m_near = 0.1;
    float m_far = 100;
};

void deferredShadingTest()
{
    DeferredShadingExample test;
    test.run();
}