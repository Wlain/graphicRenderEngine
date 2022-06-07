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
    return RenderPass(*this);
}

RenderPass::RenderPass(RenderPass::RenderPassBuilder& builder) :
    m_builder(builder)
{
    bind(true);
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

RenderPass::RenderPassBuilder& RenderPass::RenderPassBuilder::withFramebuffer(std::shared_ptr<FrameBuffer> framebuffer)
{
    m_framebuffer = std::move(framebuffer);
    return *this;
}

RenderPass::RenderPassBuilder re::RenderPass::create()
{
    return { &Renderer::s_instance->m_renderStatsCurrent };
}

RenderPass::~RenderPass()
{
    if (RenderPass::s_instance == this)
    {
        finish();
        RenderPass::s_instance = nullptr;
    }
}

RenderPass::RenderPass(RenderPass&& rp)
{
    if (s_instance == &rp)
    {
        s_instance = this;
    }
    m_builder = rp.m_builder;
    std::swap(m_lastBoundShader, rp.m_lastBoundShader);
    std::swap(m_lastBoundMaterial, rp.m_lastBoundMaterial);
    std::swap(m_lastBoundMesh, rp.m_lastBoundMesh);
    std::swap(m_projection, rp.m_projection);
    std::swap(m_viewportOffset, rp.m_viewportOffset);
    std::swap(m_viewportSize, rp.m_viewportSize);
}

RenderPass& RenderPass::operator=(RenderPass&& rp)
{
    if (s_instance == &rp)
    {
        s_instance = this;
    }
    m_builder = rp.m_builder;
    std::swap(m_lastBoundShader, rp.m_lastBoundShader);
    std::swap(m_lastBoundMaterial, rp.m_lastBoundMaterial);
    std::swap(m_lastBoundMesh, rp.m_lastBoundMesh);
    std::swap(m_projection, rp.m_projection);
    std::swap(m_viewportOffset, rp.m_viewportOffset);
    std::swap(m_viewportSize, rp.m_viewportSize);
    return *this;
}

void RenderPass::drawLines(const std::vector<glm::vec3>& vertices, glm::vec4 color, Mesh::Topology meshTopology)
{
    ASSERT(s_instance == this && "You can only invoke methods on the currently bound renderpass");
    // 使用static变量，共享mesh
    static std::shared_ptr<Mesh> mesh = Mesh::create()
                                            .withPositions(vertices)
                                            .withMeshTopology(meshTopology)
                                            .build();
    static auto material = Shader::getUnlit()->createMaterial();
    material->setColor(color);
    // 更新共享的mesh
    mesh->update().withPositions(vertices).withMeshTopology(meshTopology).build();
    draw(mesh, glm::mat4(1), material);
}

void RenderPass::draw(const std::shared_ptr<Mesh>& meshPtr, glm::mat4 modelTransform, std::shared_ptr<Material>& materialPtr)
{
    ASSERT(s_instance == this && "You can only invoke methods on the currently bound renderpass");
    auto* mesh = meshPtr.get();
    auto* material = materialPtr.get();
    auto* shader = material->getShader().get();
    assert(mesh != nullptr);
    m_builder.m_renderStats->drawCalls++;
    setupShader(modelTransform, shader);
    if (material != m_lastBoundMaterial)
    {
        m_builder.m_renderStats->stateChangesMaterial++;
        m_lastBoundMaterial = material;
        m_lastBoundMesh = nullptr; // force mesh to rebind
        material->bind();
    }
    if (mesh != m_lastBoundMesh)
    {
        m_builder.m_renderStats->stateChangesMesh++;
        m_lastBoundMesh = mesh;
    }
    mesh->bind(shader);
    mesh->bindIndexSet(0);
    if (mesh->getIndexSets() == 0)
    {
        glDrawArrays((GLenum)mesh->getMeshTopology(), 0, mesh->getVertexCount());
    }
    else
    {
        auto indexCount = (GLsizei)mesh->getIndices(0).size();
        glDrawElements((GLenum)mesh->getMeshTopology(), indexCount, GL_UNSIGNED_SHORT, nullptr);
    }
}

void RenderPass::draw(std::shared_ptr<Mesh>& meshPtr, glm::mat4 modelTransform, std::vector<std::shared_ptr<Material>>& materials)
{
    ASSERT(s_instance == this && "You can only invoke methods on the currently bound renderpass");
    if (materials.size() == 1)
    {
        draw(meshPtr, modelTransform, materials[0]);
        return;
    }
    ASSERT(meshPtr->m_indices.size() == materials.size());
    auto* mesh = meshPtr.get();
    // todo optimize (mesh vbo only need to be bound once)
    for (int i = 0; i < materials.size(); i++)
    {
        auto* material = materials[i].get();
        auto* shader = material->getShader().get();

        assert(mesh != nullptr);
        m_builder.m_renderStats->drawCalls++;
        setupShader(modelTransform, shader);
        if (material != m_lastBoundMaterial)
        {
            m_builder.m_renderStats->stateChangesMaterial++;
            m_lastBoundMaterial = material;
            m_lastBoundMesh = nullptr; // force mesh to rebind
            material->bind();
        }
        if (mesh != m_lastBoundMesh)
        {
            m_builder.m_renderStats->stateChangesMesh++;
            m_lastBoundMesh = mesh;
            mesh->bind(shader);
            mesh->bindIndexSet(i);
        }
        auto indexCount = (GLsizei)mesh->getIndices(0).size();
        glDrawElements((GLenum)mesh->getMeshTopology(), indexCount, GL_UNSIGNED_SHORT, 0);
    }
}

void RenderPass::draw(std::shared_ptr<SpriteBatch>& spriteBatch, glm::mat4 modelTransform)
{
    ASSERT(s_instance == this && "You can only invoke methods on the currently bound renderpass");
    if (spriteBatch == nullptr) return;

    for (int i = 0; i < spriteBatch->m_materials.size(); i++)
    {
        draw(spriteBatch->m_spriteMeshes[i], modelTransform, spriteBatch->m_materials[i]);
    }
}

void RenderPass::draw(std::shared_ptr<SpriteBatch>&& spriteBatch, glm::mat4 modelTransform)
{
    ASSERT(s_instance == this && "You can only invoke methods on the currently bound renderpass");
    if (spriteBatch == nullptr) return;

    for (int i = 0; i < spriteBatch->m_materials.size(); i++)
    {
        draw(spriteBatch->m_spriteMeshes[i], modelTransform, spriteBatch->m_materials[i]);
    }
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
            auto normalMatrix = transpose(inverse((glm::mat3)(m_builder.m_camera.getViewTransform() * modelTransform)));
            glUniformMatrix3fv(shader->m_uniformLocationNormal, 1, GL_FALSE, glm::value_ptr(normalMatrix));
        }
    }
    else
    {
        shader->bind();
        m_builder.m_renderStats->stateChangesShader++;
        m_lastBoundShader = shader;
        // model
        if (shader->m_uniformLocationModel != -1)
        {
            glUniformMatrix4fv(shader->m_uniformLocationModel, 1, GL_FALSE, glm::value_ptr(modelTransform));
        }
        // view
        if (shader->m_uniformLocationView != -1)
        {
            glUniformMatrix4fv(shader->m_uniformLocationView, 1, GL_FALSE, glm::value_ptr(m_builder.m_camera.m_viewTransform));
        }
        // projection
        if (shader->m_uniformLocationProjection != -1)
        {
            glUniformMatrix4fv(shader->m_uniformLocationProjection, 1, GL_FALSE, glm::value_ptr(m_projection));
        }
        // normal
        if (shader->m_uniformLocationNormal != -1)
        {
            auto normalMatrix = transpose(inverse((glm::mat3)(m_builder.m_camera.getViewTransform() * modelTransform)));
            glUniformMatrix3fv(shader->m_uniformLocationNormal, 1, GL_FALSE, glm::value_ptr(normalMatrix));
        }
        // viewport
        if (shader->m_uniformLocationViewport != -1)
        {
            glm::vec4 viewport((float)m_viewportSize.x, (float)m_viewportSize.y, (float)m_viewportOffset.x, (float)m_viewportOffset.y);
            glUniform4fv(shader->m_uniformLocationViewport, 1, glm::value_ptr(viewport));
        }
        shader->setLights(m_builder.m_worldLights, m_builder.m_camera.getViewTransform());
    }
}

