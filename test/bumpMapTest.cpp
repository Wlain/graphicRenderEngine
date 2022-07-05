//
// Created by cwb on 2022/7/5.
//
#include "basicProject.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
using namespace re;
class bumpMapExample : public BasicProject
{
public:
    ~bumpMapExample() override = default;
    void initialize() override
    {
        m_camera.setLookAt({ 0.0f, 0.0f, 3.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
        m_camera.setPerspectiveProjection(60.0f, 0.1f, 100.0f);
        m_mesh = Mesh::create().withCube().build();
        m_worldLights = MAKE_UNIQUE(m_worldLights);
        m_worldLights->addLight(Light::create().withDirectionalLight(glm::normalize(glm::vec3{ 1, 1, 1 })).build());
        m_specialization["S_NORMALMAP"] = "1";
        m_material = Shader::getStandardBlinnPhong()->createMaterial(m_specialization);
        m_material->setColor({ 1, 1, 1, 1 });
        m_material->setSpecularity(Color(1, 1, 1, 50));
        m_material->set("normalTex", Texture::create().withFile("resources/objFiles/spot/normal.jpg").withSamplerColorspace(Texture::SamplerColorspace::Gamma).build());
        m_material->set("normalScale", 1.0f);
    }
    void render() override
    {
        auto rp = RenderPass::create()
                      .withCamera(m_camera)
                      .withWorldLights(m_worldLights.get())
                      .withClearColor(true, { 1, 0, 0, 1 })
                      .build();

        rp.draw(m_mesh, glm::eulerAngleY(glm::radians(30 * m_totalTime)), m_material);
    }
    void setTitle() override
    {
        m_title = "bumpMapExample";
    }

private:
    std::map<std::string, std::string> m_specialization;
};

void bumpMapTest()
{
    bumpMapExample test;
    test.run();
}