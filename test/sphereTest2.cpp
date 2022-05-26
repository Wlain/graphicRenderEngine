//
// Created by william on 2022/5/24.
//

#include "commonMacro.h"
#include "core/mesh.h"
#include "core/renderer.h"
#include "core/shader.h"
#include "core/worldLights.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>

using namespace re;

static int s_canvasWidth = 640;
static int s_canvasHeight = 480;
static constexpr const char* title = "sphereTest2";

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
    auto camera = std::make_unique<Camera>();
    camera->setLookAt({ 0.0f, 0.0f, 3.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
    camera->setPerspectiveProjection(60.0f, s_canvasWidth, s_canvasHeight, 0.1f, 100.0f);
    Shader* shader = Shader::getStandard();
    auto* mesh = Mesh::create().withSphere().build();
    auto worldLights = std::make_unique<WorldLights>();
    worldLights->addLight(Light::create().withDirectionalLight({ 0, 2, 1 }).withColor({ 1, 1, 1 }).withRange(10).build());
    glm::mat4 pos1 = glm::translate(glm::mat4(1), { -1, 0, 0 });
    glm::mat4 pos2 = glm::translate(glm::mat4(1), { 1, 0, 0 });
    auto renderPass = r.createRenderPass()
                          .withCamera(*camera)
                          .withWorldLights(worldLights.get())
                          .build();
    while (!glfwWindowShouldClose(window))
    {
        /// 渲染
        renderPass.clearScreen({ 1, 0, 0, 1 });
        shader->set("color", { 1, 1, 1, 1 });
        shader->set("specularity", 50.0f);
        shader->set("tex", Texture::getWhiteTexture());
        renderPass.draw(mesh, pos1, shader);
        shader->set("color", { 1, 0, 0, 1 });
        shader->set("specularity", 0.0f);
        shader->set("tex", Texture::getWhiteTexture());
        renderPass.draw(mesh, pos2, shader);
        r.swapWindow();
    }
    glfwTerminate();
    exit(EXIT_SUCCESS);
}