//
// Created by cwb on 20.0f22/7/19.
//

#ifndef SIMPLERENDERENGINE_PARTICLEEMITTER_H
#define SIMPLERENDERENGINE_PARTICLEEMITTER_H
#include "color.h"
#include "renderPass.h"

#include <glm/glm.hpp>
#include <vector>
namespace ceres
{
class ParticleEmitter
{
public:
    struct Particle
    {
        glm::vec3 position{};             // 位置
        glm::vec3 positionVariance{};     // 位置变化率（变化率即振幅）
        glm::vec3 velocity;               // 速度
        glm::vec3 velocityVariance{};     // 速度变化率
        glm::vec3 acceleration{};         // 加速度
        glm::vec3 accelerationVariance{}; // 加速度变化率
        glm::vec4 colorStart{};           // 初始颜色
        glm::vec4 colorStartVariance{};   // 初始颜色变化率
        glm::vec4 colorEnd{};             // 消亡颜色
        glm::vec4 colorEndVariance{};     // 消亡颜色变化率
        float rotation{};                 // 旋转
        float rotationVariance{};         // 旋转变化率
        float angularVelocity{};          // 角速度
        float angularVelocityVariance{};  // 角速度变化率
        float angle{};                    // 当前角度
        float timeOfBirth{};              // 出生时间
        float age{};                      // 存活时间【0.0f.0.0f].
        float sizeStart{};                // 初始尺寸
        float sizeStartVariance{};        // 初始尺寸变化率
        float sizeEnd{};                  // 消亡尺寸
        float sizeEndVariance{};          // 消亡尺寸变化率
        int index{};                        // 粒子索引
        bool alive{};                     // 活标记位
    };

    class ParticleEmitterBuilder
    {
    public:
        ParticleEmitterBuilder();
        ~ParticleEmitterBuilder();
        ParticleEmitterBuilder& withSize(float startSize, float startSizeVariance, float endSize, float endSizeVariance);
        ParticleEmitterBuilder& withColor(const glm::vec4& startColor, const glm::vec4& startColorVariance, const glm::vec4& endColor, const glm::vec4& endColorVariance);
        ParticleEmitterBuilder& withPosition(const glm::vec3& position, const glm::vec3& positionVariance);
        ParticleEmitterBuilder& withAcceleration(const glm::vec3& acceleration, const glm::vec3& accelerationVariance);
        ParticleEmitterBuilder& withVelocity(const glm::vec3& velocity, const glm::vec3& velocityVariance);
        ParticleEmitterBuilder& withTexture(const std::shared_ptr<Texture>& texture);
        ParticleEmitterBuilder& withMaterial(const std::shared_ptr<Material>& material);
        ParticleEmitterBuilder& withRotation(float rotation, float rotationVariance);
        ParticleEmitterBuilder& withAngularVelocity(float angularVelocity, float angularVelocityVariance);
        ParticleEmitterBuilder& withAge(float age, float ageVariance);
        ParticleEmitter& build();

    private:
        Particle m_particleProp;
        ParticleEmitter* m_emitter{};
        friend ParticleEmitter;
    };

public:
    /**
     * 创建ParticleEmitterBuilder
     * @return
     */
    static ParticleEmitterBuilder create();

    /**
     * 更新ParticleEmitterBuilder
     * @return
     */
    ParticleEmitterBuilder update();

    /**
     * constructor
     */
    ParticleEmitter(int particleCount, std::shared_ptr<Texture>& texture);
    /**
     * destructor
     */
    ~ParticleEmitter();
    /**
     * 更新粒子状态
     * @param deltaTime
     */
    void update(float deltaTime);

    /**
     * 绘制粒子
     * @param renderPass：renderPass
     * @param transform：模型矩阵
     */
    void draw(RenderPass& renderPass, glm::mat4 transform = glm::mat4(1));
    /**
     * 发射粒子
     */
    void emitOnce();
    /**
     * 设置粒子位置
     * @param position
     * @param positionVariance
     */
    void setPosition(const glm::vec3& position, const glm::vec3& positionVariance);
    /**
     * 设置粒子加速度
     * @param acceleration
     * @param accelerationVariance
     */
    void setAcceleration(const glm::vec3& acceleration, const glm::vec3& accelerationVariance);
    /**
     * 设置粒子初速度
     * @param velocity
     * @param velocityVariance
     */
    void setVelocity(const glm::vec3& velocity, const glm::vec3& velocityVariance);
    /**
     * 设置角速度
     * @param angularVelocityMin
     * @param angularVelocityMax
     */
    void setAngularVelocity(float angularVelocityMin, float angularVelocityMax);
    /**
     * 设置旋转角
     * @param rotationMin
     * @param rotationMax
     */
    void setRotation(float rotationMin, float rotationMax);
    /**
     * 设置粒子颜色
     * @param starMin
     * @param startMax
     * @param endMin
     * @param endMax
     */
    void setColor(const glm::vec4& startMin, const glm::vec4& startMax, const glm::vec4& endMin, const glm::vec4& endMax);
    /**
     * 设置texture
     * @param texture
     */
    void setTexture(const std::shared_ptr<Texture>& texture);
    /**
     * 设置texture
     * @param texture
     */
    void setMaterial(const std::shared_ptr<Material>& material);
    /**
     * 设置粒子尺寸
     * @param startMin
     * @param startMax
     * @param endMin
     * @param endMax
     */
    void setSize(float startMin, float startMax, float endMin, float endMax);
    /**
     * 获取当前激活的粒子数
     * @return
     */
    int activeParticles();

public:
    int32_t m_emissionRate = 60; // 每秒发射的粒子数
    float m_lifeSpan = 10;       // lifetime for each particle
    bool m_started = true;       // 是否还在运行
    bool m_visible = true;       // 是否可见
    bool m_emitting = true;      // 发射状态

private:
    std::vector<Particle> m_particles;
    std::shared_ptr<Material> m_material;
    std::shared_ptr<Mesh> m_mesh;
    std::vector<glm::vec3> m_positions;
    std::vector<glm::vec4> m_colors;
    std::vector<float> m_sizes;
    std::vector<glm::vec4> m_uvs;
    glm::vec4 m_colorStart{};
    glm::vec4 m_colorStartVar{};
    glm::vec4 m_colorEnd{};
    glm::vec4 m_colorEndVar{};
    glm::vec3 m_position{};
    glm::vec3 m_positionVar{};
    glm::vec3 m_acceleration{};
    glm::vec3 m_accelerationVar{};
    glm::vec3 m_velocity{};
    glm::vec3 m_velocityVar{};
    float m_sizeStartMin{};
    float m_sizeStartMax{};
    float m_sizeEndMin{};
    float m_sizeEndMax{};
    float m_rotationMin{};
    float m_rotationMax{};
    float m_angularVelocityMin{};
    float m_angularVelocityMax{};
    float m_emissions{};
    float m_totalTime{};
    int m_particleIndex{};
    int m_activeParticles{};
};
} // namespace ceres

#endif // SIMPLERENDERENGINE_PARTICLEEMITTER_H
