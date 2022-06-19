//
// Created by william on 2022/6/1.
//

#include "sprite.h"

#include "commonMacro.h"
#include "texture.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
namespace re
{
Sprite::Sprite() = default;
Sprite::~Sprite() = default;

Sprite::Sprite(glm::ivec2 spritePos, glm::ivec2 spriteSize, glm::ivec2 spriteSourcePos, glm::ivec2 spriteSourceSize, glm::vec2 spriteAnchor, Texture* texture) :
    m_spritePos(std::move(spritePos)),
    m_spriteSize(std::move(spriteSize)),
    m_spriteSourcePos(std::move(spriteSourcePos)),
    m_spriteSourceSize(std::move(spriteSourceSize)),
    m_spriteAnchor(std::move(spriteAnchor)),
    m_texture(texture)
{
    m_order.globalOrder = 0;
    m_order.details.texture = (uint32_t)texture->m_info.id;
}

Sprite::Sprite(const Sprite& s) :
    m_rotation(s.m_rotation),
    m_position(s.m_position),
    m_scale(s.m_scale),
    m_flip(s.m_flip),
    m_color(s.m_color),
    m_spritePos(s.m_spritePos),
    m_spriteSize(s.m_spriteSize),
    m_spriteAnchor(s.m_spriteAnchor),
    m_texture(s.m_texture)
{
    m_order.globalOrder = s.m_order.globalOrder;
}

float Sprite::getRotation() const
{
    return m_rotation;
}

void Sprite::setRotation(float rotation)
{
    m_rotation = rotation;
}

const glm::vec2& Sprite::getPositions() const
{
    return m_position;
}

void Sprite::setPosition(const glm::vec2& position)
{
    m_position = position;
}

const glm::vec2& Sprite::getScale() const
{
    return m_scale;
}

void Sprite::setScale(const glm::vec2& scale)
{
    m_scale = scale;
    if (m_scale.x < 0)
    {
        m_scale.x = 0;
        LOG_ERROR("Sprite.scale must be larger or equal to 0");
    }
    if (m_scale.y < 0)
    {
        m_scale.y = 0;
        LOG_ERROR("Sprite.scale must be larger or equal to 0");
    }
}

const glm::bvec2& Sprite::getFlip() const
{
    return m_flip;
}

void Sprite::setFlip(const glm::bvec2& flip)
{
    m_flip = flip;
}

uint16_t Sprite::getOrderInBatch() const
{
    return m_order.details.orderInBatch;
}

void Sprite::setOrderInBatch(uint16_t orderInBatch)
{
    m_order.details.orderInBatch = orderInBatch;
}

const glm::vec4& Sprite::getColor() const
{
    return m_color;
}

void Sprite::setColor(const glm::vec4& color)
{
    m_color = color;
}

const glm::ivec2& Sprite::getSpritePos() const
{
    return m_spritePos;
}

const glm::ivec2& Sprite::getSpriteSize() const
{
    return m_spriteSize;
}

const glm::vec2& Sprite::getSpriteAnchor() const
{
    return m_spriteAnchor;
}

std::array<glm::vec2, 4> Sprite::getTrimmedCorners() const
{
    float x0 = 0 - m_spriteAnchor.x * m_spriteSize.x;
    float x1 = m_spriteSize.x - m_spriteAnchor.x * m_spriteSize.x;
    float y0 = 0 - m_spriteAnchor.y * m_spriteSize.y;
    float y1 = m_spriteSize.y - m_spriteAnchor.y * m_spriteSize.y;
    std::array<glm::vec2, 4> res;
    res[0] = { x1, y0 };
    res[1] = { x1, y1 };
    res[2] = { x0, y1 };
    res[3] = { x0, y0 };
    // compute transformation
    glm::mat4 matrix = glm::translate(glm::vec3{ m_position, 0.0 }) * glm::rotate(glm::radians(m_rotation), glm::vec3{ 0, 0, 1 }) * glm::scale(glm::vec3{ m_scale, 1.0 });
    for (int i = 0; i < 4; i++)
    {
        res[i] = (glm::vec2)(matrix * glm::vec4(res[i], 0.0, 1.0));
    }
    return res;
}

std::array<glm::vec2, 4> Sprite::getUvs()
{
    std::array<glm::vec2, 4> res;
    float texWidth = m_texture->width();
    float texHeight = m_texture->height();
    float x0 = (m_spritePos.x) / texWidth;
    float x1 = (m_spritePos.x + m_spriteSize.x) / texWidth;
    float y0 = (m_spritePos.y) / texHeight;
    float y1 = (m_spritePos.y + m_spriteSize.y) / texHeight;
    if (m_flip.x)
    {
        std::swap(x0, x1);
    }
    if (m_flip.y)
    {
        std::swap(y0, y1);
    }
    res[0] = { x1, y0 };
    res[1] = { x1, y1 };
    res[2] = { x0, y1 };
    res[3] = { x0, y0 };
    return res;
}
} // namespace re