//
// Created by william on 2022/5/24.
//

#include "commonMacro.h"
#include "core/mesh.h"
#include "core/renderer.h"
#include "core/shader.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>

using namespace re;

static int s_canvasWidth = 640;
static int s_canvasHeight = 480;
static constexpr char* title = "sphereTest2";

void sphereTest2()
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
    r.getCamera()->setLookAt({ 0.0f, 0.0f, 3.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
    r.getCamera()->setPerspectiveProjection(60.0f, s_canvasWidth, s_canvasHeight, 0.1f, 100.0f);
    Shader* shader = Shader::getStandard();
    Mesh* mesh = Mesh::createSphere();
    r.setLight(0, { Light::Type::Directional, { 0, 0, 0 }, glm::normalize(glm::vec3(1, 1, 1)), { 1, 1, 1 }, 10 });
    glm::mat4 pos1 = glm::translate(glm::mat4(1), { -1, 0, 0 });
    glm::mat4 pos2 = glm::translate(glm::mat4(1), { 1, 0, 0 });
    while (!glfwWindowShouldClose(window))
    {
        /// 渲染
        r.clearScreen({ 1, 0, 0, 1 });
        shader->set("color", { 1, 1, 1, 1 });
        shader->set("specularity", 50.0f);
        shader->set("tex", Texture::getWhiteTexture());
        r.render(mesh, pos1, shader);
        shader->set("color", { 1, 0, 0, 1 });
        shader->set("specularity", 0.0f);
        shader->set("tex", Texture::getWhiteTexture());
        r.render(mesh, pos2, shader);
        r.swapWindow();
    }
    glfwTerminate();
    exit(EXIT_SUCCESS);
}