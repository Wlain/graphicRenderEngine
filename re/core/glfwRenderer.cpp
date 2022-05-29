// Copyright (c) 2022. Lorem ipsum dolor sit amet, consectetur adipiscing elit.
// Morbi non lorem porttitor neque feugiat blandit. Ut vitae ipsum eget quam lacinia accumsan.
// Etiam sed turpis ac ipsum condimentum fringilla. Maecenas magna.
// Proin dapibus sapien vel ante. Aliquam erat volutpat. Pellentesque sagittis ligula eget metus.
// Vestibulum commodo. Ut rhoncus gravida arcu.

//
// Created by william on 2022/5/28.
//
#include "glfwRenderer.h"

#include "renderer.h"

namespace re
{
GLFWRenderer::GLFWRenderer() :
    m_frameRender([](Renderer*) {}),
    m_frameUpdate([](float) {}),
    m_windowTitle("simpleRenderEngine" + std::to_string(Renderer::s_rgVersionMajor) + "." + std::to_string(Renderer::s_rgVersionMinor) + "." + std::to_string(Renderer::s_rgVersionPoint))
{
}

GLFWRenderer::~GLFWRenderer()
{
    delete m_renderer;
    m_renderer = nullptr;
    glfwTerminate();
}

void GLFWRenderer::init()
{
    if (m_running)
    {
        return;
    }
    if (!m_window)
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
        // glfw window creation
        m_window = glfwCreateWindow(m_windowWidth, m_windowHeight, m_windowTitle.c_str(), nullptr, nullptr);
        if (m_window == nullptr)
        {
            glfwTerminate();
        }
        m_renderer = new Renderer(m_window);
    }
}

void GLFWRenderer::setWindowTitle(std::string_view title)
{
    m_windowTitle = title;
    if (m_window != nullptr)
    {
        glfwSetWindowTitle(m_window, m_windowTitle.c_str());
    }
}

void GLFWRenderer::setWindowSize(int width, int height)
{
    m_windowWidth = width;
    m_windowHeight = height;
    if (m_window != nullptr)
    {
        glfwSetWindowSize(m_window, width, height);
    }
}

void GLFWRenderer::startEventLoop()
{
    if (!m_window)
    {
        throw std::runtime_error("GLFWRenderer::init() not called");
    }
    m_running = true;
    typedef std::chrono::high_resolution_clock Clock;
    using FpSeconds = std::chrono::duration<float, std::chrono::seconds::period>;
    auto lastTick = Clock::now();
    float deltaTime = 0;
    while (m_running)
    {
        frame(deltaTime);
        auto tick = Clock::now();
        deltaTime = std::chrono::duration_cast<FpSeconds>(tick - lastTick).count();
        lastTick = tick;
    }
}

void GLFWRenderer::stopEventLoop()
{
    m_running = false;
}

GLFWwindow* GLFWRenderer::getGlfwWindow()
{
    return m_window;
}

void GLFWRenderer::frame(float deltaTimeSec)
{
    if (glfwGetKey(m_window, GLFW_KEY_Q) == GLFW_PRESS || glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        m_running = false;
    }
    m_frameUpdate(deltaTimeSec);
    m_frameRender(m_renderer);
    m_renderer->swapWindow();
}

glm::ivec2 GLFWRenderer::getWindowSize()
{
    return m_renderer->getWindowSize();
}
} // namespace re