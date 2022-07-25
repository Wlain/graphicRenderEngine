//
// Created by william on 2022/5/29.
//

#include "commonHandle.h"
#include "core/glfwRenderer.h"
#include "core/material.h"

using namespace ceres;

class EngineTestSimple
{
public:
    EngineTestSimple(GLFWRenderer& renderer) :
        m_renderer(renderer)
    {
        m_renderer.init();
        m_handle.reset(CommonHandle::create());
    };

    void setEffect(const std::shared_ptr<EffectManager>& effect)
    {
        m_handle->setEffect(effect);
    }

    virtual ~EngineTestSimple() = default;
    void run()
    {
        m_handle->initialize();
        m_handle->setTitle();
        m_renderer.m_frameUpdate = [&](float deltaTime) {
            m_handle->update(deltaTime);
        };
        m_renderer.m_frameResize = [&](int width, int height) {
            m_handle->resize(width, height);
        };
        m_renderer.m_cursorPosEvent = [&](int xPos, int yPos) {
            m_handle->cursorPosEvent(xPos, yPos);
        };
        m_renderer.m_mouseButtonEvent = [&](int button, int action, int mods) {
            m_handle->mouseButtonEvent(button, action, mods);
        };
        m_renderer.m_dropEvent = [&](int count, const char** paths) {
            m_handle->dropEvent(count, paths);
        };
        m_renderer.m_frameRender = [&]() {
            m_handle->render();
        };
        m_renderer.startEventLoop();
    }

protected:
    GLFWRenderer& m_renderer;
    std::shared_ptr<CommonHandle> m_handle;
};