#version 330
in vec3 position;
in vec3 normal;
out vec3 vNormal;
out vec3 vWorldPos;

#include "uniforms_incl.glsl"
// 折射
void main() {
    vWorldPos = vec3(g_model * vec4(position, 1.0));
    gl_Position = g_projection * g_view * g_model * vec4(position, 1.0);
    vNormal = normal;
}