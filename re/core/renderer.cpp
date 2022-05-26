//
// Created by william on 2022/5/22.
//

#include "renderer.h"

#include "commonMacro.h"
#include "mesh.h"
#include "shader.h"

// render engine
namespace re
{
Renderer::Renderer(GLFWwindow* window) :
    m_window(window)
{
    s_instance = this;
    m_camera = &m_defaultCamera;
    glfwMakeContextCurrent(window);
    LOG_INFO("OpenGL version {}", glGetString(GL_VERSION));
    LOG_INFO("rg version {}.{}.{}", s_rgVersionMajor, s_rgVersionMinor, s_rgVersionPoint);
    // setup opengl context
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    glfwGetFramebufferSize(m_window, &m_camera->m_viewportWidth, &m_camera->m_viewportHeight);
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

void Renderer::setLight(int lightIndex, const Light& light)
{
    ASSERT(lightIndex >= 0);
    ASSERT(lightIndex < s_maxSceneLights);
    m_sceneLights[lightIndex] = light;
}

Light& Renderer::getLight(int lightIndex)
{
    ASSERT(lightIndex >= 0);
    ASSERT(lightIndex < s_maxSceneLights);
    return m_sceneLights[lightIndex];
}

void Renderer::render(Mesh* mesh, const glm::mat4& modelTransform, Shader* shader)
{
    if (m_camera == nullptr)
    {
        LOG_ERROR("Renderer::render:can not render,camera is null");
        return;
    }
    m_renderStatsCurrent.drawCalls++;
    setupShader(modelTransform, shader);
    mesh->bind();
    int indexCount = mesh->getIndices().size();
    if (indexCount == 0)
    {
        glDrawArrays((GLenum)mesh->topology(), 0, mesh->getVertexCount());
    }
    else
    {
        glDrawElements((GLenum)mesh->topology(), indexCount, GL_UNSIGNED_SHORT, 0);
    }
}

void Renderer::setCamera(Camera* camera)
{
    m_camera = camera;
    camera->setViewport(camera->m_viewportX, camera->m_viewportY, camera->m_viewportWidth, camera->m_viewportHeight);
}

void Renderer::clearScreen(const glm::vec4& color, bool clearColorBuffer, bool clearDepthBuffer)
{
    glClearColor(color.r, color.g, color.b, color.a);
    GLbitfield clear = 0;
    if (clearColorBuffer)
    {
        clear |= GL_COLOR_BUFFER_BIT;
    }
    if (clearDepthBuffer)
    {
        glDepthMask(GL_TRUE);
        clear |= GL_DEPTH_BUFFER_BIT;
    }
    glClear(clear);
}

void Renderer::swapWindow()
{
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_window, true);
    m_renderStatsLast = m_renderStatsCurrent;
    m_renderStatsCurrent.frame++;
    m_renderStatsCurrent.drawCalls = 0;
    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

void Renderer::setupShader(const glm::mat4& modelTransform, Shader* shader)
{
    shader->bind();
    if (shader->getType("model").type != Shader::UniformType::Invalid)
    {
        shader->set("model", modelTransform);
    }
    if (shader->getType("view").type != Shader::UniformType::Invalid)
    {
        shader->set("view", m_camera->getViewTransform());
    }
    if (shader->getType("projection").type != Shader::UniformType::Invalid)
    {
        shader->set("projection", m_camera->getProjectionTransform());
    }
    if (shader->getType("normalMat").type != Shader::UniformType::Invalid)
    {
        auto normalMatrix = transpose(inverse((glm::mat3)(m_camera->getViewTransform() * modelTransform)));
        shader->set("normalMat", normalMatrix);
    }
    if (shader->getType("viewHeight").type != Shader::UniformType::Invalid)
    {
        shader->set("viewHeight", (float)m_camera->m_viewportHeight);
    }
    shader->setLights(m_sceneLights, m_ambientLight, m_camera->getViewTransform());
}
} // namespace re