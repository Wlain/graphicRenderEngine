//
// Created by william on 2022/5/26.
//

#include "renderPass.h"

#include "glCommonDefine.h"
#include "guiCommonDefine.h"
#include "material.h"
#include "renderStats.h"
#include "renderer.h"

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
    GLbitfield clear = 0;
    if (m_clearColor)
    {
        glClearColor(m_clearColorValue.r, m_clearColorValue.g, m_clearColorValue.b, m_clearColorValue.a);
        clear |= GL_COLOR_BUFFER_BIT;
    }
    if (m_clearDepth)
    {
        glClearDepth(m_clearDepthValue);
        glDepthMask(GL_TRUE);
        clear |= GL_DEPTH_BUFFER_BIT;
    }
    if (m_clearStencil)
    {
        glClearStencil(m_clearStencilValue);
        clear |= GL_STENCIL_BUFFER_BIT;
    }
    if (clear) glClear(clear);
    if (m_gui)
    {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }
    return { std::move(m_camera), m_worldLights, m_renderStats, m_gui };
}

RenderPass::RenderPassBuilder::RenderPassBuilder(RenderStats* renderStats) :
    m_renderStats(renderStats)
{
}

RenderPass::RenderPassBuilder& RenderPass::RenderPassBuilder::withClearColor(bool enabled, glm::vec4 color)
{
    m_clearColor = enabled;
    m_clearColorValue = color;
    return *this;
}

RenderPass::RenderPassBuilder& RenderPass::RenderPassBuilder::withClearDepth(bool enabled, float value)
{
    m_clearDepthValue = enabled;
    m_clearDepthValue = value;
    return *this;
}
RenderPass::RenderPassBuilder& RenderPass::RenderPassBuilder::withClearStencil(bool enabled, int value)
{
    m_clearStencil = enabled;
    m_clearStencilValue = value;
    return *this;
}
RenderPass::RenderPassBuilder& RenderPass::RenderPassBuilder::withGUI(bool enabled)
{
    m_gui = enabled;
    return *this;
}

RenderPass::RenderPassBuilder re::RenderPass::create()
{
    return {};
}

RenderPass::~RenderPass() = default;

void RenderPass::drawLines(const std::vector<glm::vec3>& vertices, glm::vec4 color, Mesh::Topology meshTopology)
{
    ASSERT(m_instance != nullptr);
    // 使用static变量，共享mesh
    static auto* mesh = Mesh::create()
                            .withPositions(vertices)
                            .withMeshTopology(meshTopology)
                            .build();
    // 使用static变量，共享shader
    static auto* shader = Shader::create()
                              .withSourceUnlit()
                              .build();
    static Material material{ shader };
    material.setColor(color);
    // 更新共享的mesh
    mesh->update().withPositions(vertices).withMeshTopology(meshTopology).build();
    draw(mesh, glm::mat4(1), &material);
}

void RenderPass::draw(Mesh* mesh, glm::mat4 modelTransform, Material* material)
{
    ASSERT(m_instance != nullptr);
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
    }
    mesh->bind(material->getShader());
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

RenderPass::RenderPass(Camera&& camera, WorldLights* worldLights, RenderStats* renderStats, bool gui) :
    m_camera(camera), m_worldLights(worldLights), m_renderStats(renderStats), m_gui(gui)
{
    if (m_instance) m_instance->finish();
    //    glEnable(GL_SCISSOR_TEST);
    glScissor(m_camera.m_viewportX, m_camera.m_viewportY, m_camera.m_viewportWidth, m_camera.m_viewportHeight);
    glViewport(m_camera.m_viewportX, m_camera.m_viewportY, m_camera.m_viewportWidth, m_camera.m_viewportHeight);
    m_instance = this;
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
            glm::vec4 viewport((float)m_camera.m_viewportWidth, (float)m_camera.m_viewportHeight, (float)m_camera.m_viewportX, (float)m_camera.m_viewportY);
            glUniform4fv(shader->m_uniformLocationViewport, 1, glm::value_ptr(viewport));
        }
        shader->setLights(m_worldLights, m_camera.getViewTransform());
    }
}

void RenderPass::finish()
{
    ASSERT(m_instance != nullptr);
    if (m_gui)
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
    m_instance = nullptr;
}
} // namespace re