#version 330
in vec3 vNormal;
in vec3 vWorldPos;
out vec4 fragColor;

uniform samplerCube tex;

#include "utils_incl.glsl"
#include "uniforms_incl.glsl"

// 折射率
// 空气	1.00
// 水	1.33
// 冰	1.309
// 玻璃	1.52
// 钻石	2.42
void main()
{
    float ratio = 1.00 / 2.42;
    vec3 eyeDir = normalize(vWorldPos - g_cameraPos.xyz);
    vec3 refractDir = refract(eyeDir, normalize(vNormal), ratio);
    fragColor = vec4(texture(tex, refractDir).rgb, 1.0);
}
