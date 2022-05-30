//
// Created by william on 2022/5/23.
//

#include "basicProject.h"
#include "commonMacro.h"
#include "core/worldLights.h"

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
        m_camera = MAKE_UNIQUE(m_camera);
        m_camera->setLookAt(m_eye, m_at, m_up);
        m_camera->setPerspectiveProjection(60, 0.1, 100);
        auto shader = Shader::getStandard();
        m_material = shader->createMaterial();
        m_mesh = Mesh::create().withSphere().build();
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

    void drawCross(RenderPass& rp, const glm::vec3& p, float size = 0.3f)
    {
        rp.drawLines({ p - glm::vec3{ size, 0, 0 },
                       p + glm::vec3{ size, 0, 0 },
                       p - glm::vec3{ 0, size, 0 },
                       p + glm::vec3{ 0, size, 0 },
                       p - glm::vec3{ 0, 0, size },
                       p + glm::vec3{ 0, 0, size } },
                     { 0, 1, 0, 1 });
    }

    void drawLight(RenderPass& rp, Light* l, float size)
    {
        if (l->type == Light::Type::Point)
        {
            drawCross(rp, l->position, size);
        }
        if (l->type == Light::Type::Directional)
        {
            rp.drawLines({ l->position, l->position - l->direction * size * 2.0f }, { 1, 1, 0, 1 });
        }
    }

    void update(float deltaTime) override
    {
        if (m_animatedCamera)
        {
            m_eye = {
                sin(m_totalTime * -0.2) * 5.0f,
                sin(m_totalTime * -0.4) * 0.5f,
                cos(m_totalTime * -0.2) * 5.0f,
            };
        }
        m_camera->setLookAt(m_eye, m_at, m_up);
        if (m_animatedLight)
        {
            for (int i = 0; i < m_worldLights->lightCount(); ++i)
            {
                m_worldLights->getLight(i)->position = {
                    sin(m_totalTime + i * i) * 1.5f,
                    sin(m_totalTime * 2 + i * i) * 0.5f,
                    cos(m_totalTime + i * i) * 1.5f,
                };
            }
        }
        BasicProject::update(deltaTime);
    }

    void render(Renderer* r) override
    {
        /// 渲染
        auto renderPass = r->createRenderPass().withCamera(*m_camera).withWorldLights(m_worldLights.get()).build();
        drawCross(renderPass, { 2, 2, 2 });
        drawCross(renderPass, { -2, -2, -2 });
        // Show Label (with invisible window)
        for (int i = 0; i < m_worldLights->lightCount(); i++)
        {
            auto l = m_worldLights->getLight(i);
            if (m_debugLight)
            {
                drawLight(renderPass, l, m_debugLightSize);
            }
        }
        renderPass.draw(m_mesh, glm::eulerAngleY(m_totalTime * 30), m_material);
    }

    void setTitle() override
    {
        m_renderer.setWindowTitle("SphereExample");
    }

private:
    glm::vec3 m_eye{ 0, 0, 5 };
    glm::vec3 m_at{ 0, 0, 0 };
    glm::vec3 m_up{ 0, 1, 0 };
    bool m_debugLight = true;
    bool m_animatedLight = true;
    bool m_animatedCamera = true;
    float m_debugLightSize = 0.2;
};

void sphereTest()
{
    SphereExample test;
    test.run();
}