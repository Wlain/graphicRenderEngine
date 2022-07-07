//
// Created by william on 2022/6/12.
//

#include "engineTestSimple.h"

#include <map>
class CustomMeshLayoutTest : public CommonInterface
{
public:
    using CommonInterface::CommonInterface;
    ~CustomMeshLayoutTest() override = default;

    void initialize() override
    {
        std::vector<glm::vec3> positions({ { 0, 0.5, 0 },
                                           { -0.5, 0, 0 },
                                           { 0.5, 0, 0 } });
        std::vector<glm::vec4> colors({
            { 1, 0, 0, 1 },
            { 0, 1, 0, 1 },
            { 0, 0, 1, 1 },
        });
        m_mesh = Mesh::create()
                     .withPositions(positions)
                     .withAttribute("color", colors)
                     .build();

        m_material = Shader::getUnlit()->createMaterial({ { "S_VERTEX_COLOR", "1" } });
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

void customMeshLayoutTest()
{
    GLFWRenderer renderer{};
    EngineTestSimple test(renderer);
    auto sceneNodeEffect = std::make_shared<CustomMeshLayoutTest>(&renderer);
    auto effect = std::make_shared<EffectManager>();
    effect->insertEffect(sceneNodeEffect);
    test.setEffect(effect);
    test.setTitle("CustomMeshLayoutTest");
    test.run();
}