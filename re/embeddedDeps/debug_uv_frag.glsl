#version 140
out vec4 fragColor;
in vec4 vUV;

#pragma include "utils_incl.glsl"

void main(void)
{
    fragColor = vUV;
    fragColor = toOutput(fragColor);
}