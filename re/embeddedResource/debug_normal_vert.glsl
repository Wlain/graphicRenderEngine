#version 330
in vec3 position;
in vec3 normal;
out vec3 vNormal;

uniform mat4 g_model;
uniform mat3 g_model_view_it;
#include "uniforms_incl.glsl"

void main() {
    gl_Position = g_projection * g_view * g_model * vec4(position,1.0);
    vNormal = normal;
}