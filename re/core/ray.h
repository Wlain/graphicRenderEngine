//
// Created by cwb on 2022/6/30.
//

#ifndef SIMPLERENDERENGINE_RAY_H
#define SIMPLERENDERENGINE_RAY_H
#include <glm/glm.hpp>
namespace re
{
// ray =  position + t * direction;
struct Ray
{
    glm::vec3 position{ 0.0f };
    glm::vec3 direction{ 0.0f };
    float t{};
    inline glm::vec3 target() const
    {
        return position + t * direction;
    }
};
} // namespace re

#endif // SIMPLERENDERENGINE_RAY_H
