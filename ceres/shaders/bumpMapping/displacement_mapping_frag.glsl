#version 330
out vec4 fragColor;
in vec2 vUV;
#ifdef S_VERTEX_COLOR
in vec4 vColor;
#endif

uniform vec4 color;
uniform sampler2D tex;
uniform sampler2D displaceMap;
uniform float time;

#include "utils_incl.glsl"

void main()
{
    vec2 texcoord = vec2(vUV.s + time, vUV.t + time);
    vec4 displace = texture(displaceMap, texcoord);
    float displaceValue  = displace.g * 1.0;
    vec2 uvDisplaced = vec2(vUV.x + displaceValue, vUV.y + displaceValue);
    fragColor = color * toLinear(texture(tex, uvDisplaced));
    #ifdef S_VERTEX_COLOR
    fragColor = fragColor * vColor;
    #endif
    fragColor = toOutput(fragColor);
}