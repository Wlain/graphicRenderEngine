////
//// Created by william on 2022/6/2.
////
//#include "basicProject.h"
//#include "commonMacro.h"
//#include "core/camera.h"
//#include "core/material.h"
//#include "core/mesh.h"
//#include "core/worldLights.h"
//#include "guiCommonDefine.h"
//
//#include <glm/glm.hpp>
//#define GLM_ENABLE_EXPERIMENTAL
//#include <glm/gtx/euler_angles.hpp>
//using namespace re;
//class PolygonOffsetExample : public BasicProject
//{
//public:
//    using BasicProject::BasicProject;
//    ~PolygonOffsetExample() override = default;
//    void initialize() override
//    {
//        m_camera.setLookAt({ 0, 0, 3 }, { 0, 0, 0 }, { 0, 1, 0 });
//
//        m_material = Shader::create().withSourceStandard().withCullFace(Shader::CullFace::None).build()->createMaterial();
//        m_material->setColor({ 1.0f, 1.0f, 1.0f, 1.0f });
//        m_material->setSpecularity(20.0f);
//
//        m_material2 = Shader::create().withSourceStandard().withOffset(m_factor, m_offset).build()->createMaterial();
//        m_material2->setColor({ 1.0f, 0.0f, 0.0f, 1.0f });
//        m_material2->setSpecularity(20.0f);
//
//        m_mesh = Mesh::create().withQuad(10.9999999f).build();
//        m_mesh2 = Mesh::create().withQuad(10.000001f).build();
//        m_worldLights = MAKE_UNIQUE(m_worldLights);
//        m_worldLights->setAmbientLight({ 0.5, 0.5, 0.5 });
//        m_worldLights->addLight(Light::create().withPointLight({ 0, 3, 10 }).withColor({ 1, 0, 0 }).withRange(20).build());
//        m_worldLights->addLight(Light::create().withPointLight({ 3, 0, 10 }).withColor({ 0, 1, 0 }).withRange(20).build());
//        m_worldLights->addLight(Light::create().withPointLight({ 0, -3, 10 }).withColor({ 0, 0, 1 }).withRange(20).build());
//        m_worldLights->addLight(Light::create().withPointLight({ -3, 0, 10 }).withColor({ 1, 1, 1 }).withRange(20).build());
//    }
//    void render() override
//    {
//        m_camera.setPerspectiveProjection(60, 0.001, 10000000);
//        glm::mat4 rot = glm::eulerAngleY(glm::radians(30.0f * m_totalTime)) * glm::eulerAngleX(glm::radians((float)sin((float)30.0f * m_totalTime) * 30.8f));
//        m_camera.setLookAt((glm::vec3)(rot * glm::vec4(0, 0, 30, 1)), { 0, 0, 0 }, { 0, 1, 0 });
//        auto renderPass = RenderPass::create()
//                              .withCamera(m_camera)
//                              .withWorldLights(m_worldLights.get())
//                              .withClearColor(true, { 1, 0, 0, 1 })
//                              .build();
//        renderPass.draw(m_mesh, glm::mat4(1), m_material);
//        renderPass.draw(m_mesh2, glm::mat4(1), m_material2);
//
//        bool changed = ImGui::SliderFloat2("Factor/Offset", &m_factor, 0, 3);
//        if (changed)
//        {
//            m_material2 = Shader::create().withSourceStandard().withOffset(m_factor, m_offset).build()->createMaterial();
//            m_material2->setColor({ 1.0f, 0.0f, 0.0f, 1.0f });
//            m_material2->setSpecularity(Color(1, 1, 1, 20.0f));
//        }
//        ImGui::Checkbox("Rotate", &m_rotate);
//        m_inspector.update();
//        m_inspector.gui();
//    }
//    void setTitle() override
//    {
//        m_title = "PolygonOffsetExample";
//    }
//
//private:
//    std::shared_ptr<Mesh> m_mesh2;
//    std::shared_ptr<Material> m_material2;
//    float m_factor = 0.0f;
//    float m_offset = 0.0f;
//    bool m_rotate = true;
//};
//
//void polygonOffsetTest()
//{
//    PolygonOffsetExample test;
//    test.run();
//}