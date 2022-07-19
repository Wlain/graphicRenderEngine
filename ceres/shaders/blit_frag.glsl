#version 330
out vec4 fragColor;
in vec2 vUV;

uniform sampler2D tex;

#include "utils_incl.glsl"

void main()
{
    fragColor = toLinear(texture(tex, vUV));
    fragColor = toOutput(fragColor);
}