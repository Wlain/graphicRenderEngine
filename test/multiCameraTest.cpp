//
// Created by william on 2022/6/12.
//
#include "basicProject.h"
#include "guiCommonDefine.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
using namespace re;
class MultiCameraExample : public BasicProject
{
public:
    ~MultiCameraExample() override = default;
    void initialize() override
    {
        m_camera.setLookAt(m_eye, m_at, m_up);
        m_camera.setPerspectiveProjection(60, 0.1, 100);

        m_camera2.setLookAt({ 0, 0, 3 }, { 0, 0, 0 }, { 0, 1, 0 });
        m_camera2.setPerspectiveProjection(60, 0.1, 100);
        m_camera2.setViewport({ 0, 0.8 }, { 0.2, 0.2 });

        m_material = Shader::getStandardBlinnPhong()->createMaterial();
        m_material->setColor({ 1.0f, 1.0f, 1.0f, 1.0f });
        m_material->setSpecularity({0, 0, 0, 20.0f});

        m_mesh = Mesh::create().withCube().build();
        m_worldLights = MAKE_UNIQUE(m_worldLights);
        m_worldLights->setAmbientLight({ 0.5, 0.5, 0.5 });
        m_worldLights->addLight(Light::create().withPointLight({ 0, 3, 0 }).withColor({ 1, 0, 0 }).withRange(20).build());
        m_worldLights->addLight(Light::create().withPointLight({ 3, 0, 0 }).withColor({ 0, 1, 0 }).withRange(20).build());
        m_worldLights->addLight(Light::create().withPointLight({ 0, -3, 0 }).withColor({ 0, 0, 1 }).withRange(20).build());
        m_worldLights->addLight(Light::create().withPointLight({ -3, 0, 0 }).withColor({ 1, 1, 1 }).withRange(20).build());
    }

    void render() override
    {
        auto renderPass = RenderPass::create()
                              .withCamera(m_camera)
                              .withWorldLights(m_worldLights.get())
                              .withClearColor(true, { 1, 0, 0, 1 })
                              .withGUI(false)
                              .build();

        renderPass.draw(m_mesh, glm::eulerAngleY(glm::radians(30.0f * m_totalTime)), m_material);

        renderPass = RenderPass::create()
                         .withCamera(m_camera2)
                         .withWorldLights(m_worldLights.get())
                         .withClearColor(true, { 1, 1, 0, 1 })
                         .withGUI(true)
                         .build();

        renderPass.draw(m_mesh, glm::eulerAngleY(glm::radians(30.0f * m_totalTime)), m_material);

        static bool lookAt = true;
        ImGui::Checkbox("LookAt", &lookAt);
        if (lookAt)
        {
            ImGui::DragFloat3("eye", &m_eye.x);
            ImGui::DragFloat3("at", &m_at.x);
            ImGui::DragFloat3("up", &m_up.x);
            m_camera.setLookAt(m_eye, m_at, m_up);
        }
        else
        {
            ImGui::DragFloat3("position", &m_position.x);
            ImGui::DragFloat3("rotation", &m_rotation.x);
            m_camera.setPositionAndRotation(m_position, m_rotation);
        }
        auto pos = m_camera.getPosition();
        auto rot = m_camera.getRotationEuler();
        ImGui::LabelText("GetPos", "%f %f %f", pos.x, pos.y, pos.z);
        ImGui::LabelText("GetRot", "%f %f %f", rot.x, rot.y, rot.z);
    }
    void setTitle() override
    {
        m_title = "MultiCameraExample";
    }

private:
    glm::vec3 m_eye{ 0, 0, 3 };
    glm::vec3 m_at{ 0, 0, 0 };
    glm::vec3 m_up{ 0, 1, 0 };
    glm::vec3 m_position{ 0, 0, 3 };
    glm::vec3 m_rotation{ 0, 0, 0 };
    Camera m_camera2;
};

void multiCameraTest()
{
    MultiCameraExample test;
    test.run();
}