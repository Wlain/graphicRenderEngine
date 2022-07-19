//
// Created by william on 2022/6/1.
//

#ifndef SIMPLERENDERENGINE_SPRITE_H
#define SIMPLERENDERENGINE_SPRITE_H
#include "commonMacro.h"

#include <array>
#include <glm/glm.hpp>
#include <string>
/// 精灵图集
namespace ceres
{
class SpriteAtlas;
class Texture;
class Sprite
{
public:
    Sprite();
    virtual ~Sprite();
    Sprite(const Sprite&);
    [[nodiscard]] float getRotation() const;
    void setRotation(float rotation);

    [[nodiscard]] const glm::vec2& getPositions() const;
    void setPosition(const glm::vec2& position);

    [[nodiscard]] const glm::vec2& getScale() const;
    void setScale(const glm::vec2& scale);

    [[nodiscard]] const glm::bvec2& getFlip() const;
    void setFlip(const glm::bvec2& flip);

    [[maybe_unused]] [[nodiscard]] uint16_t getOrderInBatch() const;
    [[maybe_unused]] void setOrderInBatch(uint16_t orderInBatch);

    [[nodiscard]] const glm::vec4& getColor() const;
    void setColor(const glm::vec4& color);

    [[nodiscard]] const glm::ivec2& getSpritePos() const;
    [[nodiscard]] const glm::ivec2& getSpriteSize() const;
    [[nodiscard]] const glm::vec2& getSpriteAnchor() const; // 锚点
    // 获取AABB 8个顶点的世界坐标
    [[nodiscard]] std::array<glm::vec2, 4> getTrimmedCorners() const;
    std::array<glm::vec2, 4> getUvs();

private:
    Sprite(glm::ivec2 spritePos, glm::ivec2 spriteSize, glm::ivec2 spriteSourcePos, glm::ivec2 spriteSourceSize, glm::vec2 spriteAnchor, Texture* texture);

private:
    glm::vec4 m_color = { 1.0f, 1.0f, 1.0f, 1.0f };
    glm::vec2 m_position = { 0.0f, 0.0f };
    glm::vec2 m_scale = { 1.0f, 1.0f };
    glm::bvec2 m_flip = { false, false };
    glm::ivec2 m_spritePos{ 0.0f, 0.0f };
    glm::ivec2 m_spriteSize{ 0.0f, 0.0f };
    glm::vec2 m_spriteAnchor{ 0.0f, 0.0f };
    glm::ivec2 m_spriteSourcePos{ 0, 0 };
    glm::ivec2 m_spriteSourceSize{ 0, 0 };
    Texture* m_texture{ nullptr };
    float m_rotation{ 0.0f };
    union
    {
        uint64_t globalOrder;
        PACK(struct
             {
                 uint16_t drawOrder; // lowest priority
                 uint32_t texture;
                 uint16_t orderInBatch; // highest priority
             })
        details;
    } m_order{};
    friend class SpriteAtlas;
    friend class SpriteBatch;
    friend class Inspector;
};
} // namespace ceres

#endif // SIMPLERENDERENGINE_SPRITE_H
