//
// Created by william on 2022/7/5.
//

#include "basicProject.h"
#include "core/modelImporter.h"
#include "guiCommonDefine.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>

using namespace re;

class StencilExample : public BasicProject
{
public:
    ~StencilExample() override = default;
    void initialize() override
    {
        m_camera.setLookAt({ 0, 0, 3.5f }, { 0, 0, 0 }, { 0, 1, 0 });
        m_camera.setPerspectiveProjection(45.0f, 0.1f, 100.0f);
        std::vector<std::shared_ptr<Material>> materialUnused;
        m_mesh = ModelImporter::importObj("resources/objFiles/suzanne.obj", materialUnused);
        m_plane = Mesh::create().withQuad(2).build();
        m_matStencilWrite = Shader::create()
                                .withSourceFile("embeddedResource/unlit_vert.glsl", Shader::ShaderType::Vertex)
                                .withSourceFile("embeddedResource/unlit_frag.glsl", Shader::ShaderType::Fragment)
                                .withDepthWrite(false)
                                .withColorWrite({ false, false, false, false })
                                .withStencil(Shader::Stencil{
                                    Shader::StencilFunc ::Always,
                                    1,
                                    1,
                                    Shader::StencilOp::Replace,
                                    Shader::StencilOp::Replace,
                                    Shader::StencilOp::Replace,
                                })
                                .withName("StencilWrite")
                                .build()
                                ->createMaterial();

        m_matStencilTest = Shader::create()
                               .withSourceFile("embeddedResource/unlit_vert.glsl", Shader::ShaderType::Vertex)
                               .withSourceFile("embeddedResource/unlit_frag.glsl", Shader::ShaderType::Fragment)
                               .withStencil(Shader::Stencil{
                                   Shader::StencilFunc ::LEqual,
                                   1,
                                   1 })
                               .withName("StencilClippedShadow")
                               .build()
                               ->createMaterial();
        m_matStencilTest->setColor({ 0.3f, 0.3f, 0.0f });
        m_material = Shader::getStandardPhong()->createMaterial();
        m_matShadow = Shader::getUnlit()->createMaterial();
        m_matShadow->setColor({ 0.3f, 0.3f, 0.3f });

        m_worldLights = MAKE_UNIQUE(m_worldLights);
        m_worldLights->setAmbientLight(glm::vec3{ 0.05f });
        m_worldLights->addLight(Light::create().withPointLight({ 0.5, 2, 0.5 }).build());
    }

    void render() override
    {
        auto rp = RenderPass::create()
                      .withCamera(m_camera)
                      .withClearColor(true, { 0, 0, 0, 1 })
                      .withClearStencil(true)
                      .withWorldLights(m_worldLights.get())
                      .build();

        auto pos = m_worldLights->getLight(0)->position;
        float bias = 0.01f;
        float y = pos.y - bias - m_shadowPlane;
        // 阴影投影
        glm::mat4 shadow = glm::transpose(glm::mat4(
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 1 / -y, 0, 0));
        glm::mat4 projectedShadow = glm::translate(pos) * shadow * glm::translate(-pos);
        auto modelMatrix = glm::eulerAngleY(glm::radians(30 * m_totalTime));
        if (m_drawShadow)
        {
            if (m_useStencil)
            {
                rp.draw(m_plane, glm::translate(glm::vec3{ 0, m_shadowPlane, 0 }) * glm::rotate(-glm::half_pi<float>(), glm::vec3{ 1, 0, 0 }), m_matStencilWrite);
                rp.draw(m_mesh, projectedShadow * modelMatrix, m_matStencilTest);
            }
            else
            {
                rp.draw(m_mesh, projectedShadow * modelMatrix, m_matShadow);
            }
        }
        // 绘制模型
        rp.draw(m_mesh, modelMatrix, m_material);
        // 绘制平面
        if (m_drawPlane)
        {
            rp.draw(m_plane, glm::translate(glm::vec3{ 0, m_shadowPlane, 0 }) * glm::rotate(-glm::half_pi<float>(), glm::vec3{ 1, 0, 0 }), m_material);
        }
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowContentSize(ImVec2(300, 120));
        ImGui::Begin("Shadow");
        ImGui::DragFloat("Ground height ", &m_shadowPlane, 0.1f);
        ImGui::Checkbox("Draw plane", &m_drawPlane);
        ImGui::Checkbox("Draw shadow", &m_drawShadow);
        ImGui::Checkbox("Use stencil", &m_useStencil);
        ImGui::DragFloat3("Light pos", &m_worldLights->getLight(0)->position.x, 0.1f);
        ImGui::End();
    }
    void setTitle() override
    {
        m_title = "StencilExample";
    }

private:
    std::shared_ptr<Mesh> m_plane;
    std::shared_ptr<Material> m_matStencilWrite;
    std::shared_ptr<Material> m_matStencilTest;
    std::shared_ptr<Material> m_matShadow;
    float m_shadowPlane = -1.0f;
    bool m_drawPlane = true;
    bool m_drawShadow = true;
    bool m_useStencil = true;
};

void stencilTest()
{
    StencilExample test;
    test.run();
}