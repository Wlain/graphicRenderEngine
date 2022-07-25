//
// Created by william on 2022/6/1.
//
#include "engineTestSimple.h"
class DrawLineExample : public CommonInterface
{
public:
    using CommonInterface::CommonInterface;
    void render() override
    {
        RenderPass rp = RenderPass::create().withClearColor(true, { 0.0f, 0.0f, 0.0f, 1.0f }).build();
        rp.drawLines({ { -.5, -.5, 0 }, { .5, .5, 0 }, { -.5, .5, 0 }, { .5, -.5, 0 } });
        m_inspector.update();
        m_inspector.gui();
    }
    void setTitle() override
    {
        m_title = "DrawLineExample";
    }
};

void drawLineTest()
{
    GLFWRenderer renderer{};
    EngineTestSimple test(renderer);
    auto sceneNodeEffect = std::make_shared<DrawLineExample>(&renderer);
    auto effect = std::make_shared<EffectManager>();
    effect->insertEffect(sceneNodeEffect);
    test.setEffect(effect);
    test.run();
}