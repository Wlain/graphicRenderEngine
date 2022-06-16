//
// Created by william on 2022/5/22.
//

#include "renderer.h"

#include "commonMacro.h"
#include "guiCommonDefine.h"
// render engine
namespace re
{
Renderer::Renderer(GLFWwindow* window, bool vsync) :
    m_window(window), m_vsync(vsync)
{
    if (s_instance != nullptr)
    {
        LOG_ERROR("multiple versions of Renderer initialized. Only a single instance is supported.");
    }
    s_instance = this;
    glfwMakeContextCurrent(window);
    if (m_vsync)
    {
        glfwSwapInterval(1); // 开启垂直同步
    }
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        exit(EXIT_FAILURE);
        /* Problem: glewInit failed, something is seriously wrong. */
        LOG_FATAL("Error initializing OpenGL using GLEW: {}", glewGetErrorString(err));
    }
    // initialize ImGUI
    ImGui::CreateContext();
    // 设置平台和渲染器
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    LOG_INFO("OpenGL version {}", glGetString(GL_VERSION));
    LOG_INFO("rg version {}.{}.{}", s_rgVersionMajor, s_rgVersionMinor, s_rgVersionPoint);
    // setup opengl context
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    // reset render stats
    m_renderStatsCurrent.frame = 0;
    m_renderStatsCurrent.meshCount = 0;
    m_renderStatsCurrent.meshBytes = 0;
    m_renderStatsCurrent.textureCount = 0;
    m_renderStatsCurrent.textureBytes = 0;
    m_renderStatsCurrent.drawCalls = 0;
    m_renderStatsLast = m_renderStatsCurrent;
}

Renderer::~Renderer() = default;

void Renderer::swapWindow()
{
    m_renderStatsLast = m_renderStatsCurrent;
    m_renderStatsCurrent.frame++;
    m_renderStatsCurrent.drawCalls = 0;
    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

glm::ivec2 Renderer::getFramebufferSize()
{
    glm::ivec2 size;
    glfwGetFramebufferSize(m_window, &size.r, &size.g);
    return size;
}

glm::ivec2 Renderer::getWindowSize()
{
    glm::ivec2 size;
    glfwGetWindowSize(m_window, &size.r, &size.g);
    return size;
}

int Renderer::getMaxSceneLights()
{
    return maxSceneLights;
}

const Renderer::RenderInfo& re::Renderer::getRenderInfo()
{
    return m_renderInfo;
}
} // namespace re