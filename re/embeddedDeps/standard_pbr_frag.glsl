#version 330
#extension GL_EXT_shader_texture_lod: enable
#extension GL_OES_standard_derivatives : enable
out vec4 fragColor;
#if defined(S_TANGENTS) && defined(S_NORMALMAP)
in mat3 vTBN;
#else
in vec3 vNormal;
#endif
in vec2 vUV;
in vec3 vWsPos;
uniform vec4 color;
uniform vec4 metallicRoughness;
uniform vec4 g_cameraPos;
uniform sampler2D tex;
#ifdef S_METALROUGHNESSMAP
uniform sampler2D mrTex;
#endif
#ifdef S_NORMALMAP
uniform sampler2D normalTex;
uniform float normalScale;
#endif
#ifdef S_EMISSIVEMAP
uniform sampler2D emissiveTex;
uniform vec4 emissiveFactor;
#endif
#ifdef S_OCCLUSIONMAP
uniform sampler2D occlusionTex;
uniform float occlusionStrength;
#endif
#ifdef S_VERTEX_COLOR
in vec4 vColor;
#endif

#pragma include "normalmap_incl.glsl"
#pragma include "utils_incl.glsl"
#pragma include "light_incl.glsl"


// Encapsulate the various inputs used by the various functions in the shading equation
// We store values in this struct to simplify the integration of alternative implementations
// of the shading terms, outlined in the Readme.MD Appendix.
struct PBRInfo
{
    float NdotL;// 法向量和光照方向的夹角
    float NdotV;// 法向量和相机方向的夹角
    float NdotH;// 法向量和半程向量的夹角
    float LdotH;// 光照方向和半程向量的夹角
    float VdotH;// 相机方向和半程向量的夹角
    float perceptualRoughness;// 表面粗糙度
    float metalness;// 表面金属度
    vec3 reflectance0;// 全反射色(法向入射角) F0
    vec3 reflectance90;// reflectance color at grazing angle
    float alphaRoughness;// roughness mapped to a more linear change in the roughness (proposed by [2])：粗糙度映射
    vec3 diffuseColor;// 漫反射颜色
    vec3 specularColor;// 高光颜色
};

const float M_PI = 3.141592653589793;
const float c_MinRoughness = 0.04;

// The following equation models the Fresnel reflectance term of the spec equation (aka F())
// Implementation of fresnel from [4], Equation 15
// 菲涅耳反射项
vec3 specularReflection(PBRInfo pbrInputs)
{
    return pbrInputs.reflectance0 + (pbrInputs.reflectance90 - pbrInputs.reflectance0) * pow(clamp(1.0 - pbrInputs.VdotH, 0.0, 1.0), 5.0);
}

// Basic Lambertian diffuse
// Implementation from Lambert's Photometria https://archive.org/details/lambertsphotome00lambgoog
// See also [1], Equation 1
vec3 diffuse(PBRInfo pbrInputs)
{
    return pbrInputs.diffuseColor / M_PI;
}

// This calculates the specular geometric attenuation (aka G()),
// where rougher material will reflect less light back to the viewer.
// This implementation is based on [1] Equation 4, and we adopt their modifications to
// alphaRoughness as input as originally proposed in [2].
// 自遮挡项（smith GGX）
float geometricOcclusion(PBRInfo pbrInputs)
{
    float NdotL = pbrInputs.NdotL;
    float NdotV = pbrInputs.NdotV;
    float r = pbrInputs.alphaRoughness;

    float attenuationL = 2.0 * NdotL / (NdotL + sqrt(r * r + (1.0 - r * r) * (NdotL * NdotL)));
    float attenuationV = 2.0 * NdotV / (NdotV + sqrt(r * r + (1.0 - r * r) * (NdotV * NdotV)));
    return attenuationL * attenuationV;
}

// The following equation(s) model the distribution of microfacet normals across the area being drawn (aka D())
// Implementation from "Average Irregularity Representation of a Roughened Surface for Ray Reflection" by T. S. Trowbridge, and K. P. Reitz
// Follows the distribution function recommended in the SIGGRAPH 2013 course notes from EPIC Games [1], Equation 3.
// 法线分布函数（GGX）
float microfacetDistribution(PBRInfo pbrInputs)
{
    float roughnessSq = pbrInputs.alphaRoughness * pbrInputs.alphaRoughness;
    float f = pbrInputs.NdotH * (roughnessSq - 1.0) + 1.0;
    return roughnessSq / (M_PI * f * f);
}

