//
// Created by william on 2022/5/22.
//

#include "light.h"
namespace re
{
Light::Light() = default;

Light::LightBuilder::~LightBuilder()
{
    delete m_light;
}

Light::LightBuilder::LightBuilder() :
    m_light{ new Light() }
{
}

Light::LightBuilder Light::create()
{
    return {};
}

Light::LightBuilder& Light::LightBuilder::withPointLight(glm::vec3 pos)
{
    m_type = Type::Point;
    m_position = pos;
    return *this;
}

Light::LightBuilder& Light::LightBuilder::withDirectionalLight(glm::vec3 dir)
{
    m_type = Type::Directional;
    m_direction = glm::normalize(dir);
    return *this;
}

Light::LightBuilder& Light::LightBuilder::withColor(Color _color, float intensity)
{
    m_color = glm::vec3(_color.toLinear()) * intensity;
    return *this;
}

Light::LightBuilder& Light::LightBuilder::withRange(float _range)
{
    m_range = _range;
    return *this;
}

Light& Light::LightBuilder::build()
{
    auto& light = *m_light;
    light.position = m_position;
    light.direction = m_direction;
    light.color = m_color;
    light.range = m_range;
    light.type = m_type;
    return *m_light;
}
} // namespace re