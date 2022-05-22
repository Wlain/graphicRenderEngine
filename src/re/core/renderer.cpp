//
// Created by william on 2022/5/22.
//

#include "renderer.h"

#include "commonMacro.h"
#include "mesh.h"
#include "shader.h"

#include <iostream>

// render engine
namespace re
{
Renderer::Renderer(GLFWwindow* window) :
    m_window(window)
{
    s_instance = this;
    m_camera = &m_defaultCamera;
    glfwMakeContextCurrent(window);
    std::cout << "OpenGL version " << glGetString(GL_VERSION) << std::endl;
    std::cout << "rg version " << s_rgVersionMajor << "." << s_rgVersionMinor << std::endl;
    // setup opengl context
    glEnable(GL_CULL_FACE);
}

Renderer::~Renderer() = default;

void Renderer::setLight(int lightIndex, Light light)
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
    shader->setMatrix("model", modelTransform);
    shader->setMatrix("view", m_camera->getViewTransform());
    shader->setMatrix("projection", m_camera->getProjectionTransform());
    auto normalMatrix = glm::transpose(glm::inverse((glm::mat3)(m_camera->getViewTransform() * modelTransform)));
    shader->setMatrix("normalMat", normalMatrix);
    shader->setLights(m_sceneLights);
    mesh->bind();
    glDrawArrays((GLenum)mesh->getMeshTopology(), 0, mesh->getVertexCount());
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