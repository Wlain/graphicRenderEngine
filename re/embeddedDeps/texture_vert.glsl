#version 330
in vec3 position;
in vec4 uv;
out vec2 vUV;

uniform mat4 g_model;
#pragma include "uniforms_incl.glsl"

void main(void) {
    gl_Position = g_projection * g_view * g_model * vec4(position, 1.0);
    vUV = uv.xy;
}