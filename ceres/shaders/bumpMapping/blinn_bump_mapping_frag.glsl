#version 330
out vec4 fragColor;
in vec3 vNormal;
in vec3 vWorldPos;
in vec3 vPosition;
in vec2 vUv;

uniform sampler2D tex;

#include "utils_incl.glsl"
#include "uniforms_incl.glsl"
#include "light_incl.glsl"

void main()
{
    vec3 normalDir = normalize(vNormal);
    float depth = 0.25;// controls depth of bumps
    float width = 100.0;// controls width of bumps
    normalDir = vNormal + depth * sin(width * vPosition);
    normalDir = normalize(normalDir);
    vec3 specularLight = vec3(0.0, 0.0, 0.0);
    vec3 light = computeLightBlinnPhong(vWorldPos, g_cameraPos.xyz, normalDir, specularLight);
    fragColor = toLinear(texture(tex, vUv)) * vec4(light, 1.0);
    fragColor = toOutput(fragColor);
}