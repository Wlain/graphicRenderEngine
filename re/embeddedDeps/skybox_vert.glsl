#version 330
in vec3 position;
out vec3 vUV;

uniform mat4 g_model;
uniform mat4 g_view;
uniform mat4 g_projection;

void main(void) {
    vec4 eyespacePos = (g_view * vec4(position, 0.0));
    eyespacePos.w = 1.0;
    gl_Position = g_model * eyespacePos;// 模型矩阵包含无限投影
    vUV = position;
}