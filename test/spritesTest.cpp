//
// Created by william on 2022/5/24.
//
#include "basicProject.h"
#include "commonMacro.h"

#include <glm/glm.hpp>

using namespace re;

namespace
{
std::shared_ptr<Mesh> createParticles()
{
    std::vector<glm::vec3> positions;
    std::vector<glm::vec4> colors;
    std::vector<glm::vec4> uvs;
    std::vector<float> sizes;
    positions.emplace_back(0, 0, 0);
    colors.emplace_back(1, 1, 1, 1);
    sizes.emplace_back(10.0f);
    return Mesh::create()
        .withPositions(positions)
        .withColors(colors)
        .withUvs(uvs)
        .withParticleSizes(sizes)
        .withMeshTopology(Mesh::Topology::Points)
        .build();
}

void updateParticlesAnimation(float time, glm::vec4& pos, float& size, float& rotation)
{
    int frame = ((int)(time * 10)) % 16;
    int frameX = 3 - frame % 4;
    int frameY = frame / 4;
    pos = glm::vec4(frameX * 0.25f, frameY * 0.25f, 0.0f, rotation);
    size = 0.25f;
}

void updateParticles(Mesh* mesh, glm::vec4 uv, float uvSize, float rotation, float size)
{
    std::vector<glm::vec3> positions;
    std::vector<glm::vec4> uvs;
    std::vector<float> sizes;
    positions.emplace_back(0, 0, 0);
    uvs.emplace_back(uv.x, uv.y, uvSize, rotation);
    sizes.push_back(size);
    mesh->update()
        .withPositions(positions)
        .withUvs(uvs)
        .withParticleSizes(sizes)
        .build();
}
} // namespace

class SpritesExample : public BasicProject
{
public:
    using BasicProject::BasicProject;
    ~SpritesExample() = default;

    void initialize() override
    {
        m_camera = MAKE_UNIQUE(m_camera);
        m_camera->setLookAt({ 0.0f, 0.0f, 3.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
        m_camera->setPerspectiveProjection(60, 0.1, 100);
        auto shader = Shader::getStandardParticles();
        m_material = shader->createMaterial();
        m_material->setTexture(Texture::create().withFile(GET_CURRENT("test/resources/sprite.png")).build());
        m_mesh = createParticles();
    }

    void render() override
    {
        auto renderPass = RenderPass::create().withCamera(*m_camera).build();
        updateParticlesAnimation(m_totalTime, spriteUV, m_uvSize, m_uvRotation);
        updateParticles(m_mesh.get(), spriteUV, m_uvSize, m_uvRotation, m_spritesize);
        renderPass.draw(m_mesh, glm::mat4(1), m_material);
        m_profiler.update();
        m_profiler.gui();
    }

    void setTitle() override
    {
        m_title = "SpritesExample";
    }

private:
    glm::vec4 spriteUV{ 0 };
    float m_uvSize{ 1.0 };
    float m_uvRotation{ 1.0f };
    float m_spritesize{ 500.0f };
};

void spriteTest()
{
    SpritesExample test;
    test.run();
}
