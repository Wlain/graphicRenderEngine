//
// Created by william on 2022/5/30.
//
#include "engineTestSimple.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>
class PickColorExample : public CommonInterface
{
public:
    using CommonInterface::CommonInterface;
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
        auto pixelValue = renderPass.readPixels(m_mousePos.x, m_mousePos.y);
    }
    void update(float deltaTime) override
    {
        CommonInterface::update(deltaTime);
    }
    void resize(int width, int height) override
    {
        CommonInterface::resize(width, height);
    }

private:
    inline static const uint32_t s_primitiveCount = 5;

private:
    std::shared_ptr<Material> m_material[s_primitiveCount];
    std::shared_ptr<Mesh> m_mesh[s_primitiveCount];
};

void pickColorTest()
{
    GLFWRenderer renderer{};
    EngineTestSimple test(renderer);
    auto sceneNodeEffect = std::make_shared<PickColorExample>(&renderer);
    auto effect = std::make_shared<EffectManager>();
    effect->insertEffect(sceneNodeEffect);
    test.setEffect(effect);
    test.setTitle("PickColorExample");
    test.run();
}