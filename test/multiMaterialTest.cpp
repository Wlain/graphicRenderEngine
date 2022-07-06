//
// Created by william on 2022/6/24.
//
#include "basicProject.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/rotate_vector.hpp>
class MultiMaterialExample : public BasicProject
{
public:
    using BasicProject::BasicProject;
    ~MultiMaterialExample() override = default;
    void initialize() override
    {
        m_camera.setLookAt({ 0, 0, 3 }, { 0, 0, 0 }, { 0, 1, 0 });
        m_camera.setPerspectiveProjection(60, 0.1, 100);
        m_camera2.setLookAt({ 0, 0, 3 }, { 0, 0, 0 }, { 0, 1, 0 });
        m_camera2.setPerspectiveProjection(60, 0.1, 100);
        m_camera2.setViewport({ 0, 0.8 }, { 0.15, 0.15 });
        m_materialPhong = Shader::getStandardBlinnPhong()->createMaterial();
        m_materialPhong->setColor({ 1.0f, 1.0f, 1.0f, 1.0f });
        m_materialPhong->setSpecularity(Color(.5, .5, .5, 180.0f));
        m_materialPhongRed = Shader::getStandardBlinnPhong()->createMaterial();
        m_materialPhongRed->setColor({ 1.0f, 0.0f, 0.0f, 1.0f });
        m_materialPhongRed->setSpecularity(Color(.5, .5, .5, 180.0f));
        m_materialPhongGreen = Shader::getStandardBlinnPhong()->createMaterial();
        m_materialPhongGreen->setColor({ 0.0f, 1.0f, 0.0f, 1.0f });
        m_materialPhongGreen->setSpecularity(Color(.5, .5, .5, 180.0f));
        m_materials = { m_materialPhong, m_materialPhongRed };
        m_meshSingle = Mesh::create().withCube().build();
        auto indices = m_meshSingle->getIndices(0);
        auto indicesFirst = std::vector<uint16_t>(indices.begin() + 0, indices.begin() + indices.size() / 2);
        auto indicesLast = std::vector<uint16_t>(indices.begin() + indices.size() / 2, indices.begin() + indices.size());
        m_mesh = Mesh::create()
                     .withCube()
                     .withIndices(indicesFirst, Mesh::Topology::Triangles, 0)
                     .withIndices(indicesLast, Mesh::Topology::Triangles, 1)
                     .build();
        m_worldLights = MAKE_UNIQUE(m_worldLights);
        m_worldLights->setAmbientLight({ 0.0, 0.0, 0.0 });
        m_worldLights->addLight(Light::create().withPointLight({ 0, 3, 0 }).withColor({ 1, 1, 1 }).withRange(20).build());
        m_worldLights->addLight(Light::create().withPointLight({ 3, 0, 0 }).withColor({ 1, 1, 1 }).withRange(20).build());
        m_worldLights->addLight(Light::create().withPointLight({ 0, -3, 0 }).withColor({ 1, 1, 1 }).withRange(20).build());
        m_worldLights->addLight(Light::create().withPointLight({ -3, 0, 0 }).withColor({ 1, 1, 1 }).withRange(20).build());
    }

    void render() override
    {
        auto renderPass = RenderPass::create()
                              .withCamera(m_camera)
                              .withWorldLights(m_worldLights.get())
                              .withClearColor(true, { 1, 0, 0, 1 })
                              .build();
        renderPass.draw(m_mesh, glm::eulerAngleY(glm::radians(30.0f * m_totalTime)), m_materials);
        renderPass.finish();
        auto renderPass2 = RenderPass::create()
                               .withCamera(m_camera2)
                               .withWorldLights(m_worldLights.get())
                               .withClearColor(true, { 1, 0, 0, 1 })
                               .build();
        renderPass2.draw(m_meshSingle, glm::eulerAngleY(glm::radians(30.0f * m_totalTime)) , m_materialPhongGreen);
    }
    void setTitle() override
    {
        m_title = "MultiMaterialExample";
    }

private:
    std::shared_ptr<Material> m_materialPhong;
    std::shared_ptr<Material> m_materialPhongRed;
    std::shared_ptr<Material> m_materialPhongGreen;
    std::shared_ptr<Mesh> m_meshSingle;
    std::vector<std::shared_ptr<Material>> m_materials;
    Camera m_camera2;
};

void multiMaterialTest()
{
    MultiMaterialExample test;
    test.run();
}