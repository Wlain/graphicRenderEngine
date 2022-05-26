//
// Created by william on 2022/5/26.
//

#include "renderPass.h"

#include "glCommonDefine.h"
#include "renderStats.h"
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
    shader->set("color", color);
    shader->set("tex", Texture::getWhiteTexture());
    draw(mesh, glm::mat4(1), shader);
    delete mesh;
}

void RenderPass::draw(Mesh* mesh, glm::mat4 modelTransform, Shader* shader)
{
    m_renderStats->drawCalls++;
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

RenderPass::RenderPass(Camera&& camera, WorldLights* worldLights, RenderStats* renderStats) :
    m_camera(camera), m_worldLights(worldLights), m_renderStats(renderStats)
{
    m_currentRenderPass = this;
    glViewport(m_camera.m_viewportX, m_camera.m_viewportY, m_camera.m_viewportWidth, m_camera.m_viewportHeight);
    glScissor(m_camera.m_viewportX, m_camera.m_viewportY, m_camera.m_viewportWidth, m_camera.m_viewportHeight);
}

void RenderPass::setupShader(const glm::mat4& modelTransform, Shader* shader)
{
    shader->bind();
    if (shader->getType("model").type != Shader::UniformType::Invalid)
    {
        shader->set("model", modelTransform);
    }
    if (shader->getType("view").type != Shader::UniformType::Invalid)
    {
        shader->set("view", m_camera.getViewTransform());
    }
    if (shader->getType("projection").type != Shader::UniformType::Invalid)
    {
        shader->set("projection", m_camera.getProjectionTransform());
    }
    if (shader->getType("normalMat").type != Shader::UniformType::Invalid)
    {
        auto normalMatrix = transpose(inverse((glm::mat3)(m_camera.getViewTransform() * modelTransform)));
        shader->set("normalMat", normalMatrix);
    }
    if (shader->getType("viewHeight").type != Shader::UniformType::Invalid)
    {
        shader->set("viewHeight", (float)m_camera.m_viewportHeight);
    }
    shader->setLights(m_worldLights, m_camera.getViewTransform());
}
} // namespace re