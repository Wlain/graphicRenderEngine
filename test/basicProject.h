//
// Created by william on 2022/5/29.
//
#include "core/glfwRenderer.h"
#include "core/material.h"
#include "core/Inspector.h"
#include "core/renderer.h"
#include "core/shader.h"
#include "core/worldLights.h"

using namespace re;

class BasicProject
{
public:
    BasicProject() :
        m_renderer{}, m_inspector{ 300, &m_renderer }
    {
        m_renderer.init();
    };

    virtual ~BasicProject() = default;
    virtual void run()
    {
        initialize();
        setTitle();
        m_renderer.setWindowTitle(m_title);
        m_renderer.m_frameUpdate = [&](float deltaTime) {
            update(deltaTime);
        };
        m_renderer.m_frameResize = [&](int width, int height) {
            resize(width, height);
        };
        m_renderer.m_mouseEvent = [&](int xPos, int yPos) {
            touchEvent(xPos, yPos);
        };
        m_renderer.m_dropEvent = [&](int count, const char** paths) {
            dropEvent(count, paths);
        };
        m_renderer.m_frameRender = [&]() {
            render();
        };
        m_renderer.startEventLoop();
    }
    virtual void initialize() {}
    virtual void resize(int width, int height) {}
    virtual void touchEvent(double xPos, double yPos) {}
    virtual void dropEvent(int count, const char** paths) {}
    virtual void render(){};
    virtual void update(float deltaTime)
    {
        m_deltaTime = deltaTime;
        m_totalTime += m_deltaTime;
    }
    virtual void setTitle() = 0;

protected:
    std::string m_title{ "BasicProject" };
    GLFWRenderer m_renderer;
    Inspector m_inspector;
    Camera m_camera;
    std::shared_ptr<Material> m_material;
    std::shared_ptr<Mesh> m_mesh;
    std::unique_ptr<WorldLights> m_worldLights;
    float m_deltaTime{ 0.0f };
    float m_totalTime{ 0.0f };
    bool m_enableProfiling{ true };
};