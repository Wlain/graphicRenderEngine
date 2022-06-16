// autogenerated by
// files_to_cpp shader src/embedded_deps/sre_utils_incl.glsl sre_utils_incl.glsl src/embedded_deps/debug_normal_frag.glsl debug_normal_frag.glsl src/embedded_deps/debug_normal_vert.glsl debug_normal_vert.glsl src/embedded_deps/debug_uv_frag.glsl debug_uv_frag.glsl src/embedded_deps/debug_uv_vert.glsl debug_uv_vert.glsl src/embedded_deps/light_phong_incl.glsl light_phong_incl.glsl src/embedded_deps/particles_frag.glsl particles_frag.glsl src/embedded_deps/particles_vert.glsl particles_vert.glsl src/embedded_deps/sprite_frag.glsl sprite_frag.glsl src/embedded_deps/sprite_vert.glsl sprite_vert.glsl src/embedded_deps/standard_pbr_frag.glsl standard_pbr_frag.glsl src/embedded_deps/standard_pbr_vert.glsl standard_pbr_vert.glsl src/embedded_deps/standard_blinn_phong_frag.glsl standard_blinn_phong_frag.glsl src/embedded_deps/standard_blinn_phong_vert.glsl standard_blinn_phong_vert.glsl src/embedded_deps/unlit_frag.glsl unlit_frag.glsl src/embedded_deps/unlit_vert.glsl unlit_vert.glsl src/embedded_deps/debug_tangent_frag.glsl debug_tangent_frag.glsl src/embedded_deps/debug_tangent_vert.glsl debug_tangent_vert.glsl src/embedded_deps/normalmap_incl.glsl normalmap_incl.glsl include/sre/impl/ShaderSource.inl
#include <map>
#include <string>
#include <utility>

