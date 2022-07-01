#version 330
out vec4 fragColor;
in vec4 vUV;

#include "utils_incl.glsl"

void main(void)
{
    fragColor = vUV;
    fragColor = toOutput(fragColor);
}