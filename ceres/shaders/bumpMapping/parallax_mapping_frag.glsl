#version 330
out vec4 fragColor;
in vec2 vUv;
in vec3 vWorldPos;
in vec3 vNormal;

#include "uniforms_incl.glsl"
#include "light_incl.glsl"
#include "utils_incl.glsl"
uniform sampler2D normalTex;
uniform sampler2D tex;
uniform sampler2D depthTex;

uniform float heightScale;

vec2 parallaxMapping(vec2 texCoords, vec3 viewDir)
{
    float height = texture(depthTex, texCoords).r;
    return texCoords - viewDir.xy * (height * heightScale);
}

mat3 calculateTBN()
{
    vec3 posDx = dFdx(vWorldPos);
    vec3 posDy = dFdy(vWorldPos);
    vec3 texDx = dFdx(vec3(vUv, 0.0));
    vec3 texDy = dFdy(vec3(vUv, 0.0));
    // 根据矩阵求解
    // texDx.s: dU1
    // texDx.t: dV1
    // texDy.s: dU2
    // texDy.t: dV2
    // posDx:E1
    // posDy:E2
    // y = (dV2 * E1 - dV1 * E2) / (dU1 * dV2 - dU2 * dV1)
    vec3 t = (texDy.t * posDx - texDx.t * posDy) / (texDx.s * texDy.t - texDy.s * texDx.t);
    vec3 ng = normalize(vNormal);
    // 当在更大的网格上计算切线向量的时候，它们往往有很大数量的共享顶点，当法向贴图应用到这些表面时将切线向量平均化通常能获得更好更平滑的结果。
    // 这样做有个问题，就是TBN向量可能会不能互相垂直，这意味着TBN矩阵不再是正交矩阵了。法线贴图可能会稍稍偏移，但这仍然可以改进。
    // 格拉姆-施密特正交化：在不必花费太多性能开销的情况下稍稍提升画质表现
    // 表示t在ng下的投影
    t = normalize(t - ng * dot(ng, t));
    vec3 b = normalize(cross(ng, t));
    mat3 tbn = mat3(t, b, ng);
    return tbn;
}

void main()
{
    mat3 tbn = calculateTBN();
    vec3 worldPos = tbn * vWorldPos;
    vec3 cameraPos = tbn * g_cameraPos.xyz;
    vec3 viewDir = cameraPos - worldPos;
    vec2 texCoords = parallaxMapping(vUv,  viewDir);
    vec3 specularLight = vec3(0.0, 0.0, 0.0);
    vec3 normal = texture(normalTex, texCoords).rgb;
    // 将法线向量转换为范围[-1,1]
    normal = normalize(normal * 2.0 - 1.0);
    vec3 light = computeLightBlinnPhongWithParallaxMapping(worldPos, viewDir, normal, tbn, specularLight);
    fragColor = toLinear(texture(tex, texCoords)) * vec4(light, 1.0);
    fragColor = toOutput(fragColor);
}