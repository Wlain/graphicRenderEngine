//
// Created by william on 2022/6/12.
//

#include "basicProject.h"
#include "guiCommonDefine.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

class ScreePointToRayExample : public BasicProject
{
public:
    using BasicProject::BasicProject;
    ~ScreePointToRayExample() override = default;
    void initialize() override
    {
        m_camera.setLookAt(m_eye, m_at, { 0, 1, 0 });
        m_camera.setPerspectiveProjection(m_fov, m_near, m_far);
        m_mesh = Mesh::create()
                     .withSphere()
                     .build();

        m_planeMesh = Mesh::create()
                          .withCube(10)
                          .build();
        m_worldLights = MAKE_UNIQUE(m_worldLights);
        m_worldLights->addLight(Light::create()
                                    .withDirectionalLight(glm::normalize(glm::vec3(1, 1, 1)))
                                    .build());

        // Add fake shadows
        m_worldLights->addLight(Light::create()
                                    .withPointLight(m_p1 - glm::vec3(0, 0.8, 0))
                                    .withColor({ -3.0f, -3.0f, -3.0f })
                                    .withRange(4)
                                    .build());
        m_worldLights->addLight(Light::create()
                                    .withPointLight(m_p2 - glm::vec3(0, 0.8, 0))
                                    .withColor({ -3.0f, -3.0f, -3.0f })
                                    .withRange(4)
                                    .build());

        m_material = Shader::getStandard()->createMaterial();
        m_material->setColor({ 1, 1, 1, 1 });
        m_material->setSpecularity(0);

        m_materia2 = Shader::getStandard()->createMaterial();
        m_materia2->setColor({ 1, 0, 0, 1 });
        m_materia2->setSpecularity(0);
    }

    void touchEvent(double xPos, double yPos) override
    {
        glm::vec2 pos = { xPos, yPos };
        pos.y = Renderer::s_instance->getWindowSize().y - pos.y; // flip y axis
        auto res = m_camera.screenPointToRay(pos);
        m_raycastOrigin = res[0];
        m_raycastDirection = res[1];
        m_points = { { m_raycastOrigin, m_raycastOrigin + m_raycastDirection } };
    }
    void cameraGUI()
    {
        ImGui::Checkbox("Perspective projection", &m_perspective);
        if (m_perspective)
        {
            ImGui::DragFloat("FOV", &m_fov, 1, 1, 179);
        }
        else
        {
            ImGui::DragFloat("OrthoSize", &m_orthoSize, 0.1, 0.1, 10);
        }
        ImGui::DragFloat("Near", &m_near, 0.1, -10, 10);
        ImGui::DragFloat("Far", &m_far, 0.1, 0.1, 100);
        if (m_perspective)
        {
            m_camera.setPerspectiveProjection(m_fov, m_near, m_far);
        }
        else
        {
            m_camera.setOrthographicProjection(m_orthoSize, m_near, m_far);
        }
        ImGui::DragFloat3("eye", &m_eye.x, 0.1, -10, 10);
        ImGui::DragFloat3("at", &m_at.x, 0.1, -10, 10);
        m_camera.setLookAt(m_eye, m_at, { 0, 1, 0 });
    }

    void render() override
    {
        auto rp = RenderPass::create()
                      .withCamera(m_camera)
                      .withWorldLights(m_worldLights.get())
                      .withClearColor(true, { 1, 0, 0, 1 })
                      .build();

        rp.draw(m_mesh, m_pos1, m_material);

        checkGLError();
        rp.draw(m_mesh, m_pos2, m_materia2);

        checkGLError();

        ImGui::LabelText("Rightclick to shoot ray", "");
        rp.draw(m_planeMesh, glm::translate(glm::vec3{ 0, -1.0f, 0 }) * glm::scale(glm::vec3{ 1, .01f, 1 }), m_material);

        ImGui::LabelText("raycastOrigin", "%.1f,%.1f,%.1f", m_raycastOrigin.x, m_raycastOrigin.y, m_raycastOrigin.z);
        ImGui::LabelText("raycastDirection", "%.1f,%.1f,%.1f", m_raycastDirection.x, m_raycastDirection.y, m_raycastDirection.z);

        rp.drawLines(m_points);

        cameraGUI();
    }
    void setTitle() override
    {
        m_title = "ScreePointToRayExample";
    }

private:
    std::shared_ptr<Mesh> m_planeMesh;
    glm::vec3 m_eye = { 0, 0, 3 };
    glm::vec3 m_at = { 0, 0, 0 };
    glm::vec3 m_p1 = { -1, 0, 0 };
    glm::vec3 m_p2 = { 1, 0, 0 };
    glm::vec3 m_raycastOrigin{ 0 };
    glm::vec3 m_raycastDirection{ 0 };
    std::vector<glm::vec3> m_points{ { m_raycastOrigin, m_raycastOrigin + m_raycastDirection } };
    glm::mat4 m_pos1 = glm::translate(glm::mat4(1), m_p1);
    glm::mat4 m_pos2 = glm::translate(glm::mat4(1), m_p2);
    std::shared_ptr<Material> m_materia2;
    // camera properties
    bool m_perspective = true;
    float m_fov = 60;
    float m_near = 0.1;
    float m_far = 100;
    float m_orthoSize = 2;
};

void screePointToRayTest()
{
    ScreePointToRayExample test;
    test.run();
}