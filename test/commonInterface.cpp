//
// Created by cwb on 2022/7/25.
//

#include "commonInterface.h"

#include "core/glfwRenderer.h"

CommonInterface::CommonInterface(GLFWRenderer* renderer) :
    m_renderer(renderer), m_inspector(300, m_renderer)
{
    m_title = "CommonInterface";
}

CommonInterface::~CommonInterface() = default;

void CommonInterface::initialize()
{
}

void CommonInterface::resize(int width, int height)
{
}

void CommonInterface::update(float deltaTime)
{
    m_deltaTime = deltaTime;
    m_totalTime += m_deltaTime;
    if (glfwGetKey(m_renderer->getGlfwWindow(), GLFW_KEY_D) == GLFW_PRESS && !m_debugKeyPressed)
    {
        m_debug = !m_debug;
        m_debugKeyPressed = true;
    }
    if (glfwGetKey(m_renderer->getGlfwWindow(), GLFW_KEY_D) == GLFW_RELEASE)
    {
        m_debugKeyPressed = false;
    }
}

void CommonInterface::render()
{
}

void CommonInterface::finalize()
{
}

void CommonInterface::setTitle()
{
    m_renderer->setWindowTitle(m_title);
}

void CommonInterface::cursorPosEvent(double xPos, double yPos)
{
    if (m_leftMousePressed)
    {
        auto framebufferSize = m_renderer->getFrameBufferSize();
        auto windowsSize = m_renderer->getWindowSize();
        auto ratio = framebufferSize.x / windowsSize.x;
        // 坐标映射:左下角(0, 0), 右上角（width, height）
        m_mousePos.x = std::clamp((int)(xPos * ratio), 0, windowsSize.x * (int)ratio);
        m_mousePos.y = std::clamp((int)(windowsSize.y - yPos) * (int)ratio, 0, windowsSize.y * (int)ratio);
        float mouseSpeed = 1.0f / 50.0f;
        m_rotate.x = m_mousePos.x * mouseSpeed;
        m_rotate.y = m_mousePos.y * mouseSpeed;
    }
}

void CommonInterface::mouseButtonEvent(int button, int action, int mods)
{
    m_leftMousePressed = action == GLFW_PRESS && button >= 0;
}

void CommonInterface::dropEvent(int count, const char** paths)
{
}