//
// Created by william on 2022/5/23.
//

#include "commonMacro.h"
#include "core/worldLights.h"
#include "engineTestSimple.h"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include "guiCommonDefine.h"

#include <glm/gtx/euler_angles.hpp>

class multiLightExample : public CommonInterface
{
public:
    using CommonInterface::CommonInterface;
    ~multiLightExample() override = default;

    void initialize() override
    {
        m_camera.setLookAt(m_eye, m_at, m_up);
        m_camera.setPerspectiveProjection(60, 0.1, 100);
        m_mesh = Mesh::create().withCube().build();
        m_meshSphere = Mesh::create().withSphere().build();
        m_worldLights = MAKE_UNIQUE(m_worldLights);
        LOG_ERROR("cwb:lightCount:{}", Renderer::s_instance->getMaxSceneLights());
        for (int i = 0; i < Renderer::s_instance->getMaxSceneLights(); i++)
        {
            m_worldLights->addLight(Light::create()
                                        .withPointLight({ (float(rand()) / RAND_MAX) * 2 - 1, 0.02f, (float(rand()) / RAND_MAX) * 2 - 1 })
                                        .withColor({ (float(rand()) / RAND_MAX) * 0.75f, (float(rand()) / RAND_MAX) * 0.75f, (float(rand()) / RAND_MAX) * 0.75f })
                                        .withRange(10)
                                        .build());
        }
        m_materials[0] = Shader::getStandardBlinnPhong()->createMaterial();
        m_materials[1] = Shader::getStandardPhong()->createMaterial();
        m_materials[2] = Shader::getStandardPBR()->createMaterial();
    }

    void drawCross(RenderPass& rp, const glm::vec3& p, float size = 0.3f)
    {
        rp.drawLines({ p - glm::vec3{ size, 0, 0 },
                       p + glm::vec3{ size, 0, 0 },
                       p - glm::vec3{ 0, size, 0 },
                       p + glm::vec3{ 0, size, 0 },
                       p - glm::vec3{ 0, 0, size },
                       p + glm::vec3{ 0, 0, size } },
                     { 0, 1, 0, 1 });
    }

    void drawLight(RenderPass& rp, Light* l, float size)
    {
        if (l->type == Light::Type::Point)
        {
            drawCross(rp, l->position, size);
        }
        if (l->type == Light::Type::Directional)
        {
            rp.drawLines({ l->position, l->position - l->direction * size * 2.0f }, { 1, 1, 0, 1 });
        }
    }

    void update(float deltaTime) override
    {
        m_materials[m_selectedMaterial]->setSpecularity(m_specularity);
        m_materials[m_selectedMaterial]->setColor(m_color);
        if (m_animatedCamera)
        {
            m_eye = {
                sin(m_totalTime * -0.2) * 5.0f,
                sin(m_totalTime * -0.4) * 0.5f,
                cos(m_totalTime * -0.2) * 5.0f,
            };
        }
        m_camera.setLookAt(m_eye, m_at, m_up);
        if (m_animatedLight)
        {
            for (int i = 0; i < m_worldLights->lightCount(); ++i)
            {
                m_worldLights->getLight(i)->position = {
                    sin(m_totalTime + i * i) * 1.5f,
                    sin(m_totalTime * 2 + i * i) * 0.5f,
                    cos(m_totalTime + i * i) * 1.5f,
                };
            }
        }
        CommonInterface::update(deltaTime);
    }

