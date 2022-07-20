//
// Created by cwb on 2022/7/19.
//

#include "particleEmitter.h"

#include "shader.h"

#include <glm/gtc/random.hpp>
namespace ceres
{
ParticleEmitter::ParticleEmitterBuilder::ParticleEmitterBuilder() = default;
ParticleEmitter::ParticleEmitterBuilder::~ParticleEmitterBuilder() = default;

std::shared_ptr<ParticleEmitter> ParticleEmitter::ParticleEmitterBuilder::build()
{
    if (m_emitter != nullptr)
    {
        return m_emitter->shared_from_this();
    }
    return {};
}

ParticleEmitter::ParticleEmitterBuilder& ParticleEmitter::ParticleEmitterBuilder::withParticleCount(uint32_t particleCount)
{
    return *this;
}

ParticleEmitter::ParticleEmitterBuilder& ParticleEmitter::ParticleEmitterBuilder::withSize(float startSize, float startSizeVariance, float endSize, float endSizeVariance)
{
    return *this;
}

ParticleEmitter::ParticleEmitterBuilder& ParticleEmitter::ParticleEmitterBuilder::withColor(const glm::vec4& startColor, const glm::vec4& startColorVariance, const glm::vec4& endColor, const glm::vec4& endColorVariance)
{
    return *this;
}

ParticleEmitter::ParticleEmitterBuilder& ParticleEmitter::ParticleEmitterBuilder::withPosition(const glm::vec3& position, const glm::vec3& positionVariance)
{
    return *this;
}

ParticleEmitter::ParticleEmitterBuilder& ParticleEmitter::ParticleEmitterBuilder::withAcceleration(const glm::vec3& acceleration, const glm::vec3& accelerationVariance)
{
    return *this;
}

ParticleEmitter::ParticleEmitterBuilder& ParticleEmitter::ParticleEmitterBuilder::withVelocity(const glm::vec3& velocity, const glm::vec3& velocityVariance)
{
    return *this;
}

ParticleEmitter::ParticleEmitterBuilder& ParticleEmitter::ParticleEmitterBuilder::withTexture(const std::shared_ptr<Texture>& texture)
{
    return *this;
}

ParticleEmitter::ParticleEmitterBuilder& ParticleEmitter::ParticleEmitterBuilder::withMaterial(const std::shared_ptr<Material>& material)
{
    return *this;
}

ParticleEmitter::ParticleEmitterBuilder& ParticleEmitter::ParticleEmitterBuilder::withRotation(float rotation, float rotationVariance)
{
    return *this;
}

ParticleEmitter::ParticleEmitterBuilder& ParticleEmitter::ParticleEmitterBuilder::withAngularVelocity(float angularVelocity, float angularVelocityVariance)
{
    return *this;
}

ParticleEmitter::ParticleEmitterBuilder& ParticleEmitter::ParticleEmitterBuilder::withEmissionRate(uint32_t emissionRate)
{
    return *this;
}

ParticleEmitter::ParticleEmitterBuilder& ParticleEmitter::ParticleEmitterBuilder::withLifeSpan(uint32_t lifeSpan)
{
    return *this;
}

ParticleEmitter::ParticleEmitterBuilder ParticleEmitter::create()
{
    return {};
}

ParticleEmitter::ParticleEmitterBuilder ParticleEmitter::update()
{
    ParticleEmitter::ParticleEmitterBuilder builder;
    builder.m_emitter = this;
    return builder;
}

ParticleEmitter::ParticleEmitter()
{
    auto particleCount = m_particleProp.particleCount;
    m_particles.resize(particleCount);
    m_material = Shader::getStandardParticles()->createMaterial();
    m_material->setTexture(m_particleProp.texture);
    m_positions.resize(particleCount, { 0.0f, 0.0f, 0.0f });
    m_colors.resize(particleCount, { 1.0f, 1.0f, 1.0f, 1.0f });
    m_sizes.resize(particleCount, { 100.0f });
    m_uvs.resize(particleCount, { 0.0f, 0.0f, 1.0f, 1.0f });
    // preallocate arrays for mesh data
    for (int i = 0; i < particleCount; i++)
    {
        m_positions[i] = glm::vec3(i * 0.1f, i * 0.1f, i * 0.1f);
        m_particles[i].index = i;
        m_particles[i].velocity = glm::sphericalRand(1.0f);
    }
    m_mesh = Mesh::create()
                 .withPositions(m_positions)
                 .withColors(m_colors)
                 .withParticleSizes(m_sizes)
                 .withUvs(m_uvs)
                 .withMeshTopology(Mesh::Topology::Points)
                 .build();
}

ParticleEmitter::~ParticleEmitter() = default;

void ParticleEmitter::update(float deltaTime)
{
    if (m_emitting && m_started)
    {
        int oldEmissions = (int)m_emissions;
        m_emissions += deltaTime * m_particleProp.emissionRate;
        int newEmissions = (int)m_emissions;
        for (int i = oldEmissions; i < newEmissions; i++)
        {
            emitOnce();
        }
    }
    if (m_started)
    {
        m_totalTime += deltaTime;
        m_activeParticles = 0;
        for (auto& p : m_particles)
        {
            // v = at
            p.velocity += deltaTime * p.acceleration;
            // deltaS = vt
            p.position += p.velocity * deltaTime;
            p.colorStart = glm::mix(m_particleProp.colorStart, m_particleProp.colorEnd, p.age);
            p.sizeStart = glm::mix(m_particleProp.sizeStart, m_particleProp.sizeEnd, p.age);
            p.rotation += p.angularVelocity * deltaTime;
            p.alive = p.timeOfBirth + m_particleProp.lifeSpan > m_totalTime;
            if (p.alive)
                m_activeParticles++;
            p.age = (m_totalTime - p.timeOfBirth) / m_particleProp.lifeSpan;
        }
    }
}

void ParticleEmitter::draw(RenderPass& renderPass, glm::mat4 transform)
{
    if (!m_visible) return;
    for (int i = 0; i < m_particles.size(); i++)
    {
        auto& p = m_particles[i];
        m_sizes[i] = p.alive ? p.sizeStart : 0;
        m_positions[i] = p.position;
        m_colors[i] = p.colorStart;
        m_uvs[i].w = p.rotation;
    }
    m_mesh->update()
        .withPositions(m_positions)
        .withColors(m_colors)
        .withParticleSizes(m_sizes)
        .withUvs(m_uvs)
        .build();
    renderPass.draw(m_mesh, transform, m_material);
}

void ParticleEmitter::emitOnce()
{
    auto& p = m_particles[m_index];
    //    p.alive = true;
    //    p.timeOfBirth = m_totalTime;
    //    p.age = 0;
    //    p.position = glm::vec3(m_particleIndex * 0.1f, m_particleIndex * 0.1f, m_particleIndex * 0.1f);
    p.acceleration = m_particleProp.acceleration;
    p.velocity = glm::sphericalRand(1.0f);
    p.rotation = m_particleProp.rotation;
    p.angularVelocity = m_particleProp.angularVelocity;
    m_index = (m_index + 1) % m_particles.size();
}

int ParticleEmitter::activeParticles()
{
    return m_activeParticles;
}

uint32_t ParticleEmitter::activeCount()
{
    return m_activeCount;
}
} // namespace ceres