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
        float size{};             // 当前尺寸
        int index{};              // 粒子索引
        bool alive{};             // 活标记位
    };

    struct ParticleProp
    {
        std::shared_ptr<Material> material; // 粒子材质
        glm::vec3 position{};               // 位置
        glm::vec3 positionVariance{};       // 位置变化率（变化率即振幅）
        glm::vec3 velocity;                 // 速度
        glm::vec3 velocityVariance{};       // 速度变化率
        glm::vec3 acceleration{};           // 加速度
        glm::vec3 accelerationVariance{};   // 加速度变化率
        glm::vec4 colorStart{};             // 初始颜色
        glm::vec4 colorStartVariance{};     // 初始颜色变化率
        glm::vec4 colorEnd{};               // 消亡颜色
        glm::vec4 colorEndVariance{};       // 消亡颜色变化率
        glm::vec4 color{};                  // 当前颜色
        float rotation{};                   // 旋转
        float rotationVariance{};           // 旋转变化率
        float angularVelocity{};            // 角速度
        float angularVelocityVariance{};    // 角速度变化率
        float timeOfBirth{};                // 出生时间
        float age{};                        // 存活时间【0.0f.0.0f].
        float ageVariance{};                // 存活时间变化率【0.0f.0.0f].
        float sizeStart{};                  // 初始尺寸
        float sizeStartVariance{};          // 初始尺寸变化率
        float sizeEnd{};                    // 消亡尺寸
        float sizeEndVariance{};            // 消亡尺寸变化率
        float lifeSpan{};                   // 粒子生命周期
        uint32_t particleCount{};           // 粒子数目
        uint32_t emissionRate{};            // 每秒发射的粒子数
        bool started{ true };               // 是否还在运行
        bool visible{ true };               // 是否可见
        bool emitting{ true };              // 发射状态
    };

    class ParticleEmitterBuilder
    {
    public:
        ParticleEmitterBuilder();
        ~ParticleEmitterBuilder();
        /**
         * 粒子数
         * @param particleCount
         * @return
         */
        ParticleEmitterBuilder& withParticleCount(uint32_t particleCount);
        /**
         * 每秒发射粒子数
         * @param emissionRate
         * @return
         */
        ParticleEmitterBuilder& withEmissionRate(uint32_t emissionRate);
        /**
         * 生命周期
         * @param lifeSpan
         * @return
         */
        ParticleEmitterBuilder& withLifeSpan(uint32_t lifeSpan);
        /**
         * 粒子尺寸
         * @param startSize
         * @param startSizeVariance
         * @param endSize
         * @param endSizeVariance
         * @return
         */
        ParticleEmitterBuilder& withSize(float startSize, float startSizeVariance, float endSize, float endSizeVariance);
        /**
         * 粒子颜色
         * @param startColor
         * @param startColorVariance
         * @param endColor
         * @param endColorVariance
         * @return
         */
        ParticleEmitterBuilder& withColor(const glm::vec4& startColor, const glm::vec4& startColorVariance, const glm::vec4& endColor, const glm::vec4& endColorVariance);
        /**
         * 粒子位置
         * @param position
         * @param positionVariance
         * @return
         */
        ParticleEmitterBuilder& withPosition(const glm::vec3& position, const glm::vec3& positionVariance);
        /**
         * 粒子加速度
         * @param acceleration
         * @param accelerationVariance
         * @return
         */
        ParticleEmitterBuilder& withAcceleration(const glm::vec3& acceleration, const glm::vec3& accelerationVariance);
        /**
         * 粒子速度
         * @param velocity
         * @param velocityVariance
         * @return
         */
        ParticleEmitterBuilder& withVelocity(const glm::vec3& velocity, const glm::vec3& velocityVariance);
        /**
         * 粒子材质
         * @param material
         * @return
         */
        ParticleEmitterBuilder& withMaterial(const std::shared_ptr<Material>& material);
        /**
         * 粒子旋转角
         * @param rotation
         * @param rotationVariance
         * @return
         */
        ParticleEmitterBuilder& withRotation(float rotation, float rotationVariance);
        /**
         * 粒子角速度
         * @param angularVelocity
         * @param angularVelocityVariance
         * @return
         */
        ParticleEmitterBuilder& withAngularVelocity(float angularVelocity, float angularVelocityVariance);
        /**
         * 是否运行
         * @param running
         * @return
         */
        ParticleEmitterBuilder& withRunning(bool running);

        /**
         * 是否可见
         * @param visible
         * @return
         */
        ParticleEmitterBuilder& withVisible(bool visible);
        /**
         * 发射状态
         * @param emitting
         * @return
         */
        ParticleEmitterBuilder& withEmitting(bool emitting);

        std::shared_ptr<ParticleEmitter> build();

    private:
        ParticleProp m_particleProp;
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
    ParticleEmitter(ParticleEmitterBuilder& build);
    /**
     * constructor
     */
    ParticleEmitter() = delete;
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
    /**
     * 获取材质
     * @return
     */
    const std::shared_ptr<Material>& material() const;

private:
    template <class T>
    T generateValue(const T& base, const T& variance);

public:
    uint32_t m_activeCount{}; // 当前激活粒子数
    uint32_t m_index{};       // 粒子索引

private:
    std::vector<Particle> m_particles;
    std::shared_ptr<Mesh> m_mesh;
    std::vector<glm::vec3> m_positions;
    std::vector<glm::vec4> m_colors;
    std::vector<float> m_sizes;
    std::vector<glm::vec4> m_uvs;
    ParticleProp m_particleProp;
    float m_emissions{}; // 发射的粒子数
    float m_totalTime{};
    int m_activeParticles{};
};
} // namespace ceres

#endif // SIMPLERENDERENGINE_PARTICLEEMITTER_H
