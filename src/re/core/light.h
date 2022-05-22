//
// Created by william on 2022/5/22.
//

#ifndef SIMPLERENDERENGINE_LIGHT_H
#define SIMPLERENDERENGINE_LIGHT_H
#include <glm/glm.hpp>
namespace re
{
enum class LightType
{
    Point,
    Directional
};

struct Light
{
    Light();
    ~Light();
    LightType light{ LightType::Point };
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;
    float range;
    Light(LightType light, const glm::vec3& position, const glm::vec3& direction, const glm::vec3& color, float range);
};
} // namespace re

#endif //SIMPLERENDERENGINE_LIGHT_H
