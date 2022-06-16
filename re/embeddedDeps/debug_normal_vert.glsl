#version 140
in vec3 position;
in vec3 normal;
out vec3 vNormal;

uniform mat4 g_model;
uniform mat4 g_view;
uniform mat4 g_projection;
uniform mat3 g_model_view_it;

void main(void) {
    gl_Position = g_projection * g_view * g_model * vec4(position,1.0);
    vNormal = normal;
}