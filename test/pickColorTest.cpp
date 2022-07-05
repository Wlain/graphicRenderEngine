//
// Created by william on 2022/5/30.
//
#include "basicProject.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>
class PickColorExample : public BasicProject
{
public:
    using BasicProject::BasicProject;
    ~PickColorExample() override = default;

    void initialize() override
    {
        m_camera.setLookAt({ 0, 0, 15 }, { 0, 0, 0 }, { 0, 1, 0 });
        m_camera.setPerspectiveProjection(60, 0.1, 100);
        for (int i = 0; i < s_primitiveCount; i++)
        {
            m_material[i] = Shader::getUnlit()->createMaterial();
            Color color(1, 1, 1, 1);
            color[i % 4] = 0;
            m_material[i]->setColor(color);
        }
        m_mesh[0] = Mesh::create()
                        .withQuad()
                        .build();
        m_mesh[1] = Mesh::create()
                        .withSphere()
                        .build();
        m_mesh[2] = Mesh::create()
                        .withCube()
                        .build();
        m_mesh[3] = Mesh::create()
                        .withTorus()
                        .build();
        m_mesh[4] = Mesh::create()
                        .withWireCube()
                        .build();
    }
    void render() override
    {
        auto renderPass = RenderPass::create()
                              .withCamera(m_camera)
                              .withClearColor(true, { 0, 0, 0, 1 })
                              .build();
        int index = 0;
        for (int x = 0; x < s_primitiveCount; x++)
        {
            glm::mat4 modelTransform = glm::translate(glm::vec3(-6 + x * 3, 0, 0)) * glm::eulerAngleY(glm::radians((float)m_totalTime * 30));
            renderPass.draw(m_mesh[index], modelTransform, m_material[index]);
            index++;
        }
        renderPass.finish();
        // read pixel values from defualt framebuffer (before gui is rendered)
        auto pixelValue = renderPass.readPixels(m_mouseX, m_mouseY);
    }
    void update(float deltaTime) override
    {
        BasicProject::update(deltaTime);
    }
    void setTitle() override
    {
        m_title = "PickColorExample";
    }

    void resize(int width, int height) override
    {
        BasicProject::resize(width, height);
    }
    void touchEvent(double xPos, double yPos) override
    {
        auto framebufferSize = m_renderer.getFrameBufferSize();
        auto windowsSize = m_renderer.getWindowSize();
        auto ratio = framebufferSize.x / windowsSize.x;
        // 坐标映射
        m_mouseX = std::clamp((int)(xPos * ratio), 0, windowsSize.x * (int)ratio);
        m_mouseY = std::clamp((int)(windowsSize.y - yPos) * (int)ratio, 0, windowsSize.y * (int)ratio);
    }

private:
    inline static const uint32_t s_primitiveCount = 5;

private:
    std::shared_ptr<Material> m_material[s_primitiveCount];
    std::shared_ptr<Mesh> m_mesh[s_primitiveCount];
    int m_mouseX;
    int m_mouseY;
};

void pickColorTest()
{
    PickColorExample test;
    test.run();
}