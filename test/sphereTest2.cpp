//
// Created by william on 2022/5/24.
//

#include "basicProject.h"
#include "commonMacro.h"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>

using namespace re;

class SphereExample2 : public BasicProject
{
public:
    using BasicProject::BasicProject;
    ~SphereExample2() override = default;
    void initialize() override
    {
        m_camera.setLookAt({ 0.0f, 0.0f, 3.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
        m_camera.setPerspectiveProjection(60, 0.1, 100);
        m_material = Shader::getStandardBlinnPhong()->createMaterial();
        m_material->setColor(1.0f);
        m_material->setSpecularity({ 1, 1, 1, 50.0f });
        m_pos1 = glm::translate(glm::mat4(1), { -1, 0, 0 });
        m_material2 = Shader::getUnlit()->createMaterial();
        m_material2->setColor({ 1, 1, 0, 1 });
        m_material2->setSpecularity(0.0f);
        m_pos2 = glm::translate(glm::mat4(1), { 1, 0, 0 });
        m_mesh = Mesh::create().withSphere().build();
        m_worldLights = std::make_unique<WorldLights>();
        m_worldLights->addLight(Light::create().withDirectionalLight({ 0, 2, 1 }).withColor({ 1, 1, 1 }).withRange(10).build());
    }

    void render() override
    {
        /// 渲染
        auto renderPass = RenderPass::create().withCamera(m_camera).withWorldLights(m_worldLights.get()).build();
        renderPass.draw(m_mesh, m_pos1, m_material);
        renderPass.draw(m_mesh, m_pos2, m_material2);
        m_inspector.update();
        m_inspector.gui();
    }

    void setTitle() override
    {
        m_title = "SphereExample2";
    }

private:
    std::shared_ptr<Material> m_material2;
    glm::mat4 m_pos1;
    glm::mat4 m_pos2;
};

void sphereTest2()
{
    SphereExample2 test;
    test.run();
}