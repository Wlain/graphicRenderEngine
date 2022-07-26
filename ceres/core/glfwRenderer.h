//
// Created by william on 2022/5/28.
//

#ifndef GRAPHICRENDERENGINE_GLFWRENDERER_H
#define GRAPHICRENDERENGINE_GLFWRENDERER_H
#include "glCommonDefine.h"

#include <functional>
#include <glm/glm.hpp>
#include <string>
#include <string_view>
// glfw抽象
namespace ceres
{
class Renderer;
class GLFWRenderer
{
public:
    GLFWRenderer();
    virtual ~GLFWRenderer();
    GLFWRenderer(const GLFWRenderer&) = delete;
    void init(bool vsync = true);
    void setWindowTitle(std::string_view title);
    void setWindowSize(int width, int height);
    void startEventLoop();
    [[maybe_unused]] void stopEventLoop();
    GLFWwindow* getGlfwWindow();
    glm::ivec2 getFrameBufferSize();
    glm::ivec2 getWindowSize();

private:
    void frame(float deltaTimeSec);

public:
    // 方法回调
    std::function<void(int width, int height)> m_frameResize;
    std::function<void(double xPos, double yPos)> m_cursorPosEvent;
    std::function<void(int button, int action, int mods)> m_mouseButtonEvent;
    std::function<void(int count, const char** paths)> m_dropEvent;
    std::function<void(float deltaTimeSec)> m_frameUpdate;
    std::function<void()> m_frameRender;

private:
    Renderer* m_renderer{ nullptr };
    GLFWwindow* m_window{ nullptr };
    std::string m_windowTitle;
    float m_timePerFrame{ 1.0f / 60.0f };
    int m_windowWidth{ 800 };
    int m_windowHeight{ 600 };
    bool m_running{ false };
};
} // namespace ceres

#endif // GRAPHICRENDERENGINE_GLFWRENDERER_H
