#version 330
out vec4 fragColor;
in vec3 vNormal;
in vec2 vUv;
in vec3 vWorldPos;

#include "utils_incl.glsl"
uniform sampler2D normalTex;
uniform sampler2D tex;

void main()
{
    fragColor = vec4(vNormal*0.5+0.5,1.0);
    fragColor = texture(tex, vUv);
    fragColor = toOutput(fragColor);
}