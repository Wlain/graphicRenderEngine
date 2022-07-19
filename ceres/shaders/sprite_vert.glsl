#version 330
in vec3 position;
in vec4 uv;
in vec4 color;
out vec2 vUV;
out vec4 vColor;

#include "uniforms_incl.glsl"

void main() {
    gl_Position = g_projection * g_view * g_model * vec4(position,1.0);
    vUV = uv.xy;
    vColor = color;
}