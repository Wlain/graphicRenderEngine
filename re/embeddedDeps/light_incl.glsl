in vec4 vLightDir[SI_LIGHTS];
uniform vec3 g_ambientLight;

uniform vec4 g_lightPosType[SI_LIGHTS];
uniform vec4 g_lightColorRange[SI_LIGHTS];
uniform vec4 specularity;

void lightDirectionAndAttenuation(vec4 lightPosType, float lightRange, vec3 pos, out vec3 lightDirection, out float attenuation){
    bool isDirectional = lightPosType.w == 0.0;
    bool isPoint       = lightPosType.w == 1.0;

    if (isDirectional){
        lightDirection = lightPosType.xyz;
        attenuation = 1.0;
    } else if (isPoint) {
        vec3 lightVector = lightPosType.xyz - pos;
        float lightVectorLength = length(lightVector);

        lightDirection = lightVector / lightVectorLength;// normalize
        if (lightRange <= 0.0) { // attenuation disabled
            attenuation = 1.0;
        } else if (lightVectorLength >= lightRange){
            attenuation = 0.0;
            return;
        } else {
            attenuation = pow(1.0 - lightVectorLength / lightRange, 1.5);// non physical range based attenuation
        }
    } else {
        attenuation = 0.0;
        lightDirection = vec3(0.0, 0.0, 0.0);
    }
}

vec3 computeLightBlinnPhong(vec3 wsPos, vec3 wsCameraPos, vec3 normal, out vec3 specularityOut){
    specularityOut = vec3(0.0, 0.0, 0.0);
    vec3 lightColor = vec3(0.0, 0.0, 0.0);
    vec3 cam = normalize(wsCameraPos - wsPos);
    for (int i = 0; i< SI_LIGHTS; ++i){
        vec3 lightDirection = vec3(0.0,0.0,0.0);
        float att = 0.0;
        lightDirectionAndAttenuation(g_lightPosType[i], g_lightColorRange[i].w, wsPos, lightDirection, att);
        if (att <= 0.0){
            continue;
        }
        // diffuse light
        float diffuse = max(0.0, dot(lightDirection, normal));
        if (diffuse > 0.0){
            lightColor += (att * diffuse) * g_lightColorRange[i].xyz;
        }

        // specular light
        if (specularity.a > 0.0){
            // 半程向量
            vec3 H = normalize(lightDirection + cam);
            float nDotHV = dot(normal, H);
            if (nDotHV > 0.0){
                float pf = pow(nDotHV, specularity.a);
                specularityOut += specularity.rgb * pf * att;// white specular highlights
            }
        }
    }
    lightColor = max(g_ambientLight.xyz, lightColor);

    return lightColor;
}

vec3 computeLightPhong(vec3 wsPos, vec3 wsCameraPos, vec3 normal, out vec3 specularityOut) {
    specularityOut = vec3(0.0, 0.0, 0.0);
    vec3 lightColor = vec3(0.0, 0.0, 0.0);
    vec3 cam = normalize(wsCameraPos - wsPos);
    for (int i = 0; i< SI_LIGHTS; i++){
        vec3 lightDirection = vec3(0.0, 0.0, 0.0);
        float att = 0.0;
        lightDirectionAndAttenuation(g_lightPosType[i], g_lightColorRange[i].w, wsPos, lightDirection, att);
        if (att <= 0.0) {
            continue;
        }
        // diffuse light
        float diffuse = dot(lightDirection, normal);
        if (diffuse > 0.0) {
            lightColor += (att * diffuse) * g_lightColorRange[i].xyz;
        }
        // specular light
        if (specularity.a > 0.0){
            vec3 R = reflect(-lightDirection, normal);
            float nDotRV = dot(cam, R);
            if (nDotRV > 0.0){
                float pf = pow(nDotRV, specularity.a);
                specularityOut += specularity.rgb * (pf * att);// white specular highlights
            }
        }
    }
    lightColor = max(g_ambientLight.xyz, lightColor);
    return lightColor;
}
