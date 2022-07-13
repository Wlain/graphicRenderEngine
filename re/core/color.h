//
// Created by cwb on 2022/6/13.
//

#ifndef SIMPLERENDERENGINE_COLOR_H
#define SIMPLERENDERENGINE_COLOR_H
#include <glm/glm.hpp>
namespace re
{
// Color 类表示 sRGBA 颜色(这意味着 RGB 在 gamma 空间中，而 alpha 在线性空间中)
class Color
{
public:
    Color();
    explicit Color(float value);
    Color(float r, float g, float b, float a = 1.0f);
    float& operator[](int index);
    [[nodiscard]] glm::vec4 toLinear() const; // Return color values in linear space
    void setFromLinear(glm::vec4 linear);     // Set sRGBA values from linear space

public:
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    float a = 1.0f;
};
} // namespace re

#endif // SIMPLERENDERENGINE_COLOR_H
