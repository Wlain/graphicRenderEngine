//
// Created by cwb on 2022/7/6.
//

#include "engineTestSimple.h"
#include "core/modelImporter.h"
#include "guiCommonDefine.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

class ShadowMapExample : public CommonInterface
{
public:
    using CommonInterface::CommonInterface;
    ~ShadowMapExample() override = default;
    void initialize() override
    {
        m_camera.setLookAt(m_eye, m_at, m_up);
        m_camera.setPerspectiveProjection(45.0f, 0.1f, 100.0f);
        std::vector<std::shared_ptr<Material>> materialUnused;
        m_mesh = ModelImporter::importObj("resources/objFiles/suzanne.obj", materialUnused);
        m_meshSphere = Mesh::create().withSphere().build();
        m_meshPlane = Mesh::create().withQuad(4).build();
        m_worldLights = MAKE_UNIQUE(m_worldLights);
        m_worldLights->setAmbientLight(glm::vec3{ 0.02f });
        m_lightDirection = glm::normalize(glm::vec3{ 1, 1, 1 });
        m_worldLights->addLight(Light::create().withDirectionalLight(m_lightDirection).withColor(Color(1, 1, 1), 7).build());
        m_material = Shader::getStandardPBR()->createMaterial({ { "S_SHADOW", "1" } });
        m_material->setName("PBR material");
        updateShadowMap();
        /// debug
        m_debugMaterial = Shader::create()
                              .withSourceFile("shaders/unlit_vert.glsl", Shader::ShaderType::Vertex)
                              .withSourceFile("shaders/unlit_frag.glsl", Shader::ShaderType::Fragment)
                              .withName("Unlit")
                              .withCullFace(Shader::CullFace::None)
                              .build()
                              ->createMaterial();

        m_debugMesh = Mesh::create().withQuad().build();
    }

    void render() override
    {
        // shadow pass - build shadow map - render shadow casters with shadow material
        updateShadowMapViewProjection(&m_shadowMapCamera, m_lightDirection, m_fieldOfViewY, m_near, m_far, m_eye, m_at);
        auto renderPass1 = RenderPass::create()
                               .withFramebuffer(m_shadowMapFbo)
                               .withCamera(m_shadowMapCamera)
                               .withWorldLights(m_worldLights.get())
                               .withClearColor(false)
                               .withGUI(false)
                               .build();

        renderWorld(renderPass1, m_shadowMapMaterial);
        renderPass1.finish();

        // render pass - render world with shadow lookup
        auto renderPass2 = RenderPass::create()
                               .withCamera(m_camera)
                               .withClearColor(true, { 0, 0, 0, 1 })
                               .withWorldLights(m_worldLights.get())
                               .build();

        m_material->set("shadowMap", m_shadowMapTexture);
        static glm::mat4 offset = glm::translate(glm::vec3(0.5f)) * glm::scale(glm::vec3(0.5f));
        m_shadowViewProjection = offset * m_shadowMapCamera.getProjectionTransform({ m_shadowMapSize, m_shadowMapSize }) * m_shadowMapCamera.getViewTransform();
        m_material->set("shadowViewProjOffset", m_shadowViewProjection);
        renderWorld(renderPass2, m_material);
        //        // Debug
        //         auto renderPass = RenderPass::create().withCamera(m_camera).build();
        //         m_debugMaterial->setTexture(m_shadowMapTexture);
        //         renderPass.draw(m_debugMesh, glm::mat4(1.0f), m_debugMaterial);

        bool update = false;

        ImGui::Checkbox("RrawSphere", &m_drawSphere);
        if (ImGui::IsItemHovered())
        {
            ImGui::SetTooltip("r to toogle");
        }
        update |= ImGui::DragFloat("Bias offset", &m_biasOffset);
        update |= ImGui::DragFloat("Bias factor", &m_biasFactor);
        update |= ImGui::Checkbox("Linear texture interpolation", &m_linearInterpolation);
        const char* resolutionString =
            "4096\0"
            "2048\0"
            "1024\0"
            "512\0"
            "256\0"
            "128\0"
            "64\0"
            "32\0";
        update |= ImGui::Combo("Resolution", &m_resolutionId, resolutionString);
        const char* depthString =
            "I16\0"
            "I24\0"
            "I32\0"
            "F32\0"
            "I24_STENCIL8\0"
            "F32_STENCIL8\0"
            "STENCIL8\0";

        update |= ImGui::Combo("Texture depth", reinterpret_cast<int*>(&m_depthPrecision), depthString);
        m_shadowMapSize = (unsigned int)glm::pow(2, 12 - m_resolutionId);
        if (update)
        {
            updateShadowMap();
        }
    }

