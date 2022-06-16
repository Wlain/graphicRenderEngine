#version 330
#define SCENE_LIGHTS 4
out vec4 fragColor;
in vec3 vNormal;
in vec2 vUV;
in vec3 vEyePos;

uniform vec3 g_ambientLight;
uniform vec4 color;
uniform sampler2D tex;

uniform vec4 g_lightPosType[SCENE_LIGHTS];
uniform vec4 g_lightColorRange[SCENE_LIGHTS];
uniform vec4 specularity;

vec3 computeLight()
{
    vec3 lightColor = g_ambientLight.xyz;
    vec3 normal = normalize(vNormal);

    for (int i = 0; i < SCENE_LIGHTS; i++)
    {
        bool isDirectional = g_lightPosType[i].w == 0.0;
        bool isPoint       = g_lightPosType[i].w == 1.0;
        vec3 lightDirection;
        float att = 1.0;
        if (isDirectional)
        {
            lightDirection = normalize(g_lightPosType[i].xyz);
        }
        else if (isPoint)
        {
            vec3 lightVector = g_lightPosType[i].xyz - vEyePos;
            float lightRange = g_lightColorRange[i].w;
            float lightVectorLength = length(lightVector);
            lightDirection = lightVector/lightVectorLength;
            if (lightRange <= 0.0)
            {
                att = 1.0;
            } else if (lightVectorLength >= lightRange)
            {
                att = 0.0;
            } else
            {
                att = pow(1.0-lightVectorLength/lightRange, 1.5);// non physical range based attenuation
            }
        }
        else
        {
            continue;
        }
        // diffuse light
        float thisDiffuse = max(0.0, dot(lightDirection, normal));
        if (thisDiffuse > 0.0)
        {
            lightColor += (att * thisDiffuse) * g_lightColorRange[i].xyz;
        }
        // specular light
        if (specularity.a > 0)
        {
            vec3 H = normalize(lightDirection - normalize(vEyePos));
            float nDotHV = dot(normal, H);
            if (nDotHV > 0)
            {
                float pf = pow(nDotHV, specularity.a);
                lightColor += vec3(att * pf);// white specular highlights
            }
        }
    }
    return lightColor;
}

void main(void)
{
    vec4 c = color * texture(tex, vUV);
    vec3 light = computeLight();
    fragColor = c * vec4(light, 1.0);
}
