#version 330
out vec4 fragColor;
in vec3 vNormal;
in vec2 vUv;

#include "utils_incl.glsl"
uniform sampler2D normalTex;
uniform sampler2D tex;

mat3 calculateTBN()
{
    vec3 pos_dx = dFdx(vWsPos);
    vec3 pos_dy = dFdy(vWsPos);
    vec3 tex_dx = dFdx(vec3(vUV, 0.0));
    vec3 tex_dy = dFdy(vec3(vUV, 0.0));
    vec3 t = (tex_dy.t * pos_dx - tex_dx.t * pos_dy) / (tex_dx.s * tex_dy.t - tex_dy.s * tex_dx.t);
    vec3 ng = normalize(vNormal);
    t = normalize(t - ng * dot(ng, t));
    vec3 b = normalize(cross(ng, t));
    mat3 tbn = mat3(t, b, ng);
}


void main()
{
    fragColor = vec4(vNormal*0.5+0.5,1.0);
    fragColor = texture(tex, vUv);
    fragColor = toOutput(fragColor);
}