//
// Created by william on 2022/5/24.
//
#include "basicProject.h"
#include "commonMacro.h"
#include "guiCommonDefine.h"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

using namespace re;

class GuiExamples : public BasicProject
{
public:
    using BasicProject::BasicProject;
    ~GuiExamples() override = default;

    void initialize() override
    {
        m_camera = MAKE_UNIQUE(m_camera);
        m_camera->setLookAt({ 0, 0, 3 }, { 0, 0, 0 }, { 0, 1, 0 });
        m_camera->setPerspectiveProjection(60, 0.1, 100);
        auto shader = Shader::getStandard();
        m_material = shader->createMaterial();
        m_mesh = Mesh::create().withCube().build();
        m_worldLights = std::make_unique<WorldLights>();
        m_worldLights->addLight(Light::create().withPointLight({ 0, 0, 10 }).withColor({ 1, 0, 0 }).withRange(50).build());
    }

    void render() override
    {
        auto renderPass = RenderPass::create().withCamera(*m_camera).withWorldLights(m_worldLights.get()).withGUI(true).build();
        m_material->setSpecularity(m_specularity);
        renderPass.draw(m_mesh, glm::eulerAngleY(glm::radians(30.0f * m_totalTime)), m_material);
        ImGui::Begin("guiTest");
        ImGui::SliderFloat("specularity", &m_specularity, 0.0f, 40.0f);
        ImGui::ColorEdit3("clear color", (float*)&m_clearColor);
        ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        auto& renderStats = Renderer::s_instance->getRenderStats();
        float bytesToMB = 1.0f; /// (1024 * 1024);
        ImGui::Text("re draw-calls %i meshes %i (%.2fbytes) textures %i (%.2fbytes) shaders %i", renderStats.drawCalls,
                    renderStats.meshCount, renderStats.meshBytes * bytesToMB, renderStats.textureCount,
                    renderStats.textureBytes * bytesToMB, renderStats.shaderCount);
        ImGui::End();
    }

    void setTitle() override
    {
        m_title = "GuiExamples";
    }

private:
    float m_specularity = 20.0f;
    ImVec4 m_clearColor = ImColor(114, 144, 154);
};

void guiTest()
{
    GuiExamples test;
    test.run();
}