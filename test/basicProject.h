//
// Created by william on 2022/5/29.
//
#include "core/Inspector.h"
#include "core/glfwRenderer.h"
#include "core/material.h"
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
        m_renderer.m_cursorPosEvent = [&](int xPos, int yPos) {
            cursorPosEvent(xPos, yPos);
        };
        m_renderer.m_mouseButtonEvent = [&](int button, int action, int mods) {
            mouseButtonEvent(button, action, mods);
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
    virtual void cursorPosEvent(double xPos, double yPos)
    {
        if (m_leftMousePressed)
        {
            auto framebufferSize = m_renderer.getFrameBufferSize();
            auto windowsSize = m_renderer.getWindowSize();
            auto ratio = framebufferSize.x / windowsSize.x;
            // 坐标映射:左下角(0, 0), 右上角（width, height）
            m_mousePos.x = std::clamp((int)(xPos * ratio), 0, windowsSize.x * (int)ratio);
            m_mousePos.y = std::clamp((int)(windowsSize.y - yPos) * (int)ratio, 0, windowsSize.y * (int)ratio);
            float mouseSpeed = 1.0f / 50.0f;
            m_rotate.x = m_mousePos.x * mouseSpeed;
            m_rotate.y = m_mousePos.y * mouseSpeed;
        }
    }
    // button: left or right, action:pressed or released
    virtual void mouseButtonEvent(int button, int action, int mods)
    {
        m_leftMousePressed = action == GLFW_PRESS && button >= 0;
    }
    virtual void dropEvent(int count, const char** paths) {}
    virtual void render(){};
    virtual void update(float deltaTime)
    {
        m_deltaTime = deltaTime;
        m_totalTime += m_deltaTime;
        if (glfwGetKey(m_renderer.getGlfwWindow(), GLFW_KEY_D) == GLFW_PRESS && !m_debugKeyPressed)
        {
            m_debug = !m_debug;
            m_debugKeyPressed = true;
        }
        if (glfwGetKey(m_renderer.getGlfwWindow(), GLFW_KEY_D) == GLFW_RELEASE)
        {
            m_debugKeyPressed = false;
        }
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
    glm::vec2 m_rotate{ 0.0f, 0.0f };
    glm::vec2 m_mousePos{0.0f, 0.0f};
    float m_deltaTime{ 0.0f };
    float m_totalTime{ 0.0f };
    bool m_enableProfiling{ true };
    bool m_debug{ false };
    bool m_debugKeyPressed{ false };
    bool m_leftMousePressed{ false };
};