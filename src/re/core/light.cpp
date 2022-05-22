//
// Created by william on 2022/5/22.
//

#include "light.h"
namespace re
{
Light::Light() = default;
Light::~Light() = default;

Light::Light(LightType light, const glm::vec3& position, const glm::vec3& direction, const glm::vec3& color, float range) :
    light(light), position(position), direction(direction), color(color), range(range)
{}

} // namespace re