std::map<std::string, std::string> builtInShaderSource{
    std::make_pair<std::string, std::string>("sre_utils_incl.glsl", R"(vec4 toLinear(vec4 col){
#ifndef SI_TEX_SAMPLER_SRGB
    float gamma = 2.2;
    return vec4 (
        col.xyz = pow(col.xyz, vec3(gamma)),
        col.w
    );
#else
    return col;
#endif
}

vec4 toOutput(vec4 colorLinear){
#ifndef SI_FRAMEBUFFER_SRGB
    float gamma = 2.2;
    return vec4(pow(colorLinear.xyz,vec3(1.0/gamma)), colorLinear.a); // gamma correction
#else
    return colorLinear;
#endif
}

vec4 toOutput(vec3 colorLinear, float alpha){
#ifndef SI_FRAMEBUFFER_SRGB
    float gamma = 2.2;
    return vec4(pow(colorLinear,vec3(1.0/gamma)), alpha); // gamma correction
#else
    return vec4(colorLinear, alpha);                      // pass through
#endif
})"),
    std::make_pair<std::string, std::string>("debug_normal_frag.glsl", R"(#version 140
out vec4 fragColor;
in vec3 vNormal;

#pragma include "sre_utils_incl.glsl"

void main(void)
{
    fragColor = vec4(vNormal*0.5+0.5,1.0);
    fragColor = toOutput(fragColor);
})"),
    std::make_pair<std::string, std::string>("debug_normal_vert.glsl", R"(#version 140
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
})"),
    std::make_pair<std::string, std::string>("debug_uv_frag.glsl", R"(#version 140
out vec4 fragColor;
in vec4 vUV;

#pragma include "sre_utils_incl.glsl"

void main(void)
{
    fragColor = vUV;
    fragColor = toOutput(fragColor);
})"),
    std::make_pair<std::string, std::string>("debug_uv_vert.glsl", R"(#version 140
in vec3 position;
in vec4 uv;
out vec4 vUV;

uniform mat4 g_model;
uniform mat4 g_view;
uniform mat4 g_projection;

void main(void) {
    gl_Position = g_projection * g_view * g_model * vec4(position,1.0);
    vUV = uv;
})"),
    std::make_pair<std::string, std::string>("light_phong_incl.glsl", R"(uniform vec3 g_ambientLight;
uniform vec4 g_lightPosType[SI_LIGHTS];
uniform vec4 g_lightColorRange[SI_LIGHTS];
uniform vec4 specularity;

vec3 computeLight(vec3 wsPos, vec3 wsCameraPos, vec3 normal, out vec3 specularityOut){
    specularityOut = vec3(0.0, 0.0, 0.0);
    vec3 lightColor = vec3(0.0,0.0,0.0);
    for (int i=0;i<SI_LIGHTS;i++){
        bool isDirectional = g_lightPosType[i].w == 0.0;
        bool isPoint       = g_lightPosType[i].w == 1.0;
        vec3 lightDirection;
        float att = 1.0;
        if (isDirectional){
            lightDirection = g_lightPosType[i].xyz;
        } else if (isPoint) {
            vec3 lightVector = g_lightPosType[i].xyz - wsPos;
            float lightVectorLength = length(lightVector);
            float lightRange = g_lightColorRange[i].w;
            lightDirection = lightVector / lightVectorLength; // compute normalized lightDirection (using length)
            if (lightRange <= 0.0){
                att = 1.0;
            } else if (lightVectorLength >= lightRange){
                att = 0.0;
            } else {
                att = pow(1.0 - lightVectorLength / lightRange,1.5); // non physical range based attenuation
            }
        } else {
            continue;
        }

        // diffuse light
        float thisDiffuse = max(0.0,dot(lightDirection, normal));
        if (thisDiffuse > 0.0){
            lightColor += (att * thisDiffuse) * g_lightColorRange[i].xyz;
        }

        // specular light
        if (specularity.a > 0.0){
            vec3 H = normalize(lightDirection - normalize(wsPos - wsCameraPos));
            float nDotHV = dot(normal, H);
            if (nDotHV > 0.0){
                float pf = pow(nDotHV, specularity.a);
                specularityOut += specularity.rgb*pf; // white specular highlights
            }
        }
    }
    lightColor = max(g_ambientLight.xyz, lightColor);

    return lightColor;
})"),
    std::make_pair<std::string, std::string>("particles_frag.glsl", R"(#version 140
out vec4 fragColor;
in mat3 vUVMat;
in vec3 uvSize;
in vec4 vColor;
#ifdef GL_ES
uniform precision highp vec4 g_viewport;
#else
uniform vec4 g_viewport;
#endif

uniform sampler2D tex;

#pragma include "sre_utils_incl.glsl"

void main(void)
{
    vec2 uv = (vUVMat * vec3(gl_PointCoord,1.0)).xy;

    if (uv != clamp(uv, uvSize.xy, uvSize.xy + uvSize.zz)){
        discard;
    }
    vec4 c = vColor * toLinear(texture(tex, uv));
    fragColor = c;
    fragColor = toOutput(fragColor);
})"),
    std::make_pair<std::string, std::string>("particles_vert.glsl", R"(#version 140
in vec3 position;
in float particleSize;
in vec4 uv;
in vec4 color;
out mat3 vUVMat;
out vec4 vColor;
out vec3 uvSize;

uniform mat4 g_model;
uniform mat4 g_view;
uniform mat4 g_projection;
uniform vec4 g_viewport;

mat3 translate(vec2 p){
 return mat3(1.0,0.0,0.0,0.0,1.0,0.0,p.x,p.y,1.0);
}

mat3 rotate(float rad){
  float s = sin(rad);
  float c = cos(rad);
 return mat3(c,s,0.0,-s,c,0.0,0.0,0.0,1.0);
}

mat3 scale(float s){
  return mat3(s,0.0,0.0,0.0,s,0.0,0.0,0.0,1.0);
}

void main(void) {
    vec4 pos = vec4( position, 1.0);
    vec4 eyeSpacePos = g_view * g_model * pos;
    gl_Position = g_projection * eyeSpacePos;
    if (g_projection[2][3] != 0.0){ // if perspective projection
        gl_PointSize = (g_viewport.y / 600.0) * particleSize * 1.0 / -eyeSpacePos.z;
    } else {
        gl_PointSize = particleSize*(g_viewport.y / 600.0);
    }

    vUVMat = translate(uv.xy)*scale(uv.z) * translate(vec2(0.5,0.5))*rotate(uv.w) * translate(vec2(-0.5,-0.5));
    vColor = color;
    uvSize = uv.xyz;
})"),
    std::make_pair<std::string, std::string>("sprite_frag.glsl", R"(#version 140
out vec4 fragColor;
in vec2 vUV;
in vec4 vColor;

uniform sampler2D tex;

#pragma include "sre_utils_incl.glsl"

void main(void)
{
    fragColor = vColor * toLinear(texture(tex, vUV));
    fragColor = toOutput(fragColor);
})"),
    std::make_pair<std::string, std::string>("sprite_vert.glsl", R"(#version 140
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
})"),
    std::make_pair<std::string, std::string>("standard_pbr_frag.glsl", R"(#version 140
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
in vec3 vLightDir[SI_LIGHTS];

uniform vec3 g_ambientLight;
uniform vec4 g_lightColorRange[SI_LIGHTS];
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
#pragma include "sre_utils_incl.glsl"


// Encapsulate the various inputs used by the various functions in the shading equation
// We store values in this struct to simplify the integration of alternative implementations
// of the shading terms, outlined in the Readme.MD Appendix.
struct PBRInfo
{
    float NdotL;                  // cos angle between normal and light direction
    float NdotV;                  // cos angle between normal and view direction
    float NdotH;                  // cos angle between normal and half vector
    float LdotH;                  // cos angle between light direction and half vector
    float VdotH;                  // cos angle between view direction and half vector
    float perceptualRoughness;    // roughness value, as authored by the model creator (input to shader)
    float metalness;              // metallic value at the surface
    vec3 reflectance0;            // full reflectance color (normal incidence angle)
    vec3 reflectance90;           // reflectance color at grazing angle
    float alphaRoughness;         // roughness mapped to a more linear change in the roughness (proposed by [2])
    vec3 diffuseColor;            // color contribution from diffuse lighting
    vec3 specularColor;           // color contribution from specular lighting
};

const float M_PI = 3.141592653589793;
const float c_MinRoughness = 0.04;

// The following equation models the Fresnel reflectance term of the spec equation (aka F())
// Implementation of fresnel from [4], Equation 15
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
float microfacetDistribution(PBRInfo pbrInputs)
{
    float roughnessSq = pbrInputs.alphaRoughness * pbrInputs.alphaRoughness;
    float f = (pbrInputs.NdotH * roughnessSq - pbrInputs.NdotH) * pbrInputs.NdotH + 1.0;
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
    vec3 color = baseColor.rgb * g_ambientLight;      // non pbr
    vec3 n = getNormal();                             // Normal at surface point
    vec3 v = normalize(g_cameraPos.xyz - vWsPos.xyz); // Vector from surface point to camera
    for (int i=0;i<SI_LIGHTS;i++) {
        vec3 l = normalize(vLightDir[i]);                 // Vector from surface point to light
        vec3 h = normalize(l+v);                          // Half vector between both l and v
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
        vec3 specContrib = F * G * D / (4.0 * NdotL * NdotV);
        color += NdotL * g_lightColorRange[i].xyz * (diffuseContrib + specContrib);
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

    fragColor = toOutput(color,baseColor.a);
})"),
    std::make_pair<std::string, std::string>("standard_pbr_vert.glsl", R"(#version 140
in vec3 position;
in vec3 normal;
in vec4 uv;
#if defined(S_TANGENTS) && defined(S_NORMALMAP)
in vec4 tangent;
out mat3 vTBN;
#else
out vec3 vNormal;
#endif
#ifdef S_VERTEX_COLOR
in vec4 color;
out vec4 vColor;
#endif
out vec2 vUV;
out vec3 vLightDir[SI_LIGHTS];
out vec3 vWsPos;

uniform mat4 g_model;
uniform mat4 g_view;
uniform mat4 g_projection;
uniform mat3 g_model_it;
uniform vec4 g_lightPosType[SI_LIGHTS];

#pragma include "normalmap_incl.glsl"

void main(void) {
    vec4 wsPos = g_model * vec4(position,1.0);
    vWsPos = wsPos.xyz;
    gl_Position = g_projection * g_view * wsPos;
#if defined(S_TANGENTS) && defined(S_NORMALMAP)
    vTBN = computeTBN(g_model_it, normal, tangent);
#else
    vNormal = normalize(g_model_it * normal);
#endif
    vUV = uv.xy;

    for (int i=0;i<SI_LIGHTS;i++){
        bool isDirectional = g_lightPosType[i].w == 0.0;
        bool isPoint       = g_lightPosType[i].w == 1.0;
        vec3 lightDirection;
        float att = 1.0;
        if (isDirectional){
            vLightDir[i] = g_lightPosType[i].xyz;
        } else if (isPoint) {
            vLightDir[i] = normalize(g_lightPosType[i].xyz - vWsPos);
        }
    }
#ifdef S_VERTEX_COLOR
    vColor = color;
#endif
})"),
    std::make_pair<std::string, std::string>("standard_blinn_phong_frag.glsl", R"(#version 140
out vec4 fragColor;
#if defined(S_TANGENTS) && defined(S_NORMALMAP)
in mat3 vTBN;
#else
in vec3 vNormal;
#endif
in vec2 vUV;
in vec3 vWsPos;
uniform vec4 g_cameraPos;
#ifdef S_NORMALMAP
uniform sampler2D normalTex;
uniform float normalScale;
#endif
#ifdef S_VERTEX_COLOR
in vec4 vColor;
#endif

uniform vec4 color;
uniform sampler2D tex;

#pragma include "light_phong_incl.glsl"
#pragma include "normalmap_incl.glsl"
#pragma include "sre_utils_incl.glsl"

void main()
{
    vec4 c = color * toLinear(texture(tex, vUV));
#ifdef S_VERTEX_COLOR
    c = c * vColor;
#endif
    vec3 normal = getNormal();
    vec3 specularLight = vec3(0.0,0.0,0.0);
    vec3 l = computeLight(vWsPos, g_cameraPos.xyz, normal, specularLight);

    fragColor = c * vec4(l, 1.0) + vec4(specularLight,0);
    fragColor = toOutput(fragColor);
})"),
    std::make_pair<std::string, std::string>("standard_blinn_phong_vert.glsl", R"(#version 140
in vec3 position;
in vec3 normal;
in vec4 uv;
out vec2 vUV;
#if defined(S_TANGENTS) && defined(S_NORMALMAP)
in vec4 tangent;
out mat3 vTBN;
#else
out vec3 vNormal;
#endif
out vec3 vWsPos;
#ifdef S_VERTEX_COLOR
in vec4 color;
out vec4 vColor;
#endif

uniform mat4 g_model;
uniform mat4 g_view;
uniform mat4 g_projection;
uniform mat3 g_model_it;

#pragma include "normalmap_incl.glsl"

void main(void) {
    vec4 wsPos = g_model * vec4(position,1.0);
    gl_Position = g_projection * g_view * wsPos;
#if defined(S_TANGENTS) && defined(S_NORMALMAP)
    vTBN = computeTBN(g_model_it, normal, tangent);
#else
    vNormal = normalize(g_model_it * normal);
#endif
    vUV = uv.xy;
    vWsPos = vWsPos.xyz;
#ifdef S_VERTEX_COLOR
    vColor = color;
#endif
})"),
    std::make_pair<std::string, std::string>("unlit_frag.glsl", R"(#version 140
out vec4 fragColor;
in vec2 vUV;
#ifdef S_VERTEX_COLOR
in vec4 vColor;
#endif

uniform vec4 color;
uniform sampler2D tex;

#pragma include "sre_utils_incl.glsl"

void main(void)
{
    fragColor = color * toLinear(texture(tex, vUV));
#ifdef S_VERTEX_COLOR
    fragColor = fragColor * vColor;
#endif
    fragColor = toOutput(fragColor);
})"),
    std::make_pair<std::string, std::string>("unlit_vert.glsl", R"(#version 140
in vec3 position;
in vec3 normal;
#ifdef S_VERTEX_COLOR
in vec4 color;
out vec4 vColor;
#endif
in vec4 uv;
out vec2 vUV;

uniform mat4 g_model;
uniform mat4 g_view;
uniform mat4 g_projection;

void main(void) {
    gl_Position = g_projection * g_view * g_model * vec4(position,1.0);
    vUV = uv.xy;
#ifdef S_VERTEX_COLOR
    vColor = color;
#endif
})"),
    std::make_pair<std::string, std::string>("debug_tangent_frag.glsl", R"(#version 140
out vec4 fragColor;
in vec3 vTangent;

#pragma include "sre_utils_incl.glsl"

void main(void)
{
    fragColor = vec4(vTangent*0.5+0.5,1.0);
    fragColor = toOutput(fragColor);
})"),
    std::make_pair<std::string, std::string>("debug_tangent_vert.glsl", R"(#version 140
in vec3 position;
in vec4 tangent;
out vec3 vTangent;

uniform mat4 g_model;
uniform mat4 g_view;
uniform mat4 g_projection;

void main(void) {
    gl_Position = g_projection * g_view * g_model * vec4(position,1.0);
    vTangent = tangent.xyz * tangent.w;
})"),
    std::make_pair<std::string, std::string>("normalmap_incl.glsl", R"(#ifdef SI_VERTEX
mat3 computeTBN(mat3 g_model_it, vec3 normal, vec4 tangent){
    vec3 wsNormal = normalize(g_model_it * normal);
    vec3 wsTangent = normalize(g_model_it * tangent.xyz);
    vec3 wsBitangent = cross(wsNormal, wsTangent) * tangent.w;
    return mat3(wsTangent, wsBitangent, wsNormal);
}
#endif
#ifdef SI_FRAGMENT

// Find the normal for this fragment, pulling either from a predefined normal map
// or from the interpolated mesh normal and tangent attributes.
vec3 getNormal()
{
#ifdef S_NORMALMAP
    // Retrieve the tangent space matrix
#ifndef S_TANGENTS
    vec3 pos_dx = dFdx(vWsPos);
    vec3 pos_dy = dFdy(vWsPos);
    vec3 tex_dx = dFdx(vec3(vUV, 0.0));
    vec3 tex_dy = dFdy(vec3(vUV, 0.0));
    vec3 t = (tex_dy.t * pos_dx - tex_dx.t * pos_dy) / (tex_dx.s * tex_dy.t - tex_dy.s * tex_dx.t);

    vec3 ng = normalize(vNormal);

    t = normalize(t - ng * dot(ng, t));
    vec3 b = normalize(cross(ng, t));
    mat3 tbn = mat3(t, b, ng);
#else // S_TANGENTS
    mat3 tbn = vTBN;
#endif

    vec3 n = texture(normalTex, vUV).rgb;
    n = normalize(tbn * ((2.0 * n - 1.0) * vec3(normalScale, normalScale, 1.0)));
#else
    vec3 n = normalize(vNormal);
#endif

    return n;
}
#endif)"),
};