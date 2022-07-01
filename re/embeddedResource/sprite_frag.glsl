#version 330
out vec4 fragColor;
in vec2 vUV;
in vec4 vColor;

uniform sampler2D tex;

#include "utils_incl.glsl"

void main(void)
{
    fragColor = vColor * toLinear(texture(tex, vUV));
    fragColor = toOutput(fragColor);
}