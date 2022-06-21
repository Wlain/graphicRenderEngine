#version 330
in vec3 position;
out vec3 vUv;
out vec3 vLightDirection;

uniform mat4 g_model;
uniform mat4 g_view;
uniform mat4 g_projection;
uniform vec4 g_viewport;
uniform vec4 g_cameraPos;
uniform vec4 g_ambientLight;
uniform vec4 g_lightColorRange[SI_LIGHTS];
uniform vec4 g_lightPosType[SI_LIGHTS];
uniform mat3 g_model_it;
uniform mat3 g_model_view_it;

void main(void) {
    vec4 eyespacePos = (g_view * vec4(position, 0.0));
    eyespacePos.w = 1.0;
    gl_Position = g_model * eyespacePos;// 模型矩阵包含无限投影
    vUV = position;
    vLightDirection = vec3(1.0);
    for (int i = 0; i < SI_LIGHTS; i++) {
        // 方向光
        if (g_lightPosType[i].w == 1.0) {
            vLightDirection = g_lightPosType[i].xyz;
            break;
        }
    }
}