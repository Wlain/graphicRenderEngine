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
#include <utility>
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
    if (m_builder.m_gui)
    {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }
}

RenderPass::RenderPassBuilder::RenderPassBuilder(RenderStats* renderStats) :
    m_renderStats(renderStats)
{
}

RenderPass::RenderPassBuilder& RenderPass::RenderPassBuilder::withClearColor(bool enabled, Color color)
{
    if (Renderer::s_instance->renderInfo().useFramebufferSRGB)
    {
        auto col3 = glm::convertSRGBToLinear(glm::vec3(color.r, color.g, color.b));
        m_clearColorValue = { col3, color.a };
    }
    else
    {
        m_clearColorValue = { color.r, color.g, color.b, color.a };
    }
    m_clearColor = enabled;
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
    return RenderPassBuilder(&Renderer::s_instance->m_renderStatsCurrent);
}

RenderPass::~RenderPass()
{
    finish();
}

RenderPass::RenderPass(RenderPass&& rp) noexcept
{
    m_builder = rp.m_builder;
    std::swap(m_isFinished, rp.m_isFinished);
    std::swap(m_lastBoundShader, rp.m_lastBoundShader);
    std::swap(m_lastBoundMaterial, rp.m_lastBoundMaterial);
    std::swap(m_lastBoundMeshId, rp.m_lastBoundMeshId);
    std::swap(m_projection, rp.m_projection);
    std::swap(m_viewportOffset, rp.m_viewportOffset);
    std::swap(m_viewportSize, rp.m_viewportSize);
}

RenderPass& RenderPass::operator=(RenderPass&& rp) noexcept
{
    finish();
    m_builder = rp.m_builder;
    std::swap(m_isFinished, rp.m_isFinished);
    std::swap(m_lastBoundShader, rp.m_lastBoundShader);
    std::swap(m_lastBoundMaterial, rp.m_lastBoundMaterial);
    std::swap(m_lastBoundMeshId, rp.m_lastBoundMeshId);
    std::swap(m_projection, rp.m_projection);
    std::swap(m_viewportOffset, rp.m_viewportOffset);
    std::swap(m_viewportSize, rp.m_viewportSize);
    return *this;
}

void RenderPass::drawLines(const std::vector<glm::vec3>& vertices, Color color, Mesh::Topology meshTopology)
{
    ASSERT(!m_isFinished && "RenderPass is finished. Can no longer be modified.");
    auto mesh = Mesh::create()
                    .withPositions(vertices)
                    .withMeshTopology(meshTopology)
                    .build();
    auto material = Shader::getUnlit()->createMaterial();
    material->setColor(color);
    m_renderQueue.emplace_back(RenderQueueObj{ mesh, glm::mat4(1), { material } });
}

void RenderPass::draw(const std::shared_ptr<Mesh>& meshPtr, glm::mat4 modelTransform, std::shared_ptr<Material>& materialPtr)
{
    ASSERT(!m_isFinished && "RenderPass is finished. Can no longer be modified.");
    m_renderQueue.emplace_back(RenderQueueObj{ meshPtr, modelTransform, { materialPtr } });
}

void RenderPass::draw(std::shared_ptr<Mesh>& meshPtr, glm::mat4 modelTransform, std::vector<std::shared_ptr<Material>>& materials)
{
    ASSERT(!m_isFinished && "RenderPass is finished. Can no longer be modified.");
    ASSERT(meshPtr->m_indices.empty() || meshPtr->m_indices.size() == materials.size());
    m_renderQueue.emplace_back(RenderQueueObj{ meshPtr, modelTransform, materials });
}

void RenderPass::draw(std::shared_ptr<SpriteBatch>& spriteBatch, glm::mat4 modelTransform)
{
    ASSERT(!m_isFinished && "RenderPass is finished. Can no longer be modified.");
    if (spriteBatch == nullptr) return;

    for (int i = 0; i < spriteBatch->m_materials.size(); i++)
    {
        m_renderQueue.emplace_back(RenderQueueObj{ spriteBatch->m_spriteMeshes[i], modelTransform, { spriteBatch->m_materials[i] } });
    }
}

