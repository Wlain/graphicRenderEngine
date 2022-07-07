//
// Created by william on 2022/5/23.
//

#include "commonMacro.h"
#include "core/texture.h"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include "engineTestSimple.h"

#include <glm/gtx/euler_angles.hpp>
#include <memory>

class CubeTexExample : public CommonInterface
{
public:
    using CommonInterface::CommonInterface;
    ~CubeTexExample() override = default;
    void initialize() override
    {
        m_camera.setLookAt({ 0, 0, 3 }, { 0, 0, 0 }, { 0, 1, 0 });
        m_camera.setPerspectiveProjection(60, 0.1, 100);
        auto shader = Shader::getUnlit();
        m_material = shader->createMaterial();
        m_material->setTexture(Texture::create().withFile("resources/test.jpg").build());
        m_mesh = Mesh::create()
                     .withCube()
                     .build();
    }

    void render() override
    {
        auto renderPass = RenderPass::create().withCamera(m_camera).withClearColor(true, { 1, 0, 0, 1 }).build();
        renderPass.draw(m_mesh, glm::eulerAngleY(glm::radians(30 * m_totalTime)), m_material);
    }
};

void cubeTexTest()
{
    GLFWRenderer renderer{};
    EngineTestSimple test(renderer);
    auto sceneNodeEffect = std::make_shared<CubeTexExample>(&renderer);
    auto effect = std::make_shared<EffectManager>();
    effect->insertEffect(sceneNodeEffect);
    test.setEffect(effect);
    test.setTitle("CubeTexExample");
    test.run();
}
