//
// Created by cwb on 2022/7/25.
//

#include "commonHandle.h"

#define FUNC_TO_ALL_EFFECTS(func, ...)       \
    do                                       \
    {                                        \
        for (auto& e : m_effects->effects()) \
        {                                    \
            e->func(__VA_ARGS__);            \
        }                                    \
    } while (0)

CommonHandle* CommonHandle::create()
{
    return new CommonHandle();
}

CommonHandle::CommonHandle() = default;

CommonHandle::~CommonHandle() = default;

void CommonHandle::initialize()
{
    FUNC_TO_ALL_EFFECTS(initialize);
}

void CommonHandle::resize(int width, int height)
{
    FUNC_TO_ALL_EFFECTS(resize, width, height);
}

void CommonHandle::update(float deltaTime)
{
    FUNC_TO_ALL_EFFECTS(update, deltaTime);
}

void CommonHandle::render()
{
    FUNC_TO_ALL_EFFECTS(render);
}

void CommonHandle::finalize()
{
    FUNC_TO_ALL_EFFECTS(finalize);
}

void CommonHandle::setTitle()
{
    FUNC_TO_ALL_EFFECTS(setTitle);
}

void CommonHandle::cursorPosEvent(double xPos, double yPos)
{
    FUNC_TO_ALL_EFFECTS(cursorPosEvent, xPos, yPos);
}

void CommonHandle::mouseButtonEvent(int button, int action, int mods)
{
    FUNC_TO_ALL_EFFECTS(mouseButtonEvent, button, action, mods);
}

void CommonHandle::dropEvent(int count, const char** paths)
{
    FUNC_TO_ALL_EFFECTS(dropEvent, count, paths);
}

void CommonHandle::setEffect(const std::shared_ptr<EffectManager>& effect)
{
    m_effects = effect;
}

#undef FUNC_TO_ALL_EFFECTS