#version 330
out vec4 fragColor;
in vec3 vNormal;

#include "utils_incl.glsl"

void main(void)
{
    fragColor = vec4(vNormal*0.5+0.5,1.0);
    fragColor = toOutput(fragColor);
}