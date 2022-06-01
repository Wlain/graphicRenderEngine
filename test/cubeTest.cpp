//
// Created by william on 2022/5/22.
//
#include "basicProject.h"
#include "commonMacro.h"
#include "core/worldLights.h"
#include "guiCommonDefine.h"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

using namespace re;

class CubeExample : public BasicProject
{
public:
    using BasicProject::BasicProject;
    ~CubeExample() = default;

    void initialize() override
    {
        m_camera = MAKE_UNIQUE(m_camera);
        m_camera->setLookAt({ 0.0f, 0.0f, 3.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
        m_camera->setPerspectiveProjection(60, 0.1, 100);
        auto shader = Shader::getStandard();
        m_material = shader->createMaterial();
        m_material->setTexture(Texture::getFontTexture());
        m_mesh = Mesh::create().withCube().build();
        m_worldLights = std::make_unique<WorldLights>();
        m_worldLights->addLight(Light::create().withPointLight({ 3, 0, 0 }).withColor({ 0, 1, 0 }).withRange(20).build());
        m_worldLights->addLight(Light::create().withPointLight({ 0, -3, 0 }).withColor({ 0, 0, 1 }).withRange(20).build());
        m_worldLights->addLight(Light::create().withPointLight({ -3, 0, 0 }).withColor({ 1, 1, 1 }).withRange(20).build());
    }

    void render() override
    {
        m_camera->setLookAt(m_eye, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
        m_camera->setPerspectiveProjection(60, 0.1, 100);
        auto renderPass = RenderPass::create().withCamera(*m_camera).withWorldLights(m_worldLights.get()).build();
        renderPass.draw(m_mesh, glm::eulerAngleY(glm::radians(30 * m_totalTime)), m_material);
        ImGui::DragFloat3(":eye", &m_eye[0]);
    }

    void setTitle() override
    {
        m_title = "CubeExample";
    }

private:
    glm::vec3 m_eye{ 0.0f, 0.0f, 3.0f };
};

void cubeTest()
{
    CubeExample test;
    test.run();
}