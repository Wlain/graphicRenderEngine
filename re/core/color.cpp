//
// Created by cwb on 2022/6/13.
//

#include "color.h"

#include <glm/gtc/color_space.hpp>

namespace re
{
Color::Color() = default;

Color::Color(float _r, float _g, float _b, float _a) :
    r(_r), g(_g), b(_b), a(_a)
{
}

float& Color::operator[](int index)
{
    switch (index)
    {
    case 0:
        return r;
    case 1:
        return g;
    case 2:
        return b;
    default:
        return a;
    }
}

glm::vec4 Color::toLinear()
{
    glm::vec3 color{ r, g, b };
    return { convertSRGBToLinear(color), a };
}

void Color::setFromLinear(glm::vec4 linear)
{
    glm::vec3 color{ linear.x, linear.y, linear.z };
    color = convertLinearToSRGB(color);
    for (int i = 0; i < 3; i++)
    {
        (*this)[i] = color[i];
    }
    a = linear.w;
}

Color::Color(float value) :
    r(value),
    g(value),
    b(value),
    a(value)
{
}
} // namespace re