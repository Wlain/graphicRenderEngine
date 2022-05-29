// Copyright (c) 2022. Lorem ipsum dolor sit amet, consectetur adipiscing elit.
// Morbi non lorem porttitor neque feugiat blandit. Ut vitae ipsum eget quam lacinia accumsan.
// Etiam sed turpis ac ipsum condimentum fringilla. Maecenas magna.
// Proin dapibus sapien vel ante. Aliquam erat volutpat. Pellentesque sagittis ligula eget metus.
// Vestibulum commodo. Ut rhoncus gravida arcu.

//
// Created by william on 2022/5/28.
//

#ifndef SIMPLERENDERENGINE_GLFWRENDERER_H
#define SIMPLERENDERENGINE_GLFWRENDERER_H

#include <GLFW/glfw3.h>
#include <functional>
#include <glm/glm.hpp>
#include <string>
#include <string_view>
// glfw抽象
namespace re
{
class Renderer;
class GLFWRenderer
{
public:
    GLFWRenderer();
    virtual ~GLFWRenderer();
    // Create the window and the graphics context (instantiates the re::Renderer).
    void init();
    void setWindowTitle(std::string_view title);

    void setWindowSize(int width, int height);
    // Start the event loop. Note that this member function in usually blocking (until the `stopEventLoop()` has been called).
    void startEventLoop();
    // The render loop will stop running when the frame is complete.
    void stopEventLoop();
    GLFWwindow* getGlfwWindow();
    glm::ivec2 getWindowSize();

private:
    void frame(float deltaTimeSec);
    GLFWRenderer(const GLFWRenderer&) = delete;

public:
    // event handlers (assigned empty default handlers)
    std::function<void(float deltaTimeSec)> m_frameUpdate;
    // Subscript be render events. The `Renderer::swapFrame()` is automatically invoked after the callback.
    std::function<void(Renderer* renderer)> m_frameRender;

private:
    Renderer* m_renderer{ nullptr };
    GLFWwindow* m_window{ nullptr };
    std::string m_windowTitle;
    float m_timePerFrame{ 1.0f / 60 };
    int m_windowWidth{ 800 };
    int m_windowHeight{ 600 };
    bool m_running{ false };
};
} // namespace re

#endif //SIMPLERENDERENGINE_GLFWRENDERER_H
