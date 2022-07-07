//
// Created by william on 2022/6/12.
//
#include "engineTestSimple.h"

class CustomMeshLayoutDefaultValuesExample : public CommonInterface
{
public:
    using CommonInterface::CommonInterface;
    ~CustomMeshLayoutDefaultValuesExample() override = default;
    void initialize() override
    {
        std::vector<glm::vec2> positions({ { 0, 0.5 },
                                           { -0.5, 0 },
                                           { 0.5, 0 } });
        std::vector<glm::vec4> colors({
            { 1, 0, 0, 1 },
            { 0, 1, 0, 1 },
            { 0, 0, 1, 1 },
        });
        m_mesh = Mesh::create()
                     .withAttribute("posxyzw", positions)
                     .withAttribute("color", colors)
                     .build();
        std::string vertexShaderSource = R"(#version 330
            in vec4 posxyzw;    // should automatically cast vec2 -> vec4 by appending (z = 0.0, w = 1.0)
            in vec4 color;
            out vec4 vColor;

            uniform mat4 g_model;
            uniform mat4 view;
            uniform mat4 projection;

            void main() {
                gl_Position = projection * view * g_model * posxyzw;
                vColor = color;
            }
        )";
        std::string fragmentShaderSource = R"(#version 330
            out vec4 fragColor;
            in vec4 vColor;

            void main()
            {
                fragColor = vColor;
            }
        )";
        m_material = Shader::create()
                         .withSourceString(vertexShaderSource, Shader::ShaderType::Vertex)
                         .withSourceString(fragmentShaderSource, Shader::ShaderType::Fragment)
                         .build()
                         ->createMaterial();
        m_material->set("view", m_camera.getViewTransform());
        m_material->set("projection", m_camera.getProjectionTransform({ 800, 600 }));
    }
    void render() override
    {
        auto rp = RenderPass::create()
                      .withCamera(m_camera)
                      .withClearColor(true, { 1, 0, 0, 1 })
                      .build();

        rp.draw(m_mesh, glm::mat4(1), m_material);
    }
};

void customMeshLayoutDefaultValuesTest()
{
    GLFWRenderer renderer{};
    EngineTestSimple test(renderer);
    auto sceneNodeEffect = std::make_shared<CustomMeshLayoutDefaultValuesExample>(&renderer);
    auto effect = std::make_shared<EffectManager>();
    effect->insertEffect(sceneNodeEffect);
    test.setEffect(effect);
    test.setTitle("CustomMeshLayoutDefaultValuesExample");
    test.run();
}