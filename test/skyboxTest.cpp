//
// Created by cwb on 2022/6/21.
//
#include "basicProject.h"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
using namespace re;
class SkyBoxExample : public BasicProject
{
public:
    using BasicProject::BasicProject;
    ~SkyBoxExample() override = default;
    void initialize() override
    {
        m_camera.setLookAt({ 0, 0, 3 }, { 0, 0, 0 }, { 0, 1, 0 });
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
                       .withFileCubeMap(GET_CURRENT("test/resources/cube/cube-posx.png"), Texture::CubeMapSide::PositiveX)
                       .withFileCubeMap(GET_CURRENT("test/resources/cube/cube-negx.png"), Texture::CubeMapSide::NegativeX)
                       .withFileCubeMap(GET_CURRENT("test/resources/cube/cube-posy.png"), Texture::CubeMapSide::PositiveY)
                       .withFileCubeMap(GET_CURRENT("test/resources/cube/cube-negy.png"), Texture::CubeMapSide::NegativeY)
                       .withFileCubeMap(GET_CURRENT("test/resources/cube/cube-posz.png"), Texture::CubeMapSide::PositiveZ)
                       .withFileCubeMap(GET_CURRENT("test/resources/cube/cube-negz.png"), Texture::CubeMapSide::NegativeZ)
                       .build();
        m_skybox = Skybox::create();
        auto skyboxMaterial = Shader::getSkybox()->createMaterial();
        skyboxMaterial->setTexture(tex);
        m_skybox->setMaterial(skyboxMaterial);
    }
    void render() override
    {
        auto renderPass = RenderPass::create()
                              .withCamera(m_camera)
                              .withWorldLights(m_worldLights.get())
                              .withSkybox(m_skybox)
                              .withName("Frame")
                              .build();

        renderPass.draw(m_mesh, glm::mat4(1), m_material);
        renderPass.draw(m_mesh, glm::translate(glm::vec3(2, 2, 2)) * glm::scale(glm::vec3(0.1)), m_material);
        renderPass.draw(m_mesh, glm::translate(glm::vec3(-2, -2, -2)) * glm::scale(glm::vec3(0.1)), m_material);
        m_inspector.update();
        m_inspector.gui();
    }
    void setTitle() override
    {
        m_title = "SkyBoxExample";
    }

private:
    std::shared_ptr<Skybox> m_skybox;
};

void skyboxTest()
{
    SkyBoxExample test;
    test.run();
}