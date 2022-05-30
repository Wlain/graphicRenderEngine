// Copyright (c) 2022. Lorem ipsum dolor sit amet, consectetur adipiscing elit.
// Morbi non lorem porttitor neque feugiat blandit. Ut vitae ipsum eget quam lacinia accumsan.
// Etiam sed turpis ac ipsum condimentum fringilla. Maecenas magna.
// Proin dapibus sapien vel ante. Aliquam erat volutpat. Pellentesque sagittis ligula eget metus.
// Vestibulum commodo. Ut rhoncus gravida arcu.

//
// Created by william on 2022/5/28.
//
#include "glfwRenderer.h"

#include "glCommonDefine.h"
#include "renderer.h"

#define SRE_DEBUG_CONTEXT 1
#ifdef SRE_DEBUG_CONTEXT
void GLAPIENTRY openglCallbackFunction(GLenum source,
                                       GLenum type,
                                       GLuint id,
                                       GLenum severity,
                                       GLsizei length,
                                       const GLchar* message,
                                       const void* userParam)
{
    LOG_INFO("---------------------opengl-callback-start------------");
    LOG_INFO("message: {}", message);
    LOG_INFO("type: ");
    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:
        LOG_ERROR("ERROR");
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        LOG_ERROR("DEPRECATED_BEHAVIOR");
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        LOG_ERROR("UNDEFINED_BEHAVIOR");
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        LOG_ERROR("PORTABILITY");
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        LOG_ERROR("PERFORMANCE");
        break;
    case GL_DEBUG_TYPE_OTHER:
        LOG_ERROR("OTHER");
        break;
    }

    LOG_INFO("id: {}", id);
    LOG_INFO("severity: ");
    switch (severity)
    {
    case GL_DEBUG_SEVERITY_LOW:
        LOG_ERROR("LOW");
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        LOG_ERROR("MEDIUM");
        break;
    case GL_DEBUG_SEVERITY_HIGH:
        LOG_ERROR("HIGH");
        break;
    }
    LOG_ERROR("---------------------opengl-callback-end--------------");
}
#endif

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
        glfwWindowHint(GLFW_CONTEXT_DEBUG, true);
#ifdef RE_DEBUG_CONTEXT
        glfwWindowHint(GLFW_SRGB_CAPABLE, true);
#endif
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
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
        if (m_window == nullptr)
        {
            glfwTerminate();
        }
        m_renderer = new Renderer(m_window);

#ifdef SRE_DEBUG_CONTEXT
        if (glDebugMessageCallback)
        {
            LOG_INFO("Register OpenGL debug callback ");
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(openglCallbackFunction, nullptr);
            GLuint unusedIds = 0;
            glDebugMessageControl(GL_DONT_CARE,
                                  GL_DONT_CARE,
                                  GL_DONT_CARE,
                                  0,
                                  &unusedIds,
                                  true);
        }
        else
        {
            LOG_INFO("glDebugMessageCallback not available");
        }
#endif
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

glm::ivec2 GLFWRenderer::getFramebuffeSize()
{
    return m_renderer->getFramebufferSize();
}

glm::ivec2 GLFWRenderer::getWindowSize()
{
    return m_renderer->getWindowSize();
}
} // namespace re