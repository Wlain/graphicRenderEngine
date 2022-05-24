//
// Created by william on 2022/5/23.
//

#ifndef SIMPLERENDERENGINE_DEBUG_H
#define SIMPLERENDERENGINE_DEBUG_H
#include <glm/glm.hpp>

namespace re
{
class Debug
{
public:
    static glm::vec4 getColor();
    static void setColor(const glm::vec4& color);
    static void drawLine(glm::vec3 from, glm::vec3 to);

private:
    inline static glm::vec4 s_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
};
} // namespace re

#endif //SIMPLERENDERENGINE_DEBUG_H
