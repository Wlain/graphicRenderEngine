#version 330
in vec3 position;
in vec3 normal;
in vec4 uv;

out vec3 vNormal;
out vec2 vUv;

#include "uniforms_incl.glsl"

void main() {
    gl_Position = g_projection * g_view * g_model * vec4(position,1.0);
    vNormal = normal;
    vUv = uv.xy;
}