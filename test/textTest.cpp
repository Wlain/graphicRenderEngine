//
// Created by william on 2022/5/24.
//

#include "core/text.h"

#include "commonMacro.h"
#include "core/mesh.h"
#include "core/renderer.h"
#include "core/shader.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

using namespace re;

static int s_canvasWidth = 640;
static int s_canvasHeight = 480;

void textText()
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
    r.getCamera()->setOrthographicProjection(0.0f, s_canvasWidth, 0.0f, s_canvasHeight, -1.f, 1.f);
    r.getCamera()->setViewTransform(glm::mat4(1));
    auto* fontShader = Shader::getFont();
    // create a text mesh (pivot point in lower left corner)
    auto* mesh = Text::createTextMesh("Hello world");
    auto m = glm::mat4(1);
    while (!glfwWindowShouldClose(window))
    {
        /// 渲染
        r.clearScreen({ 1.0f, 0.0f, 0.0f, 1.0f });
        r.render(mesh, glm::translate(m, glm::vec3(s_canvasWidth / 4, s_canvasHeight / 2, 0.0)), fontShader);
        r.swapWindow();
    }
    glfwTerminate();
    exit(EXIT_SUCCESS);
}