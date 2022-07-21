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
    return std::make_shared<ParticleEmitter>(ParticleEmitter(*this));
}

ParticleEmitter::ParticleEmitterBuilder& ParticleEmitter::ParticleEmitterBuilder::withParticleCount(uint32_t particleCount)
{
    if (m_emitter)
    {
        m_emitter->m_particleProp.particleCount = particleCount;
    }
    else
    {
        m_particleProp.particleCount = particleCount;
    }
    return *this;
}

ParticleEmitter::ParticleEmitterBuilder& ParticleEmitter::ParticleEmitterBuilder::withSize(float startSize, float startSizeVariance, float endSize, float endSizeVariance)
{
    if (m_emitter)
    {
        m_emitter->m_particleProp.sizeStart = startSize;
        m_emitter->m_particleProp.sizeStartVariance = startSizeVariance;
        m_emitter->m_particleProp.sizeEnd = endSize;
        m_emitter->m_particleProp.sizeEndVariance = endSizeVariance;
    }
    else
    {
        m_particleProp.sizeStart = startSize;
        m_particleProp.sizeStartVariance = startSizeVariance;
        m_particleProp.sizeEnd = endSize;
        m_particleProp.sizeEndVariance = endSizeVariance;
    }
    return *this;
}

ParticleEmitter::ParticleEmitterBuilder& ParticleEmitter::ParticleEmitterBuilder::withColor(const glm::vec4& startColor, const glm::vec4& startColorVariance, const glm::vec4& endColor, const glm::vec4& endColorVariance)
{
    if (m_emitter)
    {
        m_emitter->m_particleProp.colorStart = startColor;
        m_emitter->m_particleProp.colorStartVariance = startColorVariance;
        m_emitter->m_particleProp.colorEnd = endColor;
        m_emitter->m_particleProp.colorEndVariance = endColorVariance;
    }
    else
    {
        m_particleProp.colorStart = startColor;
        m_particleProp.colorStartVariance = startColorVariance;
        m_particleProp.colorEnd = endColor;
        m_particleProp.colorEndVariance = endColorVariance;
    }
    return *this;
}

ParticleEmitter::ParticleEmitterBuilder& ParticleEmitter::ParticleEmitterBuilder::withPosition(const glm::vec3& position, const glm::vec3& positionVariance)
{
    if (m_emitter)
    {
        m_emitter->m_particleProp.position = position;
        m_emitter->m_particleProp.positionVariance = positionVariance;
    }
    else
    {
        m_particleProp.position = position;
        m_particleProp.positionVariance = positionVariance;
    }
    return *this;
}

ParticleEmitter::ParticleEmitterBuilder& ParticleEmitter::ParticleEmitterBuilder::withAcceleration(const glm::vec3& acceleration, const glm::vec3& accelerationVariance)
{
    if (m_emitter)
    {
        m_emitter->m_particleProp.acceleration = acceleration;
        m_emitter->m_particleProp.accelerationVariance = accelerationVariance;
    }
    else
    {
        m_particleProp.acceleration = acceleration;
        m_particleProp.accelerationVariance = accelerationVariance;
    }
    return *this;
}

ParticleEmitter::ParticleEmitterBuilder& ParticleEmitter::ParticleEmitterBuilder::withVelocity(const glm::vec3& velocity, const glm::vec3& velocityVariance)
{
    if (m_emitter)
    {
        m_emitter->m_particleProp.velocity = velocity;
        m_emitter->m_particleProp.velocityVariance = velocityVariance;
    }
    else
    {
        m_particleProp.velocity = velocity;
        m_particleProp.velocityVariance = velocityVariance;
    }
    return *this;
}

ParticleEmitter::ParticleEmitterBuilder& ParticleEmitter::ParticleEmitterBuilder::withMaterial(const std::shared_ptr<Material>& material)
{
    if (m_emitter)
    {
        m_emitter->m_particleProp.material = material;
    }
    else
    {
        m_particleProp.material = material;
    }
    return *this;
}

ParticleEmitter::ParticleEmitterBuilder& ParticleEmitter::ParticleEmitterBuilder::withRotation(float rotation, float rotationVariance)
{
    if (m_emitter)
    {
        m_emitter->m_particleProp.rotation = rotation;
        m_emitter->m_particleProp.rotationVariance = rotationVariance;
    }
    else
    {
        m_particleProp.rotation = rotation;
        m_particleProp.rotationVariance = rotationVariance;
    }
    return *this;
}

