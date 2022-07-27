#version 330
in vec3 vNormal;
in vec3 vWorldPos;
out vec4 fragColor;

uniform samplerCube tex;

#include "utils_incl.glsl"
#include "uniforms_incl.glsl"

/// 反射
void main()
{
    vec3 eyeDir = normalize(vWorldPos - g_cameraPos.xyz);
    vec3 reflectDir = reflect(eyeDir, normalize(vNormal));
    fragColor = vec4(texture(tex, reflectDir).rgb, 1.0);
}
