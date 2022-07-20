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
class ParticleEmitter : public std::enable_shared_from_this<ParticleEmitter>
{
public:
    struct Particle
    {
        glm::vec3 position{};     // 位置
        glm::vec3 velocity;       // 速度
        glm::vec3 acceleration{}; // 加速度
        glm::vec4 colorStart{};   // 初始颜色
        glm::vec4 colorEnd{};     // 消亡颜色
        glm::vec4 color{};        // 当前颜色
        float rotation{};         // 旋转
        float angularVelocity{};  // 角速度
        float angle{};            // 当前角度
        float timeOfBirth{};      // 出生时间
        float age{};              // 存活时间【0.0f.0.0f].
        float sizeStart{};        // 初始尺寸
        float sizeEnd{};          // 消亡尺寸
        int index{};              // 粒子索引
        bool alive{};             // 活标记位
    };

    struct ParticleProp
    {
        std::shared_ptr<Texture> texture; // 粒子纹理
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
        glm::vec4 color{};                // 当前颜色
        float rotation{};                 // 旋转
        float rotationVariance{};         // 旋转变化率
        float angularVelocity{};          // 角速度
        float angularVelocityVariance{};  // 角速度变化率
        float timeOfBirth{};              // 出生时间
        float age{};                      // 存活时间【0.0f.0.0f].
        float ageVariance{};              // 存活时间变化率【0.0f.0.0f].
        float sizeStart{};                // 初始尺寸
        float sizeStartVariance{};        // 初始尺寸变化率
        float sizeEnd{};                  // 消亡尺寸
        float sizeEndVariance{};          // 消亡尺寸变化率
        float lifeSpan = 10;              // 粒子生命周期
        uint32_t particleCount{};         // 粒子数目
        uint32_t emissionRate = 60;       // 每秒发射的粒子数
    };

    class ParticleEmitterBuilder
    {
    public:
        ParticleEmitterBuilder();
        ~ParticleEmitterBuilder();
        ParticleEmitterBuilder& withParticleCount(uint32_t particleCount);
        ParticleEmitterBuilder& withEmissionRate(uint32_t emissionRate);
        ParticleEmitterBuilder& withLifeSpan(uint32_t lifeSpan);
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
        std::shared_ptr<ParticleEmitter> build();

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
    ParticleEmitter();
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
     * 获取当前激活的粒子数
     * @return
     */
    int activeParticles();
    /**
     * 获取当前激活粒子数
     * @return
     */
    uint32_t activeCount();

public:
    bool m_started = true;    // 是否还在运行
    bool m_visible = true;    // 是否可见
    bool m_emitting = true;   // 发射状态
    uint32_t m_activeCount{}; // 当前激活粒子数
    uint32_t m_index{};       // 粒子索引

private:
    std::vector<Particle> m_particles;
    std::shared_ptr<Material> m_material;
    std::shared_ptr<Mesh> m_mesh;
    std::vector<glm::vec3> m_positions;
    std::vector<glm::vec4> m_colors;
    std::vector<float> m_sizes;
    std::vector<glm::vec4> m_uvs;
    ParticleProp m_particleProp;
    float m_emissions{};
    float m_totalTime{};
    int m_activeParticles{};
};
} // namespace ceres

#endif // SIMPLERENDERENGINE_PARTICLEEMITTER_H
