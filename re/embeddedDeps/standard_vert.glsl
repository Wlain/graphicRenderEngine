#version 330
in vec3 position;
in vec3 normal;
in vec4 uv;
out vec3 vNormal;
out vec2 vUV;
out vec3 vEyePos;

uniform mat4 g_model;
#pragma include "uniforms_incl.glsl"
uniform mat3 g_model_view_it;

void main(void) {
    vec4 eyePos = g_view * g_model * vec4(position, 1.0);
    vEyePos = eyePos.xyz;
    vNormal = normalize(g_model_view_it * normal);
    vUV = uv.xy;
    gl_Position = g_projection * eyePos;
}
