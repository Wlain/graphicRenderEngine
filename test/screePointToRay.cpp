//
// Created by william on 2022/6/12.
//

#include "basicProject.h"
#include "guiCommonDefine.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/random.hpp>
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

        m_matPlane = Shader::getStandard()->createMaterial();
        m_matPlane->setColor({ 1, 1, 1, 1 });
        m_matPlane->setSpecularity(0);

        m_materia2 = Shader::getStandard()->createMaterial();
        m_materia2->setColor({ 1, 0, 0, 1 });
        m_materia2->setSpecularity(0);
    }

    void updateMaterial(std::shared_ptr<Material>& mat)
    {
        mat->setColor(Color(glm::linearRand(0.0f, 1.0f), glm::linearRand(0.0f, 1.0f), glm::linearRand(0.0f, 1.0f), 1));
    }

    float rayToSphere(std::array<glm::vec3, 2> ray, glm::vec3 sphereCenter)
    {
        float d = dot(sphereCenter - ray[0], ray[1]);
        if (d < 0)
        {
            d = 0;
        }
        glm::vec3 closestPoint = d * ray[1] + ray[0];
        return glm::distance(closestPoint, sphereCenter);
    }

    void touchEvent(double xPos, double yPos) override
    {
        auto framebufferSize = m_renderer.getFrameBufferSize();
        auto windowsSize = m_renderer.getWindowSize();
        auto ratio = framebufferSize.x / windowsSize.x;
        // 坐标映射
        auto mouseX = std::clamp((int)(xPos * ratio), 0, windowsSize.x * (int)ratio);
        auto mouseY = std::clamp((int)(windowsSize.y - yPos) * (int)ratio, 0, windowsSize.y * (int)ratio);

        glm::vec2 pos = { mouseX, mouseY };
        auto res = m_camera.screenPointToRay(pos);
        m_raycastOrigin = res[0];
        m_raycastDirection = res[1];
        m_points = { { m_raycastOrigin, m_raycastOrigin + m_raycastDirection } };
        float dist1 = rayToSphere(res, m_p1);
        if (dist1 < 1)
        {
            updateMaterial(m_material);
        }
        float dist2 = rayToSphere(res, m_p2);
        if (dist2 < 1)
        {
            updateMaterial(m_materia2);
        }
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

        checkGlError();
        rp.draw(m_mesh, m_pos2, m_materia2);

        checkGlError();

        ImGui::LabelText("Rightclick to shoot ray", "");
        rp.draw(m_planeMesh, glm::translate(glm::vec3{ 0, -1.0f, 0 }) * glm::scale(glm::vec3{ 1, .01f, 1 }), m_matPlane);
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
    std::shared_ptr<Material> m_matPlane;
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