    void setTitle() override
    {
        m_title = "ShadowMapExample";
    }

    static void updateShadowMapViewProjection(Camera* shadowMapCamera, glm::vec3 lightDirection, float fieldOfViewYDegrees, float near, float far, glm::vec3 eye, glm::vec3 at, float shadowMapFraction = 1.0)
    {
        glm::vec3 viewDirection = glm::normalize(at - eye);
        glm::vec3 lightEye = eye + viewDirection * ((far - near) * shadowMapFraction / 2 + near);
        glm::vec3 lightAt = lightEye - lightDirection;

        shadowMapCamera->setOrthographicProjection(4, -4, 4);
        shadowMapCamera->setLookAt(lightEye, lightAt, abs(lightDirection.x) < 0.5 ? glm::vec3(1, 0, 0) : glm::vec3(0, 0, 1));
    }

    void renderWorld(RenderPass& rp, std::shared_ptr<Material> mat)
    {
        rp.draw(m_drawSphere ? m_meshSphere : m_mesh, glm::rotate(m_rotate.x, glm::vec3(1, 0, 0)) * glm::rotate(m_rotate.y, glm::vec3(0, 1, 0)), mat);
        rp.draw(m_meshPlane, glm::translate(glm::vec3{ 0, -1.2f, 0 }) * glm::rotate(glm::radians(-90.0f), glm::vec3(1, 0, 0)), mat);
    }

    void updateShadowMap()
    {
        m_shadowMapTexture = Texture::create()
                                 .withName("ShadowMapTex")
                                 .withGenerateMipmaps(false)
                                 .withFilterSampling(m_linearInterpolation)
                                 .withWrapUV(Texture::Wrap::ClampToEdge)
                                 .withDepth(m_shadowMapSize, m_shadowMapSize, m_depthPrecision)
                                 .build();

        m_shadowMapFbo = FrameBuffer::create()
                             .withName("ShadowMap")
                             .withDepthTexture(m_shadowMapTexture)
                             .build();

        m_shadowMapMaterial = Shader::create()
                                  .withSourceFile("shaders/shadow_vert.glsl", Shader::ShaderType::Vertex)
                                  .withSourceFile("shaders/shadow_frag.glsl", Shader::ShaderType::Fragment)
                                  .withName("Shadow")
                                  .withOffset(m_biasOffset, m_biasFactor) // shadow bias
                                  .withColorWrite({ false, false, false, false })
                                  .build()
                                  ->createMaterial();

        m_material->setName("Shadow material");
    }

private:
    Camera m_shadowMapCamera;
    Texture::DepthPrecision m_depthPrecision = Texture::DepthPrecision::I24;
    std::shared_ptr<Mesh> m_meshSphere;
    std::shared_ptr<Mesh> m_meshPlane;
    std::shared_ptr<Texture> m_shadowMapTexture;
    std::shared_ptr<Material> m_shadowMapMaterial;
    std::shared_ptr<Material> m_debugMaterial;
    std::shared_ptr<Mesh> m_debugMesh;
    std::shared_ptr<FrameBuffer> m_shadowMapFbo;
    glm::mat4 m_shadowViewProjection{};
    glm::vec3 m_lightDirection{};
    glm::vec3 m_eye{ 0, 0, 4.0f };
    glm::vec3 m_at{ 0, 0, 0 };
    glm::vec3 m_up{ 0, 1, 0 };
    unsigned int m_shadowMapSize = 2048;
    int m_resolutionId = 1;
    float m_biasOffset = 2.5f;
    float m_biasFactor = 10.0f;
    float m_fieldOfViewY = 45.0f;
    float m_near = 0.1f;
    float m_far = 10.0f;
    bool m_linearInterpolation = true;
    bool m_drawSphere = false;
};

void shadowMapTest()
{
    GLFWRenderer renderer{};
    EngineTestSimple test(renderer);
    auto sceneNodeEffect = std::make_shared<ShadowMapExample>(&renderer);
    auto effect = std::make_shared<EffectManager>();
    effect->insertEffect(sceneNodeEffect);
    test.setEffect(effect);
    test.run();
}