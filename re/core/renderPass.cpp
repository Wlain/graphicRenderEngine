//
// Created by william on 2022/5/26.
//

#include "renderPass.h"

#include "glCommonDefine.h"
#include "material.h"
#include "renderStats.h"

#include <glm/gtc/type_ptr.hpp>
namespace re
{
RenderPass::RenderPassBuilder& RenderPass::RenderPassBuilder::withName(const std::string& name)
{
    m_name = name;
    return *this;
}

RenderPass::RenderPassBuilder& RenderPass::RenderPassBuilder::withCamera(const Camera& camera)
{
    m_camera = camera;
    return *this;
}

RenderPass::RenderPassBuilder& RenderPass::RenderPassBuilder::withWorldLights(WorldLights* worldLights)
{
    m_worldLights = worldLights;
    return *this;
}

RenderPass RenderPass::RenderPassBuilder::build()
{
    return { std::move(m_camera), m_worldLights, m_renderStats };
}

RenderPass::RenderPassBuilder::RenderPassBuilder(RenderStats* renderStats) :
    m_renderStats(renderStats)
{
}

RenderPass::RenderPassBuilder re::RenderPass::create()
{
    return {};
}

RenderPass::~RenderPass()
{
    if (m_currentRenderPass == this)
    {
        m_currentRenderPass = nullptr;
    }
}

void RenderPass::clearScreen(glm::vec4 color, bool clearColorBuffer, bool clearDepthBuffer, bool clearStencil)
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

void RenderPass::drawLines(const std::vector<glm::vec3>& vertices, glm::vec4 color, Mesh::Topology meshTopology)
{
    auto* mesh = Mesh::create()
                     .withVertexPosition(vertices)
                     .withMeshTopology(meshTopology)
                     .build();
    auto* shader = Shader::create()
                       .withSourceUnlit()
                       .build();
    static Material material{ shader };
    material.setColor(color);
    draw(mesh, glm::mat4(1), &material);
    delete mesh;
}

void RenderPass::draw(Mesh* mesh, glm::mat4 modelTransform, Material* material)
{
    m_renderStats->drawCalls++;
    setupShader(modelTransform, material->getShader());
    if (material != m_lastBoundMaterial)
    {
        m_renderStats->stateChangesMaterial++;
        m_lastBoundMaterial = material;
        material->bind();
    }
    if (mesh != m_lastBoundMesh)
    {
        m_renderStats->stateChangesMesh++;
        m_lastBoundMesh = mesh;
        mesh->bind();
    }
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

RenderPass::RenderPass(Camera&& camera, WorldLights* worldLights, RenderStats* renderStats) :
    m_camera(camera), m_worldLights(worldLights), m_renderStats(renderStats)
{
    m_currentRenderPass = this;
    glViewport(m_camera.m_viewportX, m_camera.m_viewportY, m_camera.m_viewportWidth, m_camera.m_viewportHeight);
    glScissor(m_camera.m_viewportX, m_camera.m_viewportY, m_camera.m_viewportWidth, m_camera.m_viewportHeight);
}

void RenderPass::setupShader(const glm::mat4& modelTransform, Shader* shader)
{

    if (m_lastBoundShader == shader)
    {
        // model
        if (shader->m_uniformLocationModel != -1)
        {
            glUniformMatrix4fv(shader->m_uniformLocationModel, 1, GL_FALSE, glm::value_ptr(modelTransform));
        }
        // normal
        if (shader->m_uniformLocationNormal != -1)
        {
            auto normalMatrix = transpose(inverse((glm::mat3)(m_camera.getViewTransform() * modelTransform)));
            glUniformMatrix3fv(shader->m_uniformLocationNormal, 1, GL_FALSE, glm::value_ptr(normalMatrix));
        }
    }
    else
    {
        shader->bind();
        m_renderStats->stateChangesShader++;
        m_lastBoundShader = shader;
        // model
        if (shader->m_uniformLocationModel != -1)
        {
            glUniformMatrix4fv(shader->m_uniformLocationModel, 1, GL_FALSE, glm::value_ptr(modelTransform));
        }
        // view
        if (shader->m_uniformLocationView != -1)
        {
            glUniformMatrix4fv(shader->m_uniformLocationView, 1, GL_FALSE, glm::value_ptr(m_camera.m_viewTransform));
        }
        // projection
        if (shader->m_uniformLocationProjection != -1)
        {
            glUniformMatrix4fv(shader->m_uniformLocationProjection, 1, GL_FALSE, glm::value_ptr(m_camera.m_projectionTransform));
        }
        // normal
        if (shader->m_uniformLocationNormal != -1)
        {
            auto normalMatrix = transpose(inverse((glm::mat3)(m_camera.getViewTransform() * modelTransform)));
            glUniformMatrix3fv(shader->m_uniformLocationNormal, 1, GL_FALSE, glm::value_ptr(normalMatrix));
        }
        // viewport
        if (shader->m_uniformLocationViewport != -1)
        {
            glm::vec4 viewport((float)m_camera.m_viewportWidth, (float)m_camera.m_viewportHeight, 0, 0);
            glUniform4fv(shader->m_uniformLocationViewport, 1, glm::value_ptr(viewport));
        }
        shader->setLights(m_worldLights, m_camera.getViewTransform());
    }
}
} // namespace re