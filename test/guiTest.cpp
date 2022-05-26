//
// Created by william on 2022/5/24.
//
#include "commonMacro.h"
#include "core/mesh.h"
#include "core/renderer.h"
#include "core/shader.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

using namespace re;

static int s_canvasWidth = 640;
static int s_canvasHeight = 480;
static constexpr const char* title = "guiTest";

void guiTest()
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
    r.getCamera()->setLookAt({ 0, 0, 3 }, { 0, 0, 0 }, { 0, 1, 0 });
    r.getCamera()->setPerspectiveProjection(60, 640, 480, 0.1, 100);
    auto* shader = Shader::getStandard();
    auto* mesh = Mesh::create().withCube().build();
    float specularity = 20.0f;
    //  init imageui
    ImGui::CreateContext();
    // 设置样式
    ImGui::StyleColorsDark();
    // 设置平台和渲染器
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImVec4 clearColor = ImColor(114, 144, 154);
    r.setLight(0, Light::create().withPointLight({ 0, 0, 10 }).withColor({ 1, 0, 0 }).withRange(50).build());
    while (!glfwWindowShouldClose(window))
    {
        /// 渲染
        r.clearScreen({ clearColor.x, clearColor.y, clearColor.z, 1.0f });
        shader->set("specularity", specularity);
        r.render(mesh, glm::eulerAngleY(glm::radians(360 * (float)glfwGetTime() * 0.1f)), shader);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("guiTest");
        ImGui::SliderFloat("specularity", &specularity, 0.0f, 40.0f);
        ImGui::ColorEdit3("clear color", (float*)&clearColor);
        ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        auto& renderStats = r.getRenderStats();
        float bytesToMB = 1.0f; /// (1024 * 1024);
        ImGui::Text("re draw-calls %i meshes %i (%.2fbytes) textures %i (%.2fbytes) shaders %i", renderStats.drawCalls,
                    renderStats.meshCount, renderStats.meshBytes * bytesToMB, renderStats.textureCount,
                    renderStats.textureBytes * bytesToMB, renderStats.shaderCount);
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        r.swapWindow();
    }
    glfwTerminate();
    exit(EXIT_SUCCESS);
}