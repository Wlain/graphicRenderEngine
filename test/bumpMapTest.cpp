//
// Created by cwb on 2022/7/5.
//
#include "basicProject.h"
#include "core/modelImporter.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

class bumpMapExample : public BasicProject
{
public:
    ~bumpMapExample() override = default;
    void initialize() override
    {
        m_camera.setLookAt({ 0.0f, 0.0f, 4.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
        m_camera.setPerspectiveProjection(60.0f, 0.1f, 100.0f);
        std::vector<std::shared_ptr<Material>> materialUnused;
        m_mesh = ModelImporter::importObj("resources/objFiles/spot/spot_triangulated_good.obj", materialUnused);
        m_worldLights = MAKE_UNIQUE(m_worldLights);
        m_worldLights->addLight(Light::create().withPointLight({ -10, -10, 10 }).withColor({ 1, 1, 1 }).build());
        m_worldLights->addLight(Light::create().withPointLight({ -30, -30, 10 }).withColor({ 1, 1, 1 }).build());
        m_worldLights->addLight(Light::create().withPointLight({ 30, -30 + 180, 10 }).withColor({ 1, 1, 1 }).build());
        m_specialization["S_NORMALMAP"] = "1";
        m_material = Shader::getStandardBlinnPhong()->createMaterial(m_specialization);
        m_materialNormalDebug = Shader::create()
                                    .withSourceFile("embeddedResource/debug_normal_vert.glsl", Shader::ShaderType::Vertex)
                                    .withSourceFile("embeddedResource/debug_normal_frag.glsl", Shader::ShaderType::Fragment)
                                    .build()
                                    ->createMaterial();
        m_materialUvDebug = Shader::create()
                                .withSourceFile("embeddedResource/debug_uv_vert.glsl", Shader::ShaderType::Vertex)
                                .withSourceFile("embeddedResource/debug_uv_frag.glsl", Shader::ShaderType::Fragment)
                                .build()
                                ->createMaterial();
        m_material->setColor({ 1, 1, 1, 1 });
        m_material->setSpecularity(Color(0, 0, 1, 50));
        m_material->set("normalTex", Texture::create().withFile("resources/objFiles/spot/normal.jpg").withSamplerColorspace(Texture::SamplerColorspace::Gamma).build());
        m_material->set("tex", Texture::create().withFile("resources/objFiles/spot/spot_texture.png").withSamplerColorspace(Texture::SamplerColorspace::Linear).build());
        m_material->set("normalScale", 2.0f);
    }
    void render() override
    {
        auto rp = RenderPass::create()
                      .withCamera(m_camera)
                      .withWorldLights(m_worldLights.get())
                      .withClearColor(true, { 0, 0, 0, 1 })
                      .build();
        auto rotateMatrix = glm::eulerAngleY(glm::radians(30 * m_totalTime));
        rp.draw(m_mesh, rotateMatrix, m_material);
        rp.draw(m_mesh, glm::translate(glm::vec3(2, 0, 0)) * rotateMatrix, m_materialNormalDebug);
        rp.draw(m_mesh, glm::translate(glm::vec3(-2, 0, 0)) * rotateMatrix, m_materialUvDebug);
    }
    void setTitle() override
    {
        m_title = "bumpMapExample";
    }

private:
    std::map<std::string, std::string> m_specialization;
    std::shared_ptr<Material> m_materialNormalDebug;
    std::shared_ptr<Material> m_materialUvDebug;
};

void bumpMapTest()
{
    bumpMapExample test;
    test.run();
}