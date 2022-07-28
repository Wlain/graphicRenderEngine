#version 330
in vec3 position;
in vec3 normal;
in vec4 uv;

out vec2 vUv;
out vec3 vNormal;
out vec3 vWorldPos;

#include "uniforms_incl.glsl"

void main() {
    vec4 worldPos = g_model * vec4(position, 1.0);
    vWorldPos = worldPos.xyz;
    vNormal = mat3(g_view * g_model * inverse(transpose(inverse(g_view * g_model)))) * normal; // 此处还原到原始的法线坐标
    vUv = uv.xy;
    gl_Position = g_projection * g_view * worldPos;
}