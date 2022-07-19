//
// Created by cwb on 2022/7/19.
//

#include "particleEmitter.h"

#include "shader.h"

#include <glm/gtc/random.hpp>
namespace ceres
{

ParticleEmitter::ParticleEmitter(int particleCount, std::shared_ptr<Texture>& texture)
{
    m_particles.resize(particleCount);
    m_material = Shader::getStandardParticles()->createMaterial();
    m_material->setTexture(texture);
    m_positions.resize(particleCount, { 0.0f, 0.0f, 0.0f });
    m_colors.resize(particleCount, { 1.0f, 1.0f, 1.0f, 1.0f });
    m_sizes.resize(particleCount, { 100.0f });
    m_uvs.resize(particleCount, { 0.0f, 0.0f, 1.0f, 1.0f });
    m_velocity = glm::sphericalRand(1.0f);
    m_velocityVar = glm::sphericalRand(1.0f);
    m_colorStart = { 1.0f, 1.0f, 1.0f, 1.0f };
    m_colorStartVar = m_colorStart;
    m_colorEnd = { 1.0f, 1.0f, 1.0f, 1.0f };
    m_colorEndVar = m_colorEnd;
    // preallocate arrays for mesh data
    for (int i = 0; i < particleCount; i++)
    {
        m_positions[i] = glm::vec3(i * 0.1f, i * 0.1f, i * 0.1f);
        m_particles[i].index = i;
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
        m_emissions += deltaTime * m_emissionRate;
        int newEmissions = (int)m_emissions;
        for (int i = oldEmissions; i < newEmissions; i++)
        {
            emit();
        }
    }
    if (m_started)
    {
        m_totalTime += deltaTime;
        m_activeParticles = 0;
        for (auto& p : m_particles)
        {
            // v = at
            p.velocity += deltaTime * gravity;
            // deltaS = at
            p.position += p.velocity * deltaTime;
            // Simple linear interpolation of color and size.
            float percent = 1.0f - ((float)p.energy / (float)p.energyStart);
            p.color.r = p.colorStart.r + (p.colorEnd.r - p.colorStart.r) * percent;
            p.color.g = p.colorStart.g + (p.colorEnd.g - p.colorStart.g) * percent;
            p.color.b = p.colorStart.b + (p.colorEnd.b - p.colorStart.b) * percent;
            p.color.a = p.colorStart.a + (p.colorEnd.a - p.colorStart.a) * percent;
            p.size = p.sizeStart + (p.sizeEnd - p.sizeStart) * percent;
            p.rotation += p.angularVelocity * deltaTime;
            p.alive = p.timeOfBirth + m_lifeSpan > m_totalTime;
            if (p.alive)
                m_activeParticles++;
            p.age = (m_totalTime - p.timeOfBirth) / m_lifeSpan;
        }
    }
}

void ParticleEmitter::draw(RenderPass& renderPass, glm::mat4 transform)
{
    if (!m_visible) return;
    for (int i = 0; i < m_particles.size(); i++)
    {
        auto& p = m_particles[i];
        m_sizes[i] = p.alive ? p.size : 0;
        m_positions[i] = p.position;
        m_colors[i] = p.color.toLinear();
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

void ParticleEmitter::emit()
{
    auto& p = m_particles[m_particleIndex];
    p.alive = true;
    p.timeOfBirth = m_totalTime;
    p.age = 0;
    p.position = m_position;
    p.acceleration = m_acceleration;
    p.velocity = m_velocity;
    p.rotation = m_rotationMin;
    p.angularVelocity = m_angularVelocityMin;
    m_particleIndex = (m_particleIndex + 1) % m_particles.size();
}

void ParticleEmitter::setPosition(const glm::vec3& position, const glm::vec3& positionVariance)
{
    m_position = position;
    m_positionVar = positionVariance;
}

void ParticleEmitter::setAcceleration(const glm::vec3& acceleration, const glm::vec3& accelerationVariance)
{
    m_acceleration = acceleration;
    m_accelerationVar = accelerationVariance;
}

void ParticleEmitter::setVelocity(const glm::vec3& velocity, const glm::vec3& velocityVariance)
{
    m_velocity = velocity;
    m_velocityVar = velocityVariance;
}

void ParticleEmitter::setAngularVelocity(float angularVelocityMin, float angularVelocityMax)
{
    m_angularVelocityMin = angularVelocityMin;
    m_angularVelocityMax = angularVelocityMax;
}

void ParticleEmitter::setRotation(float rotationMin, float rotationMax)
{
    m_rotationMin = rotationMax;
    m_rotationMax = rotationMax;
}

void ParticleEmitter::setColor(const glm::vec4& startMin, const glm::vec4& startMax, const glm::vec4& endMin, const glm::vec4& endMax)
{
    m_colorStart = startMin;
    m_colorStartVar = startMax;
    m_colorEnd = endMin;
    m_colorEndVar = endMax;
}

void ParticleEmitter::setTexture(const std::shared_ptr<Texture>& texture)
{
    m_material->setTexture(texture);
}

void ParticleEmitter::setMaterial(const std::shared_ptr<Material>& material)
{
    m_material = material;
}

void ParticleEmitter::setSize(float startMin, float startMax, float endMin, float endMax)
{
    m_sizeStartMin = startMin;
    m_sizeStartMax = startMax;
    m_sizeEndMin = endMin;
    m_sizeEndMax = endMax;
}

int ParticleEmitter::activeParticles()
{
    return m_activeParticles;
}
} // namespace ceres