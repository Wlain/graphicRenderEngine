//
// Created by william on 2022/5/24.
//

#include "commonMacro.h"
#include "core/mesh.h"
#include "core/renderer.h"
#include "core/shader.h"
#include "core/texture.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

using namespace re;

static int s_canvasWidth = 640;
static int s_canvasHeight = 480;
static constexpr char* title = "quadTest";

void quadTest()
{
    LOG_INFO("{}", title);
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    // glfw window creation
    auto window = glfwCreateWindow(s_canvasWidth, s_canvasHeight, title, nullptr, nullptr);
    if (window == nullptr)
    {
        glfwTerminate();
    }
    Renderer r{ window };
    r.getCamera()->setLookAt({ 0.0f, 0.0f, 4.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
    r.getCamera()->setPerspectiveProjection(45.0f, s_canvasWidth, s_canvasHeight, 0.1f, 100.0f);
    Shader* shader = Shader::getUnlit();
    shader->set("tex", Texture::create().withFile(GET_CURRENT("test/resources/test.jpg")).build());
    auto* mesh = Mesh::create().withQuad().build();
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