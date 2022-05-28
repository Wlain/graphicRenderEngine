// Copyright (c) 2022. Lorem ipsum dolor sit amet, consectetur adipiscing elit.
// Morbi non lorem porttitor neque feugiat blandit. Ut vitae ipsum eget quam lacinia accumsan.
// Etiam sed turpis ac ipsum condimentum fringilla. Maecenas magna.
// Proin dapibus sapien vel ante. Aliquam erat volutpat. Pellentesque sagittis ligula eget metus.
// Vestibulum commodo. Ut rhoncus gravida arcu.

//
// Created by william on 2022/5/26.
//

#include "commonMacro.h"
#include "core/material.h"
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
static constexpr const char* title = "cubeMapText";

static constexpr const char* vertexShaderStr = R"(#version 330
        in vec4 position;
        in vec3 normal;
        in vec2 uv;
        out vec3 vNormal;

        uniform mat4 g_model;
        uniform mat4 g_view;
        uniform mat4 g_projection;
        uniform mat3 g_normalMat;

        void main(void) {
            gl_Position = g_projection * g_view * g_model * position;
            vNormal = normal;
        }
    )";
static constexpr const char* fragmentShaderStr = R"(#version 330
        out vec4 fragColor;
        in vec3 vNormal;

        uniform samplerCube tex;

        void main(void)
        {
            fragColor = texture(tex, vNormal);
        }
    )";

void cubeMapText()
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
    glm::vec3 eye{ 0, 0, 3 };
    glm::vec3 at{ 0, 0, 0 };
    glm::vec3 up{ 0, 1, 0 };
    auto camera = std::make_unique<Camera>();
    camera->setLookAt(eye, at, up);
    camera->setPerspectiveProjection(60.0f, s_canvasWidth, s_canvasHeight, 0.1f, 100.0f);
    auto* shader = Shader::create().withSource(vertexShaderStr, fragmentShaderStr).build();
    auto* material = new Material(shader);
    auto* tex = Texture::create()
                    .withFileCubeMap(GET_CURRENT("test/resources/cube/cube-posx.png"), Texture::TextureCubemapSide::PositiveX)
                    .withFileCubeMap(GET_CURRENT("test/resources/cube/cube-negx.png"), Texture::TextureCubemapSide::NegativeX)
                    .withFileCubeMap(GET_CURRENT("test/resources/cube/cube-posy.png"), Texture::TextureCubemapSide::PositiveY)
                    .withFileCubeMap(GET_CURRENT("test/resources/cube/cube-negy.png"), Texture::TextureCubemapSide::NegativeY)
                    .withFileCubeMap(GET_CURRENT("test/resources/cube/cube-posz.png"), Texture::TextureCubemapSide::PositiveZ)
                    .withFileCubeMap(GET_CURRENT("test/resources/cube/cube-negz.png"), Texture::TextureCubemapSide::NegativeZ)
                    .build();

    material->setTexture(tex);
    auto* mesh = Mesh::create().withSphere().build();

    while (!glfwWindowShouldClose(window))
    {
        /// 渲染
        auto renderPass = r.createRenderPass().withCamera(*camera).build();
        renderPass.draw(mesh, glm::eulerAngleY(glm::radians(360 * (float)glfwGetTime() * 0.1f)), material);
        r.swapWindow();
    }
    glfwTerminate();
    exit(EXIT_SUCCESS);
}