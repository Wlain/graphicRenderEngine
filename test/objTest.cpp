//
// Created by william on 2022/5/31.
//

#include "engineTestSimple.h"
#include "core/modelImporter.h"
#include "guiCommonDefine.h"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

class ObjExample : public CommonInterface
{
public:
    using CommonInterface::CommonInterface;
    ~ObjExample() override = default;
    void initialize() override
    {
        m_camera.setLookAt({ 0, 0, 3 }, { 0, 0, 0 }, { 0, 1, 0 });
        auto material = Shader::getStandardPBR()->createMaterial({ { "S_TANGENTS", "1" } });
        material->setColor({ 1.0f, 1.0f, 1.0f, 1.0f });
        material->setSpecularity({ 1, 1, 1, 20.0f });
        m_materials.push_back(material);
        m_mesh = Mesh::create().withCube().build();
        m_worldLights = MAKE_UNIQUE(m_worldLights);
        m_worldLights->addLight(Light::create().build());
        m_worldLights->addLight(Light::create().build());
        m_worldLights->addLight(Light::create().build());
        m_worldLights->addLight(Light::create().build());
        setLight(1);
    }
    void render() override
    {
        auto renderPass = RenderPass::create()
                              .withCamera(m_camera)
                              .withWorldLights(m_worldLights.get())
                              .withClearColor(true, { 0, 0, 0, 1 })
                              .build();
        renderPass.draw(m_mesh, glm::eulerAngleY(glm::radians(30.0f * m_totalTime)) * glm::translate(m_offset), m_materials);
        lightGUI();
        m_inspector.update();
        m_inspector.gui();
    }

    void loadObjFile(const std::string& file)
    {
        std::filesystem::path path(file);
        m_materials.clear();
        m_mesh = ModelImporter::importObj(path, m_materials);
        for (const auto& m : m_materials)
        {
            LOG_INFO("{}", m->getName());
        }
        LOG_INFO("{} {}", m_materials.size(), m_mesh->getIndexSets());
        auto bounds = m_mesh->getBoundsMinMax();
        auto center = glm::mix(bounds[1], bounds[0], 0.5f);
        m_offset = -center;
        m_farPlane = glm::length(bounds[1] - bounds[0]);
        m_camera.setLookAt({ 0, 1, m_farPlane }, { 0, 0, 0 }, { 0, 1, 0 });
        m_camera.setPerspectiveProjection(60, 0.1, m_farPlane * 2);
    }

    void dropEvent(int count, const char** paths) override
    {
        const char* filename = paths[0];
        LOG_INFO("load file is:{}", filename);
        loadObjFile(filename);
    }

    void updateLight(Light* light, int i)
    {
        ImGui::PushID(i);
        ImGui::LabelText("", "Light index %i", i);
        const char* items[] = { "Point", "Directional", "None" };
        ImGui::Combo("Light type", (int*)(&light->type), items, 3);
        if (light->type == Light::Type::Directional)
        {
            ImGui::DragFloat3("Direction", &light->direction.x);
        }
        else if (light->type == Light::Type::Point)
        {
            ImGui::DragFloat3("Position", &light->position.x);
            ImGui::DragFloat("Range", &light->range);
        }
        if (light->type != Light::Type::Unused)
        {
            ImGui::ColorEdit3("Color", &light->color.x);
        }
        ImGui::PopID();
    }

    void lightGUI()
    {
        static bool lightOpen = false;
        ImGui::Begin("Lights", &lightOpen);
        if (ImGui::CollapsingHeader("Predefined configs"))
        {
            if (ImGui::Button("Camera light"))
            {
                setLight(0);
            }
            if (ImGui::Button("Twin lights"))
            {
                setLight(1);
            }
            if (ImGui::Button("Twin lights Left-Right"))
            {
                setLight(2);
            }
            if (ImGui::Button("Colorful"))
            {
                setLight(3);
            }
        }

        auto ambientLight = m_worldLights->getAmbientLight();
        if (ImGui::ColorEdit3("Ambient light", &ambientLight.x))
        {
            m_worldLights->setAmbientLight(ambientLight);
        }
        for (int i = 0; i < 4; i++)
        {
            Light* light = m_worldLights->getLight(i);
            updateLight(light, i);
        }
        ImGui::End();
    }

