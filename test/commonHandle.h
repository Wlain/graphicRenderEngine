//
// Created by cwb on 2022/7/25.
//

#ifndef GRAPHICRENDERENGINE_COMMONHANDLE_H
#define GRAPHICRENDERENGINE_COMMONHANDLE_H
#include "effectManager.h"

#include <core/glfwRenderer.h>

using namespace ceres;

class CommonHandle
{
public:
    static CommonHandle* create();
    CommonHandle();
    virtual ~CommonHandle();
    virtual void initialize();
    virtual void resize(int width, int height);
    virtual void update(float deltaTime);
    virtual void render();
    virtual void finalize();
    virtual void cursorPosEvent(double xPos, double yPos);
    // button: left or right, action:pressed or released
    virtual void mouseButtonEvent(int button, int action, int mods);
    virtual void dropEvent(int count, const char** paths);
    void setEffect(const std::shared_ptr<EffectManager>& effect);

protected:
    std::shared_ptr<EffectManager> m_effects;
};

#endif // GRAPHICRENDERENGINE_COMMONHANDLE_H
