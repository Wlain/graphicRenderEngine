#version 330
in vec3 position;

uniform mat4 g_model;
#include "uniforms_incl.glsl"

void main(void) {
    gl_Position = g_projection * g_view * g_model * vec4(position, 1.0);
}