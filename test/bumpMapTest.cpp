//
// Created by cwb on 2022/7/5.
//
#include "core/modelImporter.h"
#include "engineTestSimple.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

class bumpMapExample : public CommonInterface
{
public:
    using CommonInterface::CommonInterface;
    ~bumpMapExample() override = default;
    void initialize() override
    {
        m_camera.setLookAt({ 0.0f, 0.0f, 4.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
        m_camera.setPerspectiveProjection(60.0f, 0.1f, 100.0f);
        m_mesh = ModelImporter::importObj("resources/objFiles/spot/spot_triangulated_good.obj");
        m_meshQube = Mesh::create().withCube().build();
        m_mesh = m_meshQube;
        m_worldLights = MAKE_UNIQUE(m_worldLights);
        m_worldLights->addLight(Light::create().withPointLight({ -10, -10, 10 }).withColor({ 1, 1, 1 }).build());
        m_worldLights->addLight(Light::create().withPointLight({ -30, -30, 10 }).withColor({ 1, 1, 1 }).build());
        m_worldLights->addLight(Light::create().withPointLight({ 30, -30 + 180, 10 }).withColor({ 1, 1, 1 }).build());
        m_materialNormalMapping = Shader::create()
                                      .withSourceFile("shaders/bumpMapping/normal_mapping_vert.glsl", Shader::ShaderType::Vertex)
                                      .withSourceFile("shaders/bumpMapping/normal_mapping_frag.glsl", Shader::ShaderType::Fragment)
                                      .build()
                                      ->createMaterial();
        m_materialBlinnBumpMapping = Shader::create()
                                         .withSourceFile("shaders/bumpMapping/blinn_bump_mapping_vert.glsl", Shader::ShaderType::Vertex)
                                         .withSourceFile("shaders/bumpMapping/blinn_bump_mapping_frag.glsl", Shader::ShaderType::Fragment)
                                         .build()
                                         ->createMaterial();
        m_materialParallaxMapping = Shader::create()
                                        .withSourceFile("shaders/bumpMapping/parallax_mapping_vert.glsl", Shader::ShaderType::Vertex)
                                        .withSourceFile("shaders/bumpMapping/parallax_mapping_frag.glsl", Shader::ShaderType::Fragment)
                                        .build()
                                        ->createMaterial();
        m_materialNormalDebug = Shader::create()
                                    .withSourceFile("shaders/debug_normal_vert.glsl", Shader::ShaderType::Vertex)
                                    .withSourceFile("shaders/debug_normal_frag.glsl", Shader::ShaderType::Fragment)
                                    .build()
                                    ->createMaterial();
        m_materialUvDebug = Shader::create()
                                .withSourceFile("shaders/debug_uv_vert.glsl", Shader::ShaderType::Vertex)
                                .withSourceFile("shaders/debug_uv_frag.glsl", Shader::ShaderType::Fragment)
                                .build()
                                ->createMaterial();
        m_materialNormalMapping->setColor({ 1, 1, 1, 1 });
        m_materialNormalMapping->setSpecularity(Color(0, 0, 1, 50));
        auto tex = Texture::create().withFile("resources/blocks/bricks2.jpg").withSamplerColorspace(Texture::SamplerColorspace::Linear).build();
        auto normalTex = Texture::create().withFile("resources/blocks/bricks2_normal.jpg").withSamplerColorspace(Texture::SamplerColorspace::Gamma).build();
        auto depthTex = Texture::create().withFile("resources/blocks/bricks2_disp.jpg").withSamplerColorspace(Texture::SamplerColorspace::Gamma).build();
//        auto tex = Texture::create().withFile("resources/objFiles/spot/spot_texture.png").withSamplerColorspace(Texture::SamplerColorspace::Linear).build();
//        auto normalTex = Texture::create().withFile("resources/objFiles/spot/normal.jpg").withSamplerColorspace(Texture::SamplerColorspace::Gamma).build();
//        auto depthTex = Texture::create().withFile("resources/objFiles/spot/normal.jpg").withSamplerColorspace(Texture::SamplerColorspace::Gamma).build();
        m_materialNormalMapping->set("normalTex", normalTex);
        m_materialNormalMapping->set("tex", tex);
        m_materialNormalMapping->set("normalScale", 2.0f);
        m_materialBlinnBumpMapping->set("tex", tex);
        m_materialParallaxMapping->set("tex", tex);
        m_materialParallaxMapping->set("normalTex", normalTex);
        m_materialParallaxMapping->set("depthTex", depthTex);
    }
    void render() override
    {
        auto rp = RenderPass::create()
                      .withCamera(m_camera)
                      .withWorldLights(m_worldLights.get())
                      .withClearColor(true, { 0, 0, 0, 1 })
                      .build();
        auto size = 0.5f;
        auto scaleRotateMatrix = glm::scale(glm::vec3(size, size, size)) * glm::eulerAngleY(glm::radians(30.0f * m_totalTime));
        rp.draw(m_mesh, glm::translate(glm::vec3(0, 1, 0)) * scaleRotateMatrix, m_materialNormalMapping);
        rp.draw(m_mesh, glm::translate(glm::vec3(2, 1, 0)) * scaleRotateMatrix, m_materialNormalDebug);
        rp.draw(m_mesh, glm::translate(glm::vec3(-2, 1, 0)) * scaleRotateMatrix, m_materialUvDebug);
        rp.draw(m_mesh, glm::translate(glm::vec3(0, -1, 0)) * scaleRotateMatrix, m_materialBlinnBumpMapping);
        rp.draw(m_mesh, glm::translate(glm::vec3(-2, -1, 0)) * scaleRotateMatrix, m_materialParallaxMapping);
    }
    void setTitle() override
    {
        m_title = "bumpMapExample";
    }

private:
    std::shared_ptr<Mesh> m_meshQube;
    std::map<std::string, std::string> m_specialization;
    std::shared_ptr<Material> m_materialNormalMapping;    // 法线贴图材质
    std::shared_ptr<Material> m_materialBlinnBumpMapping; // 凹凸贴图材质
    std::shared_ptr<Material> m_materialParallaxMapping;  // 视差贴图材质
    std::shared_ptr<Material> m_materialNormalDebug;
    std::shared_ptr<Material> m_materialUvDebug;
};

void bumpMapTest()
{
    GLFWRenderer renderer{};
    EngineTestSimple test(renderer);
    auto sceneNodeEffect = std::make_shared<bumpMapExample>(&renderer);
    auto effect = std::make_shared<EffectManager>();
    effect->insertEffect(sceneNodeEffect);
    test.setEffect(effect);
    test.run();
}