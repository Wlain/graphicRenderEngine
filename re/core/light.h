//
// Created by william on 2022/5/22.
//

#ifndef SIMPLERENDERENGINE_LIGHT_H
#define SIMPLERENDERENGINE_LIGHT_H
#include <glm/glm.hpp>
namespace re
{
struct Light
{
    enum class Type
    {
        Point,
        Directional,
        Unused
    };

    Light();
    ~Light();
    Type type{ Type::Unused };
    glm::vec3 position{};
    glm::vec3 direction{};
    glm::vec3 color{};
    float range{};
    Light(Type type, const glm::vec3& position, const glm::vec3& direction, const glm::vec3& color, float range);
};
} // namespace re

#endif //SIMPLERENDERENGINE_LIGHT_H