ParticleEmitter::ParticleEmitterBuilder& ParticleEmitter::ParticleEmitterBuilder::withAngularVelocity(float angularVelocity, float angularVelocityVariance)
{
    if (m_emitter)
    {
        m_emitter->m_particleProp.angularVelocity = angularVelocity;
        m_emitter->m_particleProp.angularVelocityVariance = angularVelocityVariance;
    }
    else
    {
        m_particleProp.angularVelocity = angularVelocity;
        m_particleProp.angularVelocityVariance = angularVelocityVariance;
    }

    return *this;
}

ParticleEmitter::ParticleEmitterBuilder& ParticleEmitter::ParticleEmitterBuilder::withEmissionRate(uint32_t emissionRate)
{
    if (m_emitter)
    {
        m_emitter->m_particleProp.emissionRate = emissionRate;
    }
    else
    {
        m_particleProp.emissionRate = emissionRate;
    }
    return *this;
}

ParticleEmitter::ParticleEmitterBuilder& ParticleEmitter::ParticleEmitterBuilder::withLifeSpan(uint32_t lifeSpan)
{
    if (m_emitter)
    {
        m_emitter->m_particleProp.lifeSpan = lifeSpan;
    }
    else
    {
        m_particleProp.lifeSpan = lifeSpan;
    }
    return *this;
}


ParticleEmitter::ParticleEmitterBuilder& ParticleEmitter::ParticleEmitterBuilder::withRunning(bool running)
{
    if (m_emitter)
    {
        m_emitter->m_particleProp.started = running;
    }
    else
    {
        m_particleProp.started = running;
    }
    return *this;
}

ParticleEmitter::ParticleEmitterBuilder& ParticleEmitter::ParticleEmitterBuilder::withVisible(bool visible)
{
    if (m_emitter)
    {
        m_emitter->m_particleProp.visible = visible;
    }
    else
    {
        m_particleProp.visible = visible;
    }
    return *this;
}

ParticleEmitter::ParticleEmitterBuilder&  ParticleEmitter::ParticleEmitterBuilder::withEmitting(bool emitting)
{
    if (m_emitter)
    {
        m_emitter->m_particleProp.emitting = emitting;
    }
    else
    {
        m_particleProp.emitting = emitting;
    }
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

ParticleEmitter::ParticleEmitter(ParticleEmitter::ParticleEmitterBuilder& build)
{
    m_particleProp = build.m_particleProp;
    auto particleCount = m_particleProp.particleCount;
    m_particles.resize(particleCount);
    m_particleProp.material = m_particleProp.material;
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
    if (m_particleProp.emitting && m_particleProp.visible)
    {
        int oldEmissions = (int)m_emissions;
        m_emissions += deltaTime * m_particleProp.emissionRate;
        int newEmissions = (int)m_emissions;
        for (int i = oldEmissions; i < newEmissions; ++i)
        {
            emitOnce();
        }
    }
    if (m_particleProp.started)
    {
        m_totalTime += deltaTime;
        m_activeParticles = 0;
        for (auto& p : m_particles)
        {
            // v = at
            p.velocity += deltaTime * p.acceleration;
            // deltaS = vt
            p.position += p.velocity * deltaTime;
            p.color = glm::mix(m_particleProp.colorStart, m_particleProp.colorEnd, p.age);
            p.size = glm::mix(m_particleProp.sizeStart, m_particleProp.sizeEnd, p.age);
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
    if (!m_particleProp.visible) return;
    for (int i = 0; i < m_particles.size(); i++)
    {
        auto& p = m_particles[i];
        m_sizes[i] = p.alive ? p.size : 0;
        m_positions[i] = p.position;
        m_colors[i] = p.color;
        m_uvs[i].w = p.rotation;
    }
    m_mesh->update()
        .withPositions(m_positions)
        .withColors(m_colors)
        .withParticleSizes(m_sizes)
        .withUvs(m_uvs)
        .build();
    renderPass.draw(m_mesh, transform, m_particleProp.material);
}

void ParticleEmitter::emitOnce()
{
    auto& p = m_particles[m_index];
    p.alive = true;
    p.timeOfBirth = m_totalTime;
    p.age = 0;
    p.position = generateValue(m_particleProp.position, m_particleProp.positionVariance);
    p.colorStart = generateValue(m_particleProp.colorStart, m_particleProp.colorStartVariance);
    p.colorEnd = generateValue(m_particleProp.colorEnd, m_particleProp.colorEndVariance);
    p.sizeStart = generateValue(m_particleProp.sizeStart, m_particleProp.sizeStartVariance);
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

const std::shared_ptr<Material>& ParticleEmitter::material() const
{
    return m_particleProp.material;
}

template <class T>
T ParticleEmitter::generateValue(const T& base, const T& variance)
{
    return base + (variance - base) * MATH_RANDOM_0_1();
}

} // namespace ceres