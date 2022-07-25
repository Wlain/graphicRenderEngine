//
// Created by william on 2022/6/12.
//

#include "engineTestSimple.h"
#include "guiCommonDefine.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/random.hpp>
#include <glm/gtx/transform.hpp>

/*
 * 前置知识：
 * 射线公式：ray = p + tv(v:方向，t:长度，p:世界点位置
 * 世界坐标系转化成屏幕坐标的过程如下：
 * 1.通过worldmatrix把模型的局部点转为世界点
 * 2.通过viewmatrix矩阵把点从世界位置转为视口下
 * 3.通过projematrix投影矩阵把视口点转为近剪裁面上的点
 * 4.通过投射除法映射到NDC空间[-1.1]
 * 5.通过半兰伯特手法把[-1.1]映射到[0,1] （丢弃z值）
 * 6.再和屏幕宽高相乘获得屏幕的位置
 */

class ScreePointToRayExample : public CommonInterface
{
public:
    using CommonInterface::CommonInterface;
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
        m_material = Shader::getUnlit()->createMaterial();
        m_material->setColor({ 1, 0, 1, 1 });

        m_matPlane = Shader::getUnlit()->createMaterial();
        m_matPlane->setColor({ 0.6, 0.6, 0.6, 1 });

        m_materia2 = Shader::getUnlit()->createMaterial();
        m_materia2->setColor({ 0, 1, 0, 1 });
    }

    void updateMaterial(const std::shared_ptr<Material>& mat)
    {
        mat->setColor(Color(glm::linearRand(0.0f, 1.0f), glm::linearRand(0.0f, 1.0f), glm::linearRand(0.0f, 1.0f), 1));
    }

    float rayToSphere(glm::vec3 sphereCenter)
    {
        // 点乘：用来计算在direction方向的投影
        m_ray.t = glm::dot(sphereCenter - m_ray.position, m_ray.direction);
        m_ray.t = std::max(m_ray.t, 0.0f);
        auto target = m_ray.target();
        return glm::distance(target, sphereCenter);
    }

    void cursorPosEvent(double xPos, double yPos) override
    {
        auto framebufferSize = m_renderer->getFrameBufferSize();
        auto windowsSize = m_renderer->getWindowSize();
        auto ratio = framebufferSize.x / windowsSize.x;
        // 坐标映射
        auto mouseX = std::clamp((int)(xPos * ratio), 0, windowsSize.x * (int)ratio);
        auto mouseY = std::clamp((int)(windowsSize.y - yPos) * (int)ratio, 0, windowsSize.y * (int)ratio);
        // 第一步：拿到屏幕坐标的点
        glm::vec2 pos = { mouseX, mouseY };
        // 第二步：获取世界坐标对应的坐标
        m_ray = m_camera.screenPointToRay(pos);
        float dist1 = rayToSphere(m_p1);
        if (dist1 < 1)
        {
            updateMaterial(m_material);
        }
        float dist2 = rayToSphere(m_p2);
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
                      .withClearColor(true, { 1, 0, 0, 1 })
                      .build();
        rp.draw(m_mesh, m_pos1, m_material);
        rp.draw(m_mesh, m_pos2, m_materia2);
        ImGui::LabelText("Rightclick to shoot ray", "");
        rp.draw(m_planeMesh, glm::translate(glm::vec3{ 0, -1.0f, 0 }) * glm::scale(glm::vec3{ 1, .01f, 1 }), m_matPlane);
        ImGui::LabelText("raycastOrigin", "%.1f,%.1f,%.1f", m_ray.position.x, m_ray.position.y, m_ray.position.z);
        ImGui::LabelText("raycastDirection", "%.1f,%.1f,%.1f", m_ray.direction.x, m_ray.direction.y, m_ray.direction.z);
        rp.drawLines({ m_ray.position, m_ray.position + m_ray.direction });
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
    glm::vec3 m_p1 = { -1, 0, 0 }; // 球中心
    glm::vec3 m_p2 = { 1, 0, 0 };  // 球中心
    Ray m_ray;
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
    GLFWRenderer renderer{};
    EngineTestSimple test(renderer);
    auto sceneNodeEffect = std::make_shared<ScreePointToRayExample>(&renderer);
    auto effect = std::make_shared<EffectManager>();
    effect->insertEffect(sceneNodeEffect);
    test.setEffect(effect);
    test.run();
}