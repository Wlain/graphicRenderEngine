layout(std140) uniform g_global_uniforms {
    uniform highp mat4 g_view;
    uniform highp mat4 g_projection;
    uniform highp vec4 g_viewport;
    uniform highp vec4 g_cameraPos;
    uniform highp vec4 g_ambientLight;
    uniform highp vec4 g_lightColorRange[SI_LIGHTS];
    uniform highp vec4 g_lightPosType[SI_LIGHTS];
};

uniform mat4 g_model;
uniform mat3 g_model_it;
uniform mat3 g_model_view_it;