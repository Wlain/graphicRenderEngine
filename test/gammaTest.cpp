//
// Created by cwb on 2022/6/20.
//
#include "engineTestSimple.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

class GammaExample : public CommonInterface
{
public:
    using CommonInterface::CommonInterface;
    ~GammaExample() override = default;
    void initialize() override
    {
        m_mesh = Mesh::create().withQuad(0.5f).build();
        m_camera.setWindowCoordinates();
        m_material = Shader::getUnlit()->createMaterial();
        m_tex1 = Texture::create()
                     .withFile("resources/sprites/gamma-test.png")
                     .withFilterSampling(false)
                     .build();
        m_tex2 = Texture::create()
                     .withFile("resources/sprites/gamma-small.png")
                     .withFilterSampling(false)
                     .build();

        m_material->setTexture(m_tex2);
    }
    void render() override
    {
        auto rp = RenderPass::create()
                      .withCamera(m_camera)
                      .withClearColor(true, { 1, 0, 0, 1 })
                      .build();

        rp.draw(m_mesh, glm::translate(glm::vec3{ (int)(m_tex1->width() * 0.5), (int)(m_tex1->height() * 0.5), 0 }) * glm::scale(glm::vec3(m_tex1->width(), m_tex1->height(), 1)), m_material);
    }
    void setTitle() override
    {
        m_title = "GammaExample";
    }

private:
    std::shared_ptr<Texture> m_tex1;
    std::shared_ptr<Texture> m_tex2;
};

void gammaTest()
{
    GLFWRenderer renderer{};
    EngineTestSimple test(renderer);
    auto sceneNodeEffect = std::make_shared<GammaExample>(&renderer);
    auto effect = std::make_shared<EffectManager>();
    effect->insertEffect(sceneNodeEffect);
    test.setEffect(effect);
    test.run();
}