//
// Created by william on 2022/5/24.
//

#include "engineTestSimple.h"
#include "commonMacro.h"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

class QuadExample : public CommonInterface
{
public:
    using CommonInterface::CommonInterface;
    ~QuadExample() override = default;

    void initialize() override
    {
        m_camera.setLookAt({ 0.0f, 0.0f, 3.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
        m_camera.setPerspectiveProjection(60, 0.1, 100);
        auto shader = Shader::create()
                          .withSourceFile("shaders/unlit_vert.glsl", Shader::ShaderType::Vertex)
                          .withSourceFile("shaders/unlit_frag.glsl", Shader::ShaderType::Fragment)
                          .withName("Unlit")
                          .withCullFace(Shader::CullFace::None)
                          .build();
        m_material = shader->createMaterial();
        m_material->setTexture(Texture::create().withFile("resources/test.jpg").build());
        m_mesh = Mesh::create().withQuad().build();
    }

    void render() override
    {
        /// 渲染
        auto renderPass = RenderPass::create().withCamera(m_camera).build();
        renderPass.draw(m_mesh, glm::eulerAngleY(glm::radians(30.0f * m_totalTime)), m_material);
        m_inspector.update();
        m_inspector.gui();
    }

    void setTitle() override
    {
        m_title = "QuadExample";
    }
};

void quadTest()
{
    GLFWRenderer renderer{};
    EngineTestSimple test(renderer);
    auto sceneNodeEffect = std::make_shared<QuadExample>(&renderer);
    auto effect = std::make_shared<EffectManager>();
    effect->insertEffect(sceneNodeEffect);
    test.setEffect(effect);
    test.run();
}