void RenderPass::draw(std::shared_ptr<SpriteBatch>&& spriteBatch, glm::mat4 modelTransform)
{
    ASSERT(!m_isFinished && "You can only invoke methods on the currently bound renderpass");
    if (spriteBatch == nullptr) return;

    for (int i = 0; i < spriteBatch->m_materials.size(); i++)
    {
        m_renderQueue.emplace_back(RenderQueueObj{ spriteBatch->m_spriteMeshes[i], modelTransform, { spriteBatch->m_materials[i] } });
    }
}

void RenderPass::blit(std::shared_ptr<Texture> texture, glm::mat4 transformation)
{
    auto material = Shader::getBlit()->createMaterial();
    material->setTexture(std::move(texture));
    blit(material, transformation);
}

void RenderPass::blit(std::shared_ptr<Material> material, glm::mat4 transformation)
{
    static std::shared_ptr<Mesh> mesh;
    static bool once = []() {
        mesh = Mesh::create().withQuad().build();
        // always render
        float m = std::numeric_limits<float>::max();
        std::array<glm::vec3, 2> minMax{};
        minMax[0] = glm::vec3{ -m };
        minMax[1] = glm::vec3{ m };
        mesh->setBoundsMinMax(minMax);
        return true;
    }();

    draw(mesh, transformation, material);
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
        if (shader->m_uniformLocationModelViewInverseTranspose != -1)
        {
            auto normalMatrix = transpose(inverse((glm::mat3)(m_builder.m_camera.getViewTransform() * modelTransform)));
            glUniformMatrix3fv(shader->m_uniformLocationModelViewInverseTranspose, 1, GL_FALSE, glm::value_ptr(normalMatrix));
        }
        if (shader->m_uniformLocationModelInverseTranspose != -1)
        {
            auto normalMatrix = transpose(inverse((glm::mat3)modelTransform));
            glUniformMatrix3fv(shader->m_uniformLocationModelInverseTranspose, 1, GL_FALSE, glm::value_ptr(normalMatrix));
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
        if (shader->m_uniformLocationModelViewInverseTranspose != -1)
        {
            auto normalMatrix = transpose(inverse((glm::mat3)(m_builder.m_camera.getViewTransform() * modelTransform)));
            glUniformMatrix3fv(shader->m_uniformLocationModelViewInverseTranspose, 1, GL_FALSE, glm::value_ptr(normalMatrix));
        }
        if (shader->m_uniformLocationModelInverseTranspose != -1)
        {
            auto normalMatrix = transpose(inverse((glm::mat3)modelTransform));
            glUniformMatrix3fv(shader->m_uniformLocationModelInverseTranspose, 1, GL_FALSE, glm::value_ptr(normalMatrix));
        }
        // viewport
        if (shader->m_uniformLocationViewport != -1)
        {
            glm::vec4 viewport((float)m_viewportSize.x, (float)m_viewportSize.y, (float)m_viewportOffset.x, (float)m_viewportOffset.y);
            glUniform4fv(shader->m_uniformLocationViewport, 1, glm::value_ptr(viewport));
        }
        if (shader->m_uniformLocationCameraPosition != -1)
        {
            glm::vec4 cameraPos = glm::vec4(m_builder.m_camera.getPosition(), 1.0f);
            glUniform4fv(shader->m_uniformLocationCameraPosition, 1, glm::value_ptr(cameraPos));
        }
        shader->setLights(m_builder.m_worldLights);
    }
}

