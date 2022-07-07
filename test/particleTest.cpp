//
// Created by william on 2022/5/24.
//
#include "engineTestSimple.h"
#include "commonMacro.h"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/euler_angles.hpp>

class ParticleExample : public CommonInterface
{
public:
    using CommonInterface::CommonInterface;
    ~ParticleExample() override = default;
    void initialize() override
    {
        m_camera.setLookAt({ 0.0f, 0.0f, 3.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
        m_camera.setPerspectiveProjection(60.0f, 0.1f, 100.0f);
        auto shader = Shader::getStandardBlinnPhong();
        m_material = shader->createMaterial();
        m_material->setTexture(Texture::create().withFile("resources/test.jpg").build());
        m_material->setSpecularity({0.1f, 0.1f, 0.1f, 1.0f});
        m_mesh = Mesh::create().withCube().build();
        auto particleShader = Shader::getStandardParticles();
        m_particleMaterial = particleShader->createMaterial();
        m_particleMaterial->setTexture(Texture::getSphereTexture());
        m_particleMesh = createParticles();
        m_worldLights = std::make_unique<WorldLights>();
        m_worldLights->addLight(Light::create().withPointLight({ 0, 2, 5 }).withColor({ 1, 0, 0 }).withRange(10).build());
        m_worldLights->addLight(Light::create().withPointLight({ 2, 0, 5 }).withColor({ 0, 1, 0 }).withRange(10).build());
        m_worldLights->addLight(Light::create().withPointLight({ 0, -2, 5 }).withColor({ 0, 0, 1 }).withRange(10).build());
        m_worldLights->addLight(Light::create().withPointLight({ -2, 0, 5 }).withColor({ 1, 1, 1 }).withRange(10).build());
    }
    void render() override
    {
        auto renderPass = RenderPass::create().withCamera(m_camera).withClearColor(true, {0.33, 0.33, 0.33, 0.33}).withWorldLights(m_worldLights.get()).build();
        renderPass.draw(m_mesh, glm::eulerAngleY(glm::radians(30.0f * m_totalTime)) * glm::scale(glm::mat4(1), { 0.3f, 0.3f, 0.3f }), m_material);
        renderPass.draw(m_particleMesh, glm::eulerAngleY(glm::radians(30.0f * m_totalTime)), m_particleMaterial);
    }

    std::shared_ptr<Mesh> createParticles(int size = 2500)
    {
        std::vector<glm::vec3> positions;
        std::vector<glm::vec4> colors;
        std::vector<float> sizes;
        std::vector<glm::vec4> uvs;
        for (int i = 0; i < size; i++)
        {
            positions.push_back(glm::linearRand(glm::vec3(-1, -1, -1), glm::vec3(1, 1, 1)));
            colors.push_back(glm::linearRand(glm::vec4(0, 0, 0, 0), glm::vec4(1, 1, 1, 1)));
            sizes.push_back(glm::linearRand(0.0f, 50.0f));
            uvs.emplace_back(0, 0, 1, 1);
        }
        return Mesh::create()
            .withPositions(positions)
            .withColors(colors)
            .withParticleSizes(sizes)
            .withUvs(uvs)
            .withMeshTopology(Mesh::Topology::Points)
            .build();
    }

private:
    std::shared_ptr<Mesh> m_particleMesh;
    std::shared_ptr<Shader> m_particleShader;
    std::shared_ptr<Material> m_particleMaterial;
};

void particleTest()
{
    GLFWRenderer renderer{};
    EngineTestSimple test(renderer);
    auto sceneNodeEffect = std::make_shared<ParticleExample>(&renderer);
    auto effect = std::make_shared<EffectManager>();
    effect->insertEffect(sceneNodeEffect);
    test.setEffect(effect);
    test.setTitle("ParticleExample");
    test.run();
}