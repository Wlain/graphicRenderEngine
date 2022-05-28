//
// Created by william on 2022/5/24.
//
#include "commonMacro.h"
#include "core/material.h"
#include "core/mesh.h"
#include "core/renderer.h"
#include "core/shader.h"
#include "core/texture.h"
#include "core/worldLights.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/euler_angles.hpp>

using namespace re;

static int s_canvasWidth = 640;
static int s_canvasHeight = 480;
static constexpr const char* title = "particleTest";
namespace
{
Mesh* createParticles(int size = 2500)
{
    std::vector<glm::vec3> positions;
    std::vector<glm::vec4> colors;
    std::vector<float> sizes;
    for (int i = 0; i < size; i++)
    {
        positions.push_back(glm::linearRand(glm::vec3(-1, -1, -1), glm::vec3(1, 1, 1)));
        colors.push_back(glm::linearRand(glm::vec4(0, 0, 0, 0), glm::vec4(1, 1, 1, 1)));
        sizes.push_back(glm::linearRand(0.0f, 1.0f));
    }
    return Mesh::create()
        .withVertexPosition(positions)
        .withColors(colors)
        .withParticleSize(sizes)
        .withMeshTopology(Mesh::Topology::Points)
        .build();
}
} // namespace

void particleTest()
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
    auto* shader = Shader::getStandard();
    auto* material = new Material(shader);
    material->setTexture(Texture::create().withFile(GET_CURRENT("test/resources/test.jpg")).build());
    material->setSpecularity(20.0f);
    auto* shaderParticle = Shader::getStandardParticles();
    auto* materialParticle = new Material(shaderParticle);
    auto* particleMesh = createParticles();
    materialParticle->setTexture(Texture::getSphereTexture());
    auto* mesh = Mesh::create().withCube().build();
    auto worldLights = std::make_unique<WorldLights>();
    worldLights->addLight(Light::create().withPointLight({ 0, 2, 5 }).withColor({ 1, 0, 0 }).withRange(10).build());
    worldLights->addLight(Light::create().withPointLight({ 2, 0, 5 }).withColor({ 0, 1, 0 }).withRange(10).build());
    worldLights->addLight(Light::create().withPointLight({ 0, -2, 5 }).withColor({ 0, 0, 1 }).withRange(10).build());
    worldLights->addLight(Light::create().withPointLight({ -2, 0, 5 }).withColor({ 1, 1, 1 }).withRange(10).build());

    while (!glfwWindowShouldClose(window))
    {
        /// 渲染
        auto renderPass = r.createRenderPass().withCamera(*camera).withWorldLights(worldLights.get()).build();
        renderPass.draw(mesh, glm::eulerAngleY(glm::radians(360 * (float)glfwGetTime() * 0.1f)) * glm::scale(glm::mat4(1), { 0.3f, 0.3f, 0.3f }), material);
        renderPass.draw(particleMesh, glm::eulerAngleY(glm::radians(360 * (float)glfwGetTime() * 0.1f)), materialParticle);
        r.swapWindow();
    }
    glfwTerminate();
    exit(EXIT_SUCCESS);
}