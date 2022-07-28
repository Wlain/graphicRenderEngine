//
// Created by cwb on 2022/7/28.
//
#include "commonMacro.h"
#include "core/worldLights.h"
#include "engineTestSimple.h"
#include "guiCommonDefine.h"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

class DeferredShadingExample : public CommonInterface
{
public:
    using CommonInterface::CommonInterface;
    ~DeferredShadingExample() override = default;

    void initialize() override
    {
        m_camera.setLookAt({ 0.0f, 0.0f, 3.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
        m_camera.setPerspectiveProjection(60, 0.1, 100);
        auto shader = Shader::getStandardBlinnPhong();
        m_material = shader->createMaterial();
        m_material->setTexture(Texture::getFontTexture());
        m_material->setColor({ 1.0f, 1.0f, 1.0f, 1.0f });
        m_material->setSpecularity({ 0.5, 0.5, 0.5, 180.0f });
        m_mesh = Mesh::create().withCube().build();
        m_worldLights = std::make_unique<WorldLights>();
        m_worldLights->addLight(Light::create().withPointLight({ 3, 0, 0 }).withColor({ 0, 1, 0 }).withRange(20).build());
        m_worldLights->addLight(Light::create().withPointLight({ 0, -3, 0 }).withColor({ 0, 0, 1 }).withRange(20).build());
        m_worldLights->addLight(Light::create().withPointLight({ -3, 0, 0 }).withColor({ 1, 1, 1 }).withRange(20).build());
    }

    void render() override
    {
        m_camera.setLookAt(m_eye, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
        m_camera.setPerspectiveProjection(60, 0.1, 100);
        auto renderPass = RenderPass::create().withCamera(m_camera).withWorldLights(m_worldLights.get()).build();
        renderPass.draw(m_mesh, glm::eulerAngleY(glm::radians(30 * m_totalTime)), m_material);
        ImGui::DragFloat3(":eye", &m_eye[0]);
    }

    void setTitle() override
    {
        m_title = "DeferredShadingExample";
    }

private:
    glm::vec3 m_eye{ 0.0f, 0.0f, 3.0f };
};

void deferredShadingTest()
{
    GLFWRenderer renderer{};
    EngineTestSimple test(renderer);
    auto sceneNodeEffect = std::make_shared<DeferredShadingExample>(&renderer);
    auto effect = std::make_shared<EffectManager>();
    effect->insertEffect(sceneNodeEffect);
    test.setEffect(effect);
    test.run();
}