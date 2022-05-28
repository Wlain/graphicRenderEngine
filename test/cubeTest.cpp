//
// Created by william on 2022/5/22.
//
#include "commonMacro.h"
#include "core/material.h"
#include "core/mesh.h"
#include "core/renderer.h"
#include "core/shader.h"
#include "core/worldLights.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

using namespace re;

static int s_canvasWidth = 640;
static int s_canvasHeight = 480;
static constexpr const char* title = "cubeTest";

void cubeTest()
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
    camera->setLookAt({ 0.0f, 0.0f, 4.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
    camera->setPerspectiveProjection(60.0f, s_canvasWidth, s_canvasWidth, 0.1f, 100.0f);
    auto shader = std::unique_ptr<Shader>(Shader::getStandard());
    auto* material = new Material(shader.get());
    material->setTexture(Texture::getFontTexture());
    auto mesh = std::unique_ptr<Mesh>(Mesh::create().withCube().build());
    auto worldLights = std::make_unique<WorldLights>();
    worldLights->addLight(Light::create().withPointLight({ 3, 0, 0 }).withColor({ 0, 1, 0 }).withRange(20).build());
    worldLights->addLight(Light::create().withPointLight({ 0, -3, 0 }).withColor({ 0, 0, 1 }).withRange(20).build());
    worldLights->addLight(Light::create().withPointLight({ -3, 0, 0 }).withColor({ 1, 1, 1 }).withRange(20).build());
    while (!glfwWindowShouldClose(window))
    {
        /// 渲染
        auto renderPass = r.createRenderPass().withCamera(*camera).withWorldLights(worldLights.get()).build();
        renderPass.draw(mesh.get(), glm::eulerAngleY(glm::radians(360 * (float)glfwGetTime() * 0.1f)), material);
        r.swapWindow();
    }
    glfwTerminate();
    exit(EXIT_SUCCESS);
}