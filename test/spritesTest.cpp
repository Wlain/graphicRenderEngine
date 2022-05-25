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

using namespace re;

static int s_canvasWidth = 640;
static int s_canvasHeight = 480;
static constexpr char* title = "spriteTest";
namespace
{
ParticleMesh* createParticles()
{
    std::vector<glm::vec3> positions;
    std::vector<glm::vec4> colors;
    std::vector<glm::vec2> uvCenter;
    std::vector<float> empty;
    std::vector<float> sizes;
    positions.emplace_back(0, 0, 0);
    sizes.emplace_back(10.0f);
    return new ParticleMesh(positions, colors, uvCenter, empty, empty, sizes);
}

void updateParticlesAnimation(float time, glm::vec2& pos, float& size, float& rotation)
{
    int frame = ((int)(time * 10)) % 16;
    int frameX = 3 - frame % 4;
    int frameY = frame / 4;
    pos = glm::vec2(frameX * 0.25f, frameY * 0.25f);
    size = 0.25f;
    rotation = 0;
}

void updateParticles(ParticleMesh* particleMesh, glm::vec2 uv, float uvSize, float rotation, float size)
{
    std::vector<glm::vec3> positions;
    std::vector<glm::vec4> colors;
    std::vector<glm::vec2> uvCenter;
    std::vector<float> uvSizes;
    std::vector<float> uvRotations;
    std::vector<float> sizes;

    positions.emplace_back(0, 0, 0);
    uvCenter.push_back(uv);
    uvSizes.push_back(uvSize);
    uvRotations.push_back(rotation);
    sizes.push_back(size);

    particleMesh->update(positions, colors, uvCenter, uvSizes, uvRotations, sizes);
}
} // namespace

void spriteTest()
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
    r.getCamera()->setViewport(0, 0, s_canvasWidth / 100, s_canvasHeight / 100);
    r.getCamera()->setLookAt({ 0.0f, 0.0f, 3.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
    r.getCamera()->setPerspectiveProjection(60.0f, s_canvasWidth, s_canvasHeight, 0.1f, 100.0f);
    auto* shaderParticle = Shader::getStandardParticles();
    shaderParticle->set("tex", Texture::create().withFile(GET_CURRENT("test/resources/sprite.png")).build());
    ParticleMesh* particleMesh = createParticles();
    glm::vec4 spriteColor = glm::vec4(114, 144, 154, 255);
    glm::vec2 spriteUV = glm::vec2(0, 0);
    float uvSize = 1.0;
    float uvRotation = 0.0;
    float size = 2000.0f;
    float time = 0;
    while (!glfwWindowShouldClose(window))
    {
        /// 渲染
        r.clearScreen({ 1.0f, 0.0f, 0.0f, 1.0f });
        updateParticlesAnimation(time, spriteUV, uvSize, uvRotation);
        updateParticles(particleMesh, spriteUV, uvSize, uvRotation, size);
        r.render(particleMesh, glm::mat4(1), shaderParticle);
        r.swapWindow();
        time += 0.016f;
    }
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
