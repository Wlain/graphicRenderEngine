//
// Created by william on 2022/5/30.
//
#include "basicProject.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
class PickColorExample : public BasicProject
{
public:
    using BasicProject::BasicProject;
    ~PickColorExample() override = default;

    void initialize() override
    {
        m_camera = MAKE_UNIQUE(m_camera);
        m_camera->setLookAt({ 0, 0, 6 }, { 0, 0, 0 }, { 0, 1, 0 });
        m_camera->setPerspectiveProjection(60, 0.1, 100);
        for (int i = 0; i < 4; i++)
        {
            m_material[i] = Shader::getUnlit()->createMaterial();
            glm::vec4 color(1, 1, 1, 1);
            color[i] = 0;
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
    }
    void render() override
    {
        auto renderPass = RenderPass::create()
                              .withCamera(*m_camera)
                              .withClearColor(true, { 0, 0, 0, 1 })
                              .build();
        int index = 0;
        for (int x = 0; x < 2; x++)
        {
            for (int y = 0; y < 2; y++)
            {
                if (index < 4)
                {
                    renderPass.draw(m_mesh[index], glm::translate(glm::vec3(-1.5 + x * 3, -1.5 + y * 3, 0)), m_material[index]);
                }
                index++;
            }
        }
        // read pixel values from defualt framebuffer (before gui is rendered)
        auto pixelValues = renderPass.readPixels(m_mouseX, m_mouseY);
        m_profiler.update();
        m_profiler.gui();
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
        auto framebufferSize = m_renderer.getFramebuffeSize();
        auto windowsSize = m_renderer.getWindowSize();
        auto ratio = framebufferSize.x / windowsSize.x;
        // 坐标映射
        m_mouseX = std::clamp((int)(xPos * ratio), 0, windowsSize.x * (int)ratio);
        m_mouseY = std::clamp((int)(windowsSize.y - yPos) * (int)ratio, 0, windowsSize.y * (int)ratio);
    }

private:
    std::shared_ptr<Material> m_material[4];
    std::shared_ptr<Mesh> m_mesh[4];
    int m_mouseX;
    int m_mouseY;
};

void pickColorTest()
{
    PickColorExample test;
    test.run();
}