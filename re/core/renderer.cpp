//
// Created by william on 2022/5/22.
//

#include "renderer.h"

#include "commonMacro.h"

// render engine
namespace re
{
Renderer::Renderer(GLFWwindow* window) :
    m_window(window)
{
    s_instance = this;
    glfwMakeContextCurrent(window);
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

RenderPass::RenderPassBuilder Renderer::createRenderPass()
{
    return { &m_renderStatsCurrent };
}

void Renderer::swapWindow()
{
    if (RenderPass::m_instance)
    {
        RenderPass::m_instance->finish();
    }
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_window, true);
    m_renderStatsLast = m_renderStatsCurrent;
    m_renderStatsCurrent.frame++;
    m_renderStatsCurrent.drawCalls = 0;
    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

glm::ivec2 Renderer::getWindowSize()
{
    {
        glm::ivec2 size;
        glfwGetFramebufferSize(m_window, & size.r, &size.g);
        return size;
    }
}
} // namespace re