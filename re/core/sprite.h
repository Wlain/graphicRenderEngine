//
// Created by william on 2022/6/1.
//

#ifndef SIMPLERENDERENGINE_SPRITE_H
#define SIMPLERENDERENGINE_SPRITE_H
#include <array>
#include <glm/glm.hpp>
#include <string>
/// 精灵图集
namespace re
{
class SpriteAtlas;
class Texture;
class Sprite
{
public:
    Sprite();
    virtual ~Sprite();
    Sprite(const Sprite&);
    float getRotation() const;
    void setRotation(float rotation);

    const glm::vec2& getPositions() const;
    void setPosition(const glm::vec2& position);

    const glm::vec2& getScale() const;
    void setScale(const glm::vec2& scale);

    const glm::bvec2& getFlip() const;
    void setFlip(const glm::bvec2& flip);

    int getOrderInBatch() const;
    void setOrderInBatch(int orderInBatch);

    const glm::vec4& getColor() const;
    void setColor(const glm::vec4& color);

    const glm::ivec2& getSpritePos() const;
    const glm::ivec2& getSpriteSize() const;
    const glm::vec2& getSpriteAnchor() const; // 锚点
    // 获取AABB 8个顶点的世界坐标
    std::array<glm::vec2, 4> getCorners() const;
    std::array<glm::vec2, 4> getUvs();

private:
    Sprite(glm::ivec2 spritePos, glm::ivec2 spriteSize, glm::vec2 spriteAnchor, Texture* texture);

private:
    glm::vec4 m_color = { 1.0f, 1.0f, 1.0f, 1.0f };
    glm::vec2 m_position = { 0.0f, 0.0f };
    glm::vec2 m_scale = { 1.0f, 1.0f };
    glm::bvec2 m_flip = { false, false };
    glm::ivec2 m_spritePos{ 0.0f, 0.0f };
    glm::ivec2 m_spriteSize{ 0.0f, 0.0f };
    glm::vec2 m_spriteAnchor{ 0.0f, 0.0f };
    Texture* m_texture{ nullptr };
    float m_rotation{ 0.0f };
    int m_orderInBatch{ 0 };
    friend class SpriteAtlas;
    friend class SpriteBatch;
    friend class Profiler;
};
} // namespace re

#endif //SIMPLERENDERENGINE_SPRITE_H
