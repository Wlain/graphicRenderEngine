//
// Created by cwb on 2022/6/21.
//
#include "core/skybox.h"

#include "basicProject.h"
#include "guiCommonDefine.h"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>
using namespace re;
class SkyBoxExample : public BasicProject
{
public:
    using BasicProject::BasicProject;
    ~SkyBoxExample() override = default;
    void initialize() override
    {
        m_camera.setLookAt(m_eye, m_at, m_up);
        m_camera.setPerspectiveProjection(60, 0.1, 100);
        m_material = Shader::getStandardPBR()->createMaterial();
        m_material->setColor({ 1.0f, 1.0f, 1.0f, 1.0f });
        m_material->setMetallicRoughness({ 0.5f, 0.5f });
        m_mesh = Mesh::create().withCube().build();
        m_worldLights = MAKE_UNIQUE(m_worldLights);
        m_worldLights->setAmbientLight({ 0.0, 0.0, 0.0 });
        m_worldLights->addLight(Light::create().withPointLight({ 0, 3, 0 }).withColor({ 1, 0, 0 }).withRange(20).build());
        m_worldLights->addLight(Light::create().withPointLight({ 3, 0, 0 }).withColor({ 0, 1, 0 }).withRange(20).build());
        m_worldLights->addLight(Light::create().withPointLight({ 0, -3, 0 }).withColor({ 0, 0, 1 }).withRange(20).build());
        m_worldLights->addLight(Light::create().withPointLight({ -3, 0, 0 }).withColor({ 1, 1, 1 }).withRange(20).build());
        auto tex = Texture::create()
                       .withFileCubeMap("resources/cube/cube-posx.png", Texture::CubeMapSide::PositiveX)
                       .withFileCubeMap("resources/cube/cube-negx.png", Texture::CubeMapSide::NegativeX)
                       .withFileCubeMap("resources/cube/cube-posy.png", Texture::CubeMapSide::PositiveY)
                       .withFileCubeMap("resources/cube/cube-negy.png", Texture::CubeMapSide::NegativeY)
                       .withFileCubeMap("resources/cube/cube-posz.png", Texture::CubeMapSide::PositiveZ)
                       .withFileCubeMap("resources/cube/cube-negz.png", Texture::CubeMapSide::NegativeZ)
                       .withWrapUV(Texture::Wrap::ClampToEdge)
                       .build();
        m_skybox = Skybox::create();
        m_skybox->getMaterial()->setTexture(tex);
    }
    void render() override
    {
        auto renderPass = RenderPass::create()
                              .withCamera(m_camera)
                              .withWorldLights(m_worldLights.get())
                              .withSkybox(m_skybox)
                              .withName("Skybox Frame")
                              .build();
        auto modelMatrix = glm::eulerAngleY(glm::radians(30 * m_totalTime));
        renderPass.draw(m_mesh, modelMatrix, m_material);
        renderPass.draw(m_mesh, glm::translate(glm::vec3(2, 2, -2)) * glm::scale(glm::vec3(0.5)) * modelMatrix, m_material);
        renderPass.draw(m_mesh, glm::translate(glm::vec3(2, -2, -2)) * glm::scale(glm::vec3(0.5)) * modelMatrix, m_material);

        ImGui::DragFloat3("eye", &m_eye.x);
        ImGui::DragFloat3("at", &m_at.x);
        ImGui::DragFloat3("up", &m_up.x);
        m_camera.setLookAt(m_eye, m_at, m_up);
    }
    void setTitle() override
    {
        m_title = "SkyBoxExample";
    }

private:
    std::shared_ptr<Skybox> m_skybox;
    glm::vec3 m_eye{ 0, 0, 10.0f };
    glm::vec3 m_at{ 0, 0, 0 };
    glm::vec3 m_up{ 0, 1, 0 };
};

void skyboxTest()
{
    SkyBoxExample test;
    test.run();
}