    void render() override
    {
        /// 渲染
        auto renderPass = RenderPass::create().withCamera(m_camera).withWorldLights(m_worldLights.get()).build();
        drawCross(renderPass, { 2, 2, 2 });
        drawCross(renderPass, { -2, -2, -2 });
        // Show Label (with invisible window)
        for (int i = 0; i < m_worldLights->lightCount(); i++)
        {
            auto l = m_worldLights->getLight(i);
            if (m_debugLight)
            {
                drawLight(renderPass, l, m_debugLightSize);
            }
        }
        renderPass.draw(m_drawSphere ? m_meshSphere : m_mesh, glm::eulerAngleY(m_totalTime), m_materials[m_selectedMaterial]);
        ImGui::Checkbox("is point light ", &m_isPointLight);
        ImGui::DragFloat3("Camera", &m_eye.x);
        ImGui::Checkbox("AnimatedLight", &m_animatedLight);
        ImGui::Checkbox("AnimatedCamera", &m_animatedCamera);
        ImGui::Checkbox("AnimatedObject", &m_animatedObject);
        ImGui::Checkbox("DebugLight", &m_debugLight);
        if (m_debugLight)
        {
            ImGui::DragFloat("DebugLightSize", &m_debugLightSize, 0.1f, 0, 3);
        }
        ImGui::Checkbox("Draw Sphere", &m_drawSphere);

        // Show Label (with invisible window)
        for (int i = 0; i < Renderer::s_instance->getMaxSceneLights(); i++)
        {
            auto l = m_worldLights->getLight(i);
            if (m_debugLight)
            {
                drawLight(renderPass, l, m_debugLightSize);
            }
            std::string lightLabel = "Light ";
            lightLabel += std::to_string(i + 1);
            if (ImGui::TreeNode(lightLabel.c_str()))
            {
                auto lightType = (int)l->type;
                ImGui::RadioButton("Point", &lightType, 0);
                ImGui::SameLine();
                ImGui::RadioButton("Directional", &lightType, 1);
                ImGui::SameLine();
                ImGui::RadioButton("Unused", &lightType, 2);
                l->type = (Light::Type)lightType;

                ImGui::ColorEdit3("Color", &(l->color.x));
                ImGui::DragFloat3("Position", &(l->position.x));
                ImGui::DragFloat3("Direction", &(l->direction.x));
                ImGui::DragFloat("Range", &(l->range), 1, 0, 30);
                ImGui::TreePop();
            }
        }

        if (ImGui::TreeNode("Material"))
        {
            constexpr const char* options = "BlinnPhong\0Phong\0PBR\0";
            ImGui::Combo("Shader", &m_selectedMaterial, options);
            if (m_selectedMaterial < 2)
            {
                ImGui::DragFloat4("Specularity", &m_specularity.r, 0.1, 0, 1);
                m_materials[m_selectedMaterial]->setSpecularity(m_specularity);
            }
            else
            {
                ImGui::DragFloat("Metallic", &m_metalRoughness.x, 0.1, 0, 1);
                ImGui::DragFloat("Roughness", &m_metalRoughness.y, 0.1, 0, 1);
                m_materials[m_selectedMaterial]->setMetallicRoughness(m_metalRoughness);
            }
            auto col = m_color.toLinear();
            if (ImGui::ColorEdit3("Color", &(col.x)))
            {
                m_color.setFromLinear(col);
            }

            ImGui::TreePop();
        }

        if (m_debug)
        {
            m_inspector.update();
            m_inspector.gui();
        }
    }

private:
    glm::vec3 m_eye{ 0, 0, 5 };
    glm::vec3 m_at{ 0, 0, 0 };
    glm::vec3 m_up{ 0, 1, 0 };
    int m_selectedMaterial = 0;
    std::shared_ptr<Material> m_materials[3];
    std::shared_ptr<Mesh> m_meshSphere;
    Color m_specularity = { 1, 1, 1, 20 };
    Color m_color = { 1, 1, 1, 1 };
    glm::vec2 m_metalRoughness{ 0.5f, 0.5f };
    float m_debugLightSize = 0.2;
    bool m_isPointLight{ true };
    bool m_debugLight = true;
    bool m_animatedLight = true;
    bool m_animatedCamera = true;
    bool m_animatedObject = true;
    bool m_drawSphere = true;
};

void multiLightTest()
{
    GLFWRenderer renderer{};
    EngineTestSimple test(renderer);
    auto sceneNodeEffect = std::make_shared<multiLightExample>(&renderer);
    auto effect = std::make_shared<EffectManager>();
    effect->insertEffect(sceneNodeEffect);
    test.setEffect(effect);
    test.setTitle("multiLightExample");
    test.run();
}