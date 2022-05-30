//
// Created by william on 2022/5/24.
//
#include "basicProject.h"
#include "commonMacro.h"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/euler_angles.hpp>

using namespace re;

class ParticleExample : public BasicProject
{
public:
    using BasicProject::BasicProject;
    ~ParticleExample() override = default;
    void run() override
    {
        m_camera = MAKE_UNIQUE(m_camera);
        m_camera->setLookAt({ 0.0f, 0.0f, 3.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
        m_camera->setPerspectiveProjection(60.0f, 0.1f, 100.0f);
        m_shader = Shader::getStandard();
        m_material = std::make_unique<Material>(m_shader);
        m_material->setTexture(Texture::create().withFile(GET_CURRENT("test/resources/test.jpg")).build());
        m_material->setSpecularity(20.0f);
        m_mesh = Mesh::create().withCube().build();
        m_particleShader = Shader::getStandardParticles();
        m_particleMaterial = std::make_unique<Material>(m_particleShader);
        m_particleMaterial->setTexture(Texture::getSphereTexture());
        m_particleMesh = createParticles();
        m_worldLights = std::make_unique<WorldLights>();
        m_worldLights->addLight(Light::create().withPointLight({ 0, 2, 5 }).withColor({ 1, 0, 0 }).withRange(10).build());
        m_worldLights->addLight(Light::create().withPointLight({ 2, 0, 5 }).withColor({ 0, 1, 0 }).withRange(10).build());
        m_worldLights->addLight(Light::create().withPointLight({ 0, -2, 5 }).withColor({ 0, 0, 1 }).withRange(10).build());
        m_worldLights->addLight(Light::create().withPointLight({ -2, 0, 5 }).withColor({ 1, 1, 1 }).withRange(10).build());
        BasicProject::run();
    }
    void render(Renderer* r) override
    {
        auto renderPass = r->createRenderPass().withCamera(*m_camera).withWorldLights(m_worldLights.get()).build();
        renderPass.draw(m_mesh, glm::eulerAngleY(glm::radians(30.0f * m_totalTime)) * glm::scale(glm::mat4(1), { 0.3f, 0.3f, 0.3f }), m_material.get());
        renderPass.draw(m_particleMesh, glm::eulerAngleY(glm::radians(30.0f * m_totalTime)), m_particleMaterial.get());
    }
    void setTitle() override
    {
        m_renderer.setWindowTitle("ParticleExample");
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
    std::unique_ptr<Material> m_particleMaterial;
};

void particleTest()
{
    ParticleExample test;
    test.run();
}