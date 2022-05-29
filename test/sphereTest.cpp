//
// Created by william on 2022/5/23.
//

#include "basicProject.h"
#include "commonMacro.h"
#include "core/worldLights.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
using namespace re;

class SphereExample : public BasicProject
{
public:
    using BasicProject::BasicProject;
    ~SphereExample() override = default;

    void run() override
    {
        m_camera.setLookAt({ 0.0f, 0.0f, 3.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
        m_camera.setPerspectiveProjection(60, 0.1, 100);
        m_shader = std::unique_ptr<Shader>(Shader::getStandard());
        m_material = std::make_unique<Material>(m_shader.get());
        m_mesh.reset(Mesh::create().withSphere().build());
        m_worldLights = std::make_unique<WorldLights>();
        bool point = true;
        if (point)
        {
            m_worldLights->addLight(Light::create().withPointLight({ 0, 2, 1 }).withColor({ 1, 0, 0 }).withRange(10).build());
            m_worldLights->addLight(Light::create().withPointLight({ 2, 0, 1 }).withColor({ 0, 1, 0 }).withRange(10).build());
            m_worldLights->addLight(Light::create().withPointLight({ 0, -2, 1 }).withColor({ 0, 0, 1 }).withRange(10).build());
            m_worldLights->addLight(Light::create().withPointLight({ -2, 0, 1 }).withColor({ 1, 1, 1 }).withRange(10).build());
        }
        else
        {
            m_worldLights->addLight(Light::create().withDirectionalLight({ 0, 1, 1 }).withColor({ 1, 0, 0 }).withRange(10).build());
            m_worldLights->addLight(Light::create().withDirectionalLight({ 1, 0, 1 }).withColor({ 0, 1, 0 }).withRange(10).build());
            m_worldLights->addLight(Light::create().withDirectionalLight({ 0, -1, 1 }).withColor({ 0, 0, 1 }).withRange(10).build());
            m_worldLights->addLight(Light::create().withDirectionalLight({ -1, 0, 1 }).withColor({ 1, 1, 1 }).withRange(10).build());
        }
        BasicProject::run();
    }

    void render(Renderer* r) override
    {
        /// 渲染
        auto renderPass = r->createRenderPass().withCamera(m_camera).withWorldLights(m_worldLights.get()).build();
        renderPass.draw(m_mesh.get(), glm::eulerAngleY(glm::radians(30.0f * m_totalTime)), m_material.get());
    }

    void setTitle() override
    {
        m_renderer.setWindowTitle("SphereExample");
    }
};

void sphereTest()
{
    SphereExample test;
    test.run();
}