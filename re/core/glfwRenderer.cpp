//
// Created by william on 2022/5/28.
//
#include "glfwRenderer.h"

#include "glCommonDefine.h"
#include "renderer.h"

namespace re
{
GLFWRenderer::GLFWRenderer() :
    m_frameRender([]() {}),
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

void GLFWRenderer::init(bool vsync)
{
    if (m_running)
    {
        return;
    }
    if (!m_window)
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_DEBUG, true);
#ifdef RE_DEBUG_CONTEXT
        glfwWindowHint(GLFW_SRGB_CAPABLE, true);
#endif
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_DEPTH_BITS, re::Renderer::s_depthBits);
        glfwWindowHint(GLFW_STENCIL_BITS, re::Renderer::s_stencilBits);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
        // glfw window creation
        m_window = glfwCreateWindow(m_windowWidth, m_windowHeight, m_windowTitle.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(m_window, this);
        glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
            auto* app = reinterpret_cast<GLFWRenderer*>(glfwGetWindowUserPointer(window));
            app->m_frameResize(width, height);
        });
        glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xPos, double yPos) {
            auto* app = reinterpret_cast<GLFWRenderer*>(glfwGetWindowUserPointer(window));
            app->m_mouseEvent(xPos, yPos);
        });
        glfwSetDropCallback(m_window, [](GLFWwindow* window, int count, const char** paths) {
            auto* app = reinterpret_cast<GLFWRenderer*>(glfwGetWindowUserPointer(window));
            app->m_dropEvent(count, paths);
        });
        if (m_window == nullptr)
        {
            glfwTerminate();
        }
        m_renderer = new Renderer(m_window, vsync);
        glEnable(GL_FRAMEBUFFER_SRGB);
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

[[maybe_unused]] void GLFWRenderer::stopEventLoop()
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
    m_frameRender();
    m_renderer->swapWindow();
}

glm::ivec2 GLFWRenderer::getFrameBufferSize()
{
    return m_renderer->getFramebufferSize();
}

glm::ivec2 GLFWRenderer::getWindowSize()
{
    return m_renderer->getWindowSize();
}
} // namespace re