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
    LOG_INFO("rg version {}.{}", s_rgVersionMajor, s_rgVersionMinor);
    // setup opengl context
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
    shader->bind();
    shader->set("model", modelTransform);
    shader->set("view", m_camera->getViewTransform());
    shader->set("projection", m_camera->getProjectionTransform());
    auto normalMatrix = glm::transpose(glm::inverse((glm::mat3)(m_camera->getViewTransform() * modelTransform)));
    shader->set("normalMat", normalMatrix);
    shader->setLights(m_sceneLights, m_ambientLight, m_camera->getViewTransform());
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
        clear |= GL_DEPTH_BUFFER_BIT;
    }
    glClear(clear);
}

void Renderer::swapWindow()
{
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_window, true);
    glfwSwapBuffers(m_window);
    glfwPollEvents();
}
} // namespace re