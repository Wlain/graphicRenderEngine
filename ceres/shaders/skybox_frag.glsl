#version 330
out vec4 fragColor;
in vec3 vUV;

uniform vec4 color;
uniform samplerCube tex;

#include "utils_incl.glsl"

void main()
{
    fragColor = color * toLinear(texture(tex, vUV));
    fragColor = toOutput(fragColor);
}
