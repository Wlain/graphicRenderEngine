//
// Created by william on 2022/6/1.
//
#include "basicProject.h"
class DrawLineExample : public BasicProject
{
public:
    void render() override
    {
        RenderPass rp = RenderPass::create().withClearColor(true, { 0.0f, 0.0f, 0.0f, 1.0f }).build();
        rp.drawLines({ { -.5, -.5, 0 }, { .5, .5, 0 }, { -.5, .5, 0 }, { .5, -.5, 0 } });
        m_profiler.update();
        m_profiler.gui();
    }
    void setTitle() override
    {
        m_title = "DrawLineExample";
    }
};

void drawLineTest()
{
    DrawLineExample test;
    test.run();
}