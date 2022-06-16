#version 330
out vec4 fragColor;
in vec2 vUV;

uniform sampler2D tex;

#pragma include "utils_incl.glsl"

void main(void)
{
    fragColor = toLinear(texture(tex, vUV));
    fragColor = toOutput(fragColor);
}