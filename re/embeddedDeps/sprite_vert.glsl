#version 140
in vec3 position;
in vec4 uv;
in vec4 color;
out vec2 vUV;
out vec4 vColor;

uniform mat4 g_model;
uniform mat4 g_view;
uniform mat4 g_projection;

void main(void) {
    gl_Position = g_projection * g_view * g_model * vec4(position,1.0);
    vUV = uv.xy;
    vColor = color;
}