    void setLight(int config)
    {
        if (config == 0)
        {
            m_worldLights->setAmbientLight({ 0.05f, 0.05f, 0.05f });
            m_worldLights->getLight(0)->type = Light::Type::Point;
            m_worldLights->getLight(0)->color = { 1, 1, 1 };
            m_worldLights->getLight(0)->position = { 0, 0, 4 };
            m_worldLights->getLight(0)->range = 100;
            m_worldLights->getLight(1)->type = Light::Type::Unused;
            m_worldLights->getLight(2)->type = Light::Type::Unused;
            m_worldLights->getLight(3)->type = Light::Type::Unused;
        }
        if (config == 1)
        {
            m_worldLights->setAmbientLight({ 0.05f, 0.05f, 0.05f });
            m_worldLights->getLight(0)->type = Light::Type::Directional;
            m_worldLights->getLight(0)->color = { 1, 1, 1 };
            m_worldLights->getLight(0)->direction = { 1, 1, .2 };
            m_worldLights->getLight(1)->type = Light::Type::Directional;
            m_worldLights->getLight(1)->color = { 0, 0, .3 };
            m_worldLights->getLight(1)->direction = { -1, -1, -.8 };
            m_worldLights->getLight(2)->type = Light::Type::Unused;
            m_worldLights->getLight(3)->type = Light::Type::Unused;
        }
        if (config == 2)
        {
            m_worldLights->setAmbientLight({ 0.05f, 0.05f, 0.05f });
            m_worldLights->getLight(0)->type = Light::Type::Directional;
            m_worldLights->getLight(0)->color = { 1, 0, 0 };
            m_worldLights->getLight(0)->direction = { 1, 0, 0 };
            m_worldLights->getLight(1)->type = Light::Type::Directional;
            m_worldLights->getLight(1)->color = { 0, 1, 0 };
            m_worldLights->getLight(1)->direction = { -1, 0, 0 };
            m_worldLights->getLight(2)->type = Light::Type::Unused;
            m_worldLights->getLight(3)->type = Light::Type::Unused;
        }
        if (config == 3)
        {
            m_worldLights->setAmbientLight({ 0.05f, 0.05f, 0.05f });
            m_worldLights->getLight(0)->type = Light::Type::Point;
            m_worldLights->getLight(0)->color = { 1, 0, 0 };
            m_worldLights->getLight(0)->position = { 0, 3, 0 };
            m_worldLights->getLight(0)->range = 20;
            m_worldLights->getLight(1)->type = Light::Type::Point;
            m_worldLights->getLight(1)->color = { 0, 1, 0 };
            m_worldLights->getLight(1)->position = { 3, 0, 0 };
            m_worldLights->getLight(1)->range = 20;
            m_worldLights->getLight(2)->type = Light::Type::Point;
            m_worldLights->getLight(2)->color = { 0, -3, 0 };
            m_worldLights->getLight(2)->position = { 0, -3, 0 };
            m_worldLights->getLight(2)->range = 20;
            m_worldLights->getLight(3)->type = Light::Type::Point;
            m_worldLights->getLight(3)->color = { 1, 1, 1 };
            m_worldLights->getLight(3)->position = { -3, 0, 0 };
            m_worldLights->getLight(3)->range = 20;
        }
    }

private:
    std::vector<std::shared_ptr<Material>> m_materials;
    glm::vec3 m_offset{ 0 };
    float m_farPlane{ 100.0f };
};

void objTest()
{
    GLFWRenderer renderer{};
    EngineTestSimple test(renderer);
    auto sceneNodeEffect = std::make_shared<ObjExample>(&renderer);
    auto effect = std::make_shared<EffectManager>();
    effect->insertEffect(sceneNodeEffect);
    test.setEffect(effect);
    test.setTitle("ObjExample");
    test.run();
}