void RenderPass::finish()
{
    if (m_isFinished) return;
    if (m_builder.m_framebuffer != nullptr)
    {
        m_builder.m_framebuffer->bind();
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    auto framebufferSize = (m_builder.m_framebuffer != nullptr) ? m_builder.m_framebuffer->m_size : static_cast<glm::vec2>(Renderer::s_instance->getFramebufferSize());
    m_viewportOffset = static_cast<glm::uvec2>(m_builder.m_camera.m_viewportOffset * framebufferSize);
    m_viewportSize = static_cast<glm::uvec2>(framebufferSize * m_builder.m_camera.m_viewportSize);
    glEnable(GL_SCISSOR_TEST);
    glScissor(m_viewportOffset.x, m_viewportOffset.y, m_viewportSize.x, m_viewportSize.y);
    glViewport(m_viewportOffset.x, m_viewportOffset.y, m_viewportSize.x, m_viewportSize.y);
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
    // 0u:表示无符号整型:0
    if (clear != 0u) glClear(clear);

    m_projection = m_builder.m_camera.getProjectionTransform(m_viewportSize);
    for (auto& rqObj : m_renderQueue)
    {
        drawInstance(rqObj);
    }
    if (m_builder.m_gui)
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    if (m_builder.m_framebuffer != nullptr)
    {
        for (auto& tex : m_builder.m_framebuffer->m_textures)
        {
            if (tex->m_info.generateMipmap)
            {
                glBindTexture(tex->m_info.target, tex->m_info.id);
                glGenerateMipmap(tex->m_info.target);
                glBindTexture(tex->m_info.target, 0);
            }
        }
    }
    m_isFinished = true;
#ifndef NDEBUG
    checkGlError();
#endif
}

std::vector<Color> RenderPass::readPixels(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
    ASSERT(m_isFinished);
    finish();
    if (m_builder.m_framebuffer != nullptr)
    {
        m_builder.m_framebuffer->bind();
    }
    std::vector<Color> res(width * height);
    std::vector<glm::u8vec4> resUnsigned(width * height);
    glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, resUnsigned.data());
    for (int i = 0; i < resUnsigned.size(); i++)
    {
        for (int j = 0; j < 4; j++)
        {
            res[i][j] = resUnsigned[i][j] / 255.0f;
        }
    }
    // set default framebuffer
    if (m_builder.m_framebuffer != nullptr)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    return res;
}

void RenderPass::finishGPUCommandBuffer() const
{
    glFlush();
}

bool RenderPass::isFinished()
{
    return m_isFinished;
}

void RenderPass::drawInstance(RenderPass::RenderQueueObj& rqObj)
{
    // todo optimize (mesh vbo only need to be bound once)
    for (int i = 0; i < rqObj.materials.size(); i++)
    {
        auto materialPtr = rqObj.materials[i];
        auto* mesh = rqObj.mesh.get();
        auto* material = materialPtr.get();
        auto* shader = material->getShader().get();
        assert(mesh != nullptr);
        m_builder.m_renderStats->drawCalls++;
        setupShader(rqObj.modelTransform, shader);
        if (material != m_lastBoundMaterial)
        {
            m_builder.m_renderStats->stateChangesMaterial++;
            m_lastBoundMaterial = material;
            m_lastBoundMeshId = -1; // force mesh to rebind
            material->bind();
        }
        if (mesh->m_meshId != m_lastBoundMeshId)
        {
            m_builder.m_renderStats->stateChangesMesh++;
            m_lastBoundMeshId = mesh->m_meshId;
        }
        mesh->bind(shader);
        if (mesh->getIndexSets() == 0)
        {
            glDrawArrays((GLenum)mesh->getMeshTopology(), 0, mesh->getVertexCount());
        }
        else
        {
            auto offsetCount = mesh->m_elementBufferOffsetCount[i];
            GLsizei indexCount = offsetCount.second;
            glDrawElements((GLenum)mesh->getMeshTopology(), indexCount, GL_UNSIGNED_SHORT, (char*)nullptr + offsetCount.first);
        }
    }
}

} // namespace re