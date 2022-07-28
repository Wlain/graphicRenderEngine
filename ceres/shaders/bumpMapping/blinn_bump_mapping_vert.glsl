#version 330
in vec3 position;
in vec3 normal;
in vec4 uv;
out vec3 vNormal;
out vec3 vWorldPos;
out vec3 vPosition;
out vec2 vUv;

#include "uniforms_incl.glsl"

void main()
{
    vec4 worldPos = g_model * vec4(position, 1.0);
    vWorldPos = worldPos.xyz;
    gl_Position = g_projection * g_view * worldPos;
    vNormal = normal;
    vPosition = position;
    vUv = uv.xy;
}