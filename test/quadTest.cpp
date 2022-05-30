//
// Created by william on 2022/5/24.
//

#include "basicProject.h"
#include "commonMacro.h"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

using namespace re;

class QuadExample : public BasicProject
{
public:
    using BasicProject::BasicProject;
    ~QuadExample() = default;

    void run() override
    {
        m_camera = MAKE_UNIQUE(m_camera);
        m_camera->setLookAt({ 0.0f, 0.0f, 3.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
        m_camera->setPerspectiveProjection(60, 0.1, 100);
        m_shader = Shader::getUnlit();
        m_material = std::make_unique<Material>(m_shader);
        m_material->setTexture(Texture::create().withFile(GET_CURRENT("test/resources/test.jpg")).build());
        m_mesh = Mesh::create().withQuad().build();
        BasicProject::run();
    }

    void render(Renderer* r) override
    {
        /// 渲染
        auto renderPass = r->createRenderPass().withCamera(*m_camera).withWorldLights(m_worldLights.get()).build();
        renderPass.draw(m_mesh, glm::eulerAngleY(glm::radians(30.0f * m_totalTime)), m_material.get());
    }

    void setTitle() override
    {
        m_renderer.setWindowTitle("QuadExample");
    }
};

void quadTest()
{
    QuadExample test;
    test.run();
}