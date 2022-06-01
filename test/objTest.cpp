//
// Created by william on 2022/5/31.
//

#include "basicProject.h"
#include "core/camera.h"
#include "core/modelImporter.h"
#include "guiCommonDefine.h"

#include <filesystem>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
using namespace re;
class ObjExample : public BasicProject
{
public:
    ~ObjExample() override = default;
    void initialize() override
    {
        m_camera = MAKE_UNIQUE(m_camera);
        m_camera->setLookAt({ 0, 0, 3 }, { 0, 0, 0 }, { 0, 1, 0 });
        auto material = Shader::getStandard()->createMaterial();
        material->setColor({ 1.0f, 1.0f, 1.0f, 1.0f });
        material->setSpecularity(20.0f);
        m_materials.push_back(material);
        m_mesh = Mesh::create().withCube().build();
        m_worldLights = MAKE_UNIQUE(m_worldLights);
        m_worldLights->setAmbientLight({ 0.5, 0.5, 0.5 });
        m_worldLights->addLight(Light::create().withPointLight({ 0, 3, 0 }).withColor({ 1, 0, 0 }).withRange(20).build());
        m_worldLights->addLight(Light::create().withPointLight({ 3, 0, 0 }).withColor({ 0, 1, 0 }).withRange(20).build());
        m_worldLights->addLight(Light::create().withPointLight({ 0, -3, 0 }).withColor({ 0, 0, 1 }).withRange(20).build());
        m_worldLights->addLight(Light::create().withPointLight({ -3, 0, 0 }).withColor({ 1, 1, 1 }).withRange(20).build());
        //        loadObjFile("/Users/william/Developer/demo/simpleRenderEngine/test/resources/obj-testfiles/empty_mat.obj");
    }
    void render() override
    {
        auto renderPass = RenderPass::create()
                              .withCamera(*m_camera)
                              .withWorldLights(m_worldLights.get())
                              .withClearColor(true, { 0, 0, 0, 1 })
                              .build();
        renderPass.draw(m_mesh, glm::eulerAngleY(glm::radians(30.0f * m_totalTime)), m_materials);
    }

    void loadObjFile(std::string file)
    {
        std::filesystem::path path(file);
        m_materials.clear();
        m_mesh = ModelImporter::importObj(path, m_materials);
        for (const auto& m : m_materials)
        {
            LOG_INFO("{}", m->getName());
        }
        LOG_INFO("{} {}", m_materials.size(), m_mesh->getIndexSets());
        auto bounds = m_mesh->getBoundsMinMax();
        m_farPlane = glm::length(bounds[1] - bounds[0]);
        m_camera->setLookAt({ 0, 1, m_farPlane }, { 0, 0, 0 }, { 0, 1, 0 });
        m_camera->setPerspectiveProjection(60, 0.1, m_farPlane * 2);
    }

    void dropEvent(int count, const char** paths) override
    {
        const char* filename = paths[0];
        LOG_INFO("load file is:{}", filename);
        loadObjFile(filename);
    }

    void setTitle() override
    {
        m_title = "ObjExample";
    }

private:
    std::vector<std::shared_ptr<Material>> m_materials;
    glm::vec3 m_offset{ 0 };
    float m_farPlane{ 100.0f };
};

void objTest()
{
    ObjExample test;
    test.run();
}