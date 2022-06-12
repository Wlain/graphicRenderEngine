// Copyright (c) 2022. Lorem ipsum dolor sit amet, consectetur adipiscing elit.
// Morbi non lorem porttitor neque feugiat blandit. Ut vitae ipsum eget quam lacinia accumsan.
// Etiam sed turpis ac ipsum condimentum fringilla. Maecenas magna.
// Proin dapibus sapien vel ante. Aliquam erat volutpat. Pellentesque sagittis ligula eget metus.
// Vestibulum commodo. Ut rhoncus gravida arcu.

//
// Created by william on 2022/5/31.
//

#include "basicProject.h"
#include "core/framebuffer.h"
#include "core/texture.h"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
using namespace re;

class RenderToFrameBufferExample : public BasicProject
{
public:
    using BasicProject::BasicProject;
    ~RenderToFrameBufferExample() override = default;
    void initialize() override
    {
        m_camera.setLookAt({ 0, 0, 3 }, { 0, 0, 0 }, { 0, 1, 0 });
        m_camera.setPerspectiveProjection(60, 0.1, 100);
        m_texture = Texture::create().withRGBAData(nullptr, 1024, 1024).withName("Create Texture").build();
        m_framebuffer = FrameBuffer::create().withTexture(m_texture).build();
        m_materialOffscreen = Shader::getStandard()->createMaterial();
        m_material = Shader::getStandard()->createMaterial();
        m_material->setTexture(m_texture);
        m_mesh = Mesh::create().withCube().build();
        m_worldLights = std::make_unique<WorldLights>();
        m_worldLights->addLight(Light::create().withPointLight({ 0, 0, 3 }).withColor({ 1, 1, 1 }).withRange(20).build());
    }
    void render() override
    {
        auto renderToTexturePass = RenderPass::create()
                                       .withCamera(m_camera)
                                       .withWorldLights(m_worldLights.get())
                                       .withFramebuffer(m_framebuffer)
                                       .withClearColor(true, { 0, 1, 1, 0 })
                                       .withGUI(false)
                                       .build();
        renderToTexturePass.draw(m_mesh, glm::eulerAngleY(glm::radians((float)m_totalTime * 30)), m_materialOffscreen);
        auto renderPass = RenderPass::create()
                              .withCamera(m_camera)
                              .withWorldLights(m_worldLights.get())
                              .withClearColor(true, { 1, 0, 0, 1 })
                              .withGUI(true)
                              .build();
        renderPass.draw(m_mesh, glm::eulerAngleY(glm::radians(30.0f * m_totalTime)), m_material);
    }
    void setTitle() override
    {
        m_title = "RenderToFrameBufferExample";
    }

private:
    std::shared_ptr<FrameBuffer> m_framebuffer;
    std::shared_ptr<Texture> m_texture;
    std::shared_ptr<Material> m_materialOffscreen;
};

void renderToFrameBufferTest()
{
    RenderToFrameBufferExample test;
    test.run();
}