void main(void)
{
    float perceptualRoughness = metallicRoughness.y;
    float metallic = metallicRoughness.x;

    #ifdef S_METALROUGHNESSMAP
    // Roughness is stored in the 'g' channel, metallic is stored in the 'b' channel.
    // This layout intentionally reserves the 'r' channel for (optional) occlusion map data
    vec4 mrSample = texture(mrTex, vUV);
    perceptualRoughness = mrSample.g * perceptualRoughness;
    metallic = mrSample.b * metallic;
    #endif
    perceptualRoughness = clamp(perceptualRoughness, c_MinRoughness, 1.0);
    metallic = clamp(metallic, 0.0, 1.0);
    // Roughness is authored as perceptual roughness; as is convention,
    // convert to material roughness by squaring the perceptual roughness [2].
    float alphaRoughness = perceptualRoughness * perceptualRoughness;

    #ifndef S_NO_BASECOLORMAP
    vec4 baseColor = toLinear(texture(tex, vUV)) * color;
    #else
    vec4 baseColor = color;
    #endif
    #ifdef S_VERTEX_COLOR
    baseColor = baseColor * vColor;
    #endif

    vec3 f0 = vec3(0.04);
    vec3 diffuseColor = baseColor.rgb * (vec3(1.0) - f0);
    diffuseColor *= 1.0 - metallic;

    vec3 specularColor = mix(f0, baseColor.rgb, metallic);

    // Compute reflectance.
    float reflectance = max(max(specularColor.r, specularColor.g), specularColor.b);

    // For typical incident reflectance range (between 4% to 100%) set the grazing reflectance to 100% for typical fresnel effect.
    // For very low reflectance range on highly diffuse objects (below 4%), incrementally reduce grazing reflectance to 0%.
    float reflectance90 = clamp(reflectance * 25.0, 0.0, 1.0);
    vec3 specularEnvironmentR0 = specularColor.rgb;
    vec3 specularEnvironmentR90 = vec3(1.0, 1.0, 1.0) * reflectance90;
    vec3 color = baseColor.rgb * g_ambientLight;// non pbr
    vec3 n = getNormal();// Normal at surface point
    vec3 v = normalize(g_cameraPos.xyz - vWsPos.xyz);// Vector from surface point to camera
    for (int i=0;i<SI_LIGHTS;i++) {
        float attenuation = 0.0;
        vec3 l = vec3(0.0,0.0,0.0);
        lightDirectionAndAttenuation(g_lightPosType[i], g_lightColorRange[i].w, vWsPos, l, attenuation);
        if (attenuation <= 0.0) {
            continue;
        }
        vec3 h = normalize(l+v);// Half vector between both l and v
        vec3 reflection = -normalize(reflect(v, n));

        float NdotL = clamp(dot(n, l), 0.0001, 1.0);
        float NdotV = abs(dot(n, v)) + 0.0001;
        float NdotH = clamp(dot(n, h), 0.0, 1.0);
        float LdotH = clamp(dot(l, h), 0.0, 1.0);
        float VdotH = clamp(dot(v, h), 0.0, 1.0);

        PBRInfo pbrInputs = PBRInfo(
        NdotL,
        NdotV,
        NdotH,
        LdotH,
        VdotH,
        perceptualRoughness,
        metallic,
        specularEnvironmentR0,
        specularEnvironmentR90,
        alphaRoughness,
        diffuseColor,
        specularColor
        );

        // Calculate the shading terms for the microfacet specular shading model
        vec3 F = specularReflection(pbrInputs);
        float G = geometricOcclusion(pbrInputs);
        float D = microfacetDistribution(pbrInputs);

        // Calculation of analytical lighting contribution
        vec3 diffuseContrib = (1.0 - F) * diffuse(pbrInputs);
        vec3 specContrib = F * G * D / (4.0 * NdotL * NdotV);// BRDF
        color += attenuation * NdotL * g_lightColorRange[i].xyz * (diffuseContrib + specContrib);
    }

        // Apply optional PBR terms for additional (optional) shading
        #ifdef S_OCCLUSIONMAP
    float ao = texture(occlusionTex, vUV).r;
    color = mix(color, color * ao, occlusionStrength);
    #endif
    #ifdef S_EMISSIVEMAP
    vec3 emissive = toLinear(texture(emissiveTex, vUV)).rgb * emissiveFactor.xyz;
    color += emissive;
    #endif

    fragColor = toOutput(color, baseColor.a);
}