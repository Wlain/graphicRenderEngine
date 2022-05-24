//
// Created by william on 2022/5/23.
//

#include "commonMacro.h"
#include "core/mesh.h"
#include "core/renderer.h"
#include "core/shader.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
using namespace re;

static int s_canvasWidth = 640;
static int s_canvasHeight = 480;

void sphereTest()
{
    LOG_INFO("cubeTest");
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    // glfw window creation
    auto window = glfwCreateWindow(s_canvasWidth, s_canvasHeight, "hello world", nullptr, nullptr);
    if (window == nullptr)
    {
        glfwTerminate();
    }
    Renderer r{ window };
    r.getCamera()->lookAt({ 0.0f, 0.0f, 3.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
    r.getCamera()->perspectiveProjection(60.0f, s_canvasWidth, s_canvasHeight, 0.1f, 100.0f);
    Shader* shader = Shader::getStandard();
    Mesh* mesh = Mesh::createSphere();
    bool point = true;
    if (point)
    {
        r.setLight(0, { Light::Type::Point, { 0, 2, 1 }, { 0, 0, 0 }, { 1, 0, 0 }, 10, 5 });
        r.setLight(1, { Light::Type::Point, { 2, 0, 1 }, { 0, 0, 0 }, { 0, 1, 0 }, 10, 10 });
        r.setLight(2, { Light::Type::Point, { 0, -2, 1 }, { 0, 0, 0 }, { 0, 0, 1 }, 10, 20 });
        r.setLight(3, { Light::Type::Point, { -2, 0, 1 }, { 0, 0, 0 }, { 1, 1, 1 }, 10, 40 });
    }
    else
    {
        r.setLight(0, { Light::Type::Directional, { 0, 0, 0 }, { 0, 1, 0 }, { 1, 0, 0 }, 10, 10 });
        r.setLight(1, { Light::Type::Directional, { 0, 0, 0 }, { 1, 0, 0 }, { 0, 1, 0 }, 10, 10 });
        r.setLight(2, { Light::Type::Directional, { 0, 0, 0 }, { 0, -1, 0 }, { 0, 0, 1 }, 10, 10 });
        r.setLight(3, { Light::Type::Directional, { 0, 0, 0 }, { -1, 0, 0 }, { 1, 1, 1 }, 10, 10 });
    }

    while (!glfwWindowShouldClose(window))
    {
        /// 渲染
        r.clearScreen({ 1.0f, 0.0f, 0.0f, 1.0f });
        r.render(mesh, glm::eulerAngleY(glm::radians(360 * (float)glfwGetTime() * 0.1f)), shader);
        r.swapWindow();
    }
    glfwTerminate();
    exit(EXIT_SUCCESS);
}