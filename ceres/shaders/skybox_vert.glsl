#version 330
in vec3 position;
out vec3 vUV;

#include "uniforms_incl.glsl"

void main() {
    vec4 eyespacePos = (g_view * vec4(position, 0.0));
    eyespacePos.w = 1.0;
    gl_Position = g_model * eyespacePos;// 模型矩阵包含无限投影
    vUV = position;
}