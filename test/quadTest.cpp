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

    void initialize() override
    {
        m_camera = MAKE_UNIQUE(m_camera);
        m_camera->setLookAt({ 0.0f, 0.0f, 3.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
        m_camera->setPerspectiveProjection(60, 0.1, 100);
        auto shader = Shader::getUnlit();
        m_material = shader->createMaterial();
        m_material->setTexture(Texture::create().withFile(GET_CURRENT("test/resources/test.jpg")).build());
        m_mesh = Mesh::create().withQuad().build();
    }

    void render() override
    {
        /// 渲染
        auto renderPass = RenderPass::create().withCamera(*m_camera).withWorldLights(m_worldLights.get()).build();
        renderPass.draw(m_mesh, glm::eulerAngleY(glm::radians(30.0f * m_totalTime)), m_material);
    }

    void setTitle() override
    {
        m_title = "QuadExample";
    }
};

void quadTest()
{
    QuadExample test;
    test.run();
}