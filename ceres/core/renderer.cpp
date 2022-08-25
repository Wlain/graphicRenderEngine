//
// Created by william on 2022/5/22.
//

#include "renderer.h"

#include "commonMacro.h"
#include "guiCommonDefine.h"
#include "contextGL.h"
// render engine
namespace ceres
{
Renderer::Renderer(GLFWwindow* window, bool vsync, int maxSceneLights) :
    m_window(window), m_vsync(vsync), m_maxSceneLights(maxSceneLights)
{
    if (s_instance != nullptr)
    {
        LOG_ERROR("multiple versions of Renderer initialized. Only a single instance is supported.");
    }
    s_instance = this;
    glfwMakeContextCurrent(window);
    if (m_vsync)
    {
        glfwSwapInterval(1); // 开启垂直同步
    }
    m_renderInfo.useFramebufferSRGB = true;
    m_renderInfo.supportTextureSamplerSRGB = true;
    m_renderInfo.supportFBODepthAttachment = true;
    glEnable(GL_FRAMEBUFFER_SRGB);
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        exit(EXIT_FAILURE);
        /* Problem: glewInit failed, something is seriously wrong. */
        LOG_FATAL("Error initializing OpenGL using GLEW: {}", glewGetErrorString(err));
    }
    // initialize ImGUI
    ImGui::CreateContext();
    // 设置平台和渲染器
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    m_renderInfo.graphicsAPIVersion = (char*)glGetString(GL_VERSION);
    m_renderInfo.graphicsAPIVendor = (char*)glGetString(GL_VENDOR);
    m_renderInfo.graphicVendor = (char*)glGetString(GL_VENDOR);
    m_renderInfo.graphicRenderer = (char*)glGetString(GL_RENDERER);
    m_renderInfo.graphicVersion = (char*)glGetString(GL_VERSION);
    m_renderInfo.graphicShader = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
    LOG_INFO("graphic OpenGL version: {}", m_renderInfo.graphicsAPIVersion.c_str());
    LOG_INFO("graphic OpenGL Vendor: {}", m_renderInfo.graphicVendor.c_str());
    LOG_INFO("graphic OpenGL Renderer: {}", m_renderInfo.graphicRenderer.c_str());
    LOG_INFO("graphic OpenGL Version: {}", m_renderInfo.graphicVersion.c_str());
    LOG_INFO("graphic OpenGL Shader: {}", m_renderInfo.graphicShader.c_str());
    LOG_INFO("rg version: {}.{}.{}", s_rgVersionMajor, s_rgVersionMinor, s_rgVersionPoint);
    ContextGL::GLInfo info{};
    glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &info.maxRenderbufferSize);
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &info.maxUniformBlockSize);
    glGetIntegerv(GL_MAX_SAMPLES, &info.maxSamples);
    glGetIntegerv(GL_MAX_DRAW_BUFFERS, &info.maxDrawBuffers);
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &info.maxTextureImageUnits);
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &info.uniformBufferOffsetAlignment);
    LOG_INFO("GL_MAX_DRAW_BUFFERS = {}", info.maxDrawBuffers);
    LOG_INFO("GL_MAX_RENDERBUFFER_SIZE = {}", info.maxRenderbufferSize);
    LOG_INFO("GL_MAX_SAMPLES = {}", info.maxSamples);
    LOG_INFO("GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT = {}", info.maxTextureImageUnits);
    LOG_INFO("GL_MAX_UNIFORM_BLOCK_SIZE = {}", info.maxUniformBlockSize);
    LOG_INFO("GL_MAX_TEXTURE_IMAGE_UNITS = {}", info.maxTextureImageUnits);
    LOG_INFO("GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT = {}", info.uniformBufferOffsetAlignment);
    // setup opengl context
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    initGlobalUniformBuffer();
    // reset render stats
    m_renderStatsCurrent.frame = 0;
    m_renderStatsCurrent.meshCount = 0;
    m_renderStatsCurrent.meshBytes = 0;
    m_renderStatsCurrent.textureCount = 0;
    m_renderStatsCurrent.textureBytes = 0;
    m_renderStatsCurrent.drawCalls = 0;
    m_renderStatsLast = m_renderStatsCurrent;
}

Renderer::~Renderer()
{
    glDeleteBuffers(1, &m_globalUniformBuffer);
};

void Renderer::swapWindow()
{
    m_renderStatsLast = m_renderStatsCurrent;
    m_renderStatsCurrent.frame++;
    m_renderStatsCurrent.drawCalls = 0;
    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

glm::ivec2 Renderer::getFramebufferSize()
{
    glm::ivec2 size;
    glfwGetFramebufferSize(m_window, &size.r, &size.g);
    return size;
}

glm::ivec2 Renderer::getWindowSize()
{
    glm::ivec2 size;
    glfwGetWindowSize(m_window, &size.r, &size.g);
    return size;
}

int Renderer::getMaxSceneLights() const
{
    return m_maxSceneLights;
}

const Renderer::RenderInfo& Renderer::renderInfo()
{
    return m_renderInfo;
}

void Renderer::initGlobalUniformBuffer()
{
    glGenBuffers(1, &m_globalUniformBuffer);
    size_t lightSize = sizeof(glm::vec4) * (1 + m_maxSceneLights * 2); // ambient + (lightPosType + lightColorRange) * maxSceneLights
    m_globalUniformBufferSize = sizeof(glm::mat4) * 2 + sizeof(glm::vec4) * 2 + lightSize;
    glBindBuffer(GL_UNIFORM_BUFFER, m_globalUniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, m_globalUniformBufferSize, nullptr, GL_STREAM_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
} // namespace ceres