void RenderPass::finish()
{
    if (s_instance != nullptr)
    {
        s_instance->finishInstance();
        s_instance = nullptr;
    }
}

std::vector<glm::vec4> RenderPass::readPixels(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
    std::vector<glm::vec4> res(width * height);
    std::vector<glm::u8vec4> resUnsigned(width * height);
    glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, resUnsigned.data());
    for (int i = 0; i < resUnsigned.size(); i++)
    {
        for (int j = 0; j < 4; j++)
        {
            res[i][j] = resUnsigned[i][j] / 255.0f;
        }
    }
    return res;
}

void RenderPass::finishGPUCommandBuffer() const
{
    glFlush();
}

void RenderPass::bind(bool newFrame)
{
    if (RenderPass::s_instance != nullptr)
    {
        RenderPass::s_instance->finishInstance();
    }
    s_instance = this;
    if (m_builder.m_framebuffer != nullptr)
    {
        m_builder.m_framebuffer->bind();
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    auto framebufferSize = static_cast<glm::vec2>(Renderer::s_instance->getFramebufferSize());
    if (m_builder.m_framebuffer)
    {
        framebufferSize = m_builder.m_framebuffer->m_size;
    }
    m_viewportOffset = static_cast<glm::uvec2>(m_builder.m_camera.m_viewportOffset * framebufferSize);
    m_viewportSize = static_cast<glm::uvec2>(framebufferSize * m_builder.m_camera.m_viewportSize);
    m_projection = m_builder.m_camera.getProjectionTransform(framebufferSize);
    checkGLError();
    glEnable(GL_SCISSOR_TEST);
    glScissor(m_viewportOffset.x, m_viewportOffset.y, m_viewportSize.x, m_viewportSize.y);
    checkGLError();
    glViewport(m_viewportOffset.x, m_viewportOffset.y, m_viewportSize.x, m_viewportSize.y);
    checkGLError();
    if (newFrame)
    {
        GLbitfield clear = 0;
        if (m_builder.m_clearColor)
        {
            glClearColor(m_builder.m_clearColorValue.r, m_builder.m_clearColorValue.g, m_builder.m_clearColorValue.b, m_builder.m_clearColorValue.a);
            clear |= GL_COLOR_BUFFER_BIT;
        }
        if (m_builder.m_clearDepth)
        {
            glClearDepth(m_builder.m_clearDepthValue);
            glDepthMask(GL_TRUE);
            clear |= GL_DEPTH_BUFFER_BIT;
        }
        if (m_builder.m_clearStencil)
        {
            glClearStencil(m_builder.m_clearStencilValue);
            clear |= GL_STENCIL_BUFFER_BIT;
        }
        if (clear) glClear(clear);
        if (m_builder.m_gui)
        {
            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
        }
    }
}

void RenderPass::finishInstance()
{
    if (m_builder.m_gui)
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    if (s_lastFramebuffer)
    {
        for (auto& tex : s_lastFramebuffer->m_textures)
        {
            if (tex->m_info.generateMipmap)
            {
                glBindTexture(tex->m_info.target, tex->m_info.id);
                glGenerateMipmap(tex->m_info.target);
                glBindTexture(tex->m_info.target, 0);
            }
        }
    }
}

} // namespace re