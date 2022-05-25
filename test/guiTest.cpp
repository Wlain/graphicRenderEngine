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
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

using namespace re;

static int s_canvasWidth = 640;
static int s_canvasHeight = 480;
static constexpr char* title = "guiTest";

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
    auto* mesh = Mesh::createCube();
    static float f = 0.0f;
    //  init imageui
    ImGui::CreateContext();
    // 设置样式
    ImGui::StyleColorsDark();
    // 设置平台和渲染器
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImVec4 clearColor = ImColor(114, 144, 154);
    shader->set("specularity", 20.0f);
    r.setLight(0, Light(Light::Type::Point, { 0, 0, 10 }, { 0, 0, 0 }, { 1, 0, 0 }, 50));
    while (!glfwWindowShouldClose(window))
    {
        /// 渲染
        r.clearScreen({ clearColor.x, clearColor.y, clearColor.z, 1.0f });
        r.render(mesh, glm::eulerAngleY(glm::radians(360 * (float)glfwGetTime() * 0.1f)), shader);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Hello, world!");
        ImGui::SliderFloat("float", &f, 0.0f, 20.0f);
        ImGui::ColorEdit3("clear color", (float*)&clearColor);
        ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        r.swapWindow();
    }
    glfwTerminate();
    exit(EXIT_SUCCESS);
}