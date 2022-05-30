//
// Created by william on 2022/5/29.
//
#include "core/glfwRenderer.h"
#include "core/material.h"
#include "core/renderer.h"
#include "core/shader.h"
#include "core/worldLights.h"

using namespace re;

class BasicProject
{
public:
    BasicProject()
    {
        m_renderer.init();
    };

    virtual ~BasicProject() = default;
    virtual void run()
    {
        setTitle();
        m_renderer.m_frameUpdate = [&](float deltaTime) {
            update(deltaTime);
        };
        m_renderer.m_frameRender = [&](Renderer* r) {
            render(r);
        };
        m_renderer.startEventLoop();
    }
    virtual void render(Renderer* r){};
    virtual void update(float deltaTime)
    {
        m_deltaTime = deltaTime;
        m_totalTime += m_deltaTime;
    }
    virtual void setTitle()
    {
        m_renderer.setWindowTitle("BasicProject");
    }

protected:
    GLFWRenderer m_renderer;
    std::unique_ptr<Camera> m_camera;
    std::shared_ptr<Material> m_material;
    std::shared_ptr<Mesh> m_mesh;
    std::unique_ptr<WorldLights> m_worldLights;
    float m_deltaTime{ 0.0f };
    float m_totalTime{ 0.0f };
};