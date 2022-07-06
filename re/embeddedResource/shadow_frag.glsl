#version 330
out vec4 fragColor;

vec4 packDepth(const in float depth) {
    const vec4 bitShift = vec4(16777216.0, 65536.0, 256.0, 1.0);
    const vec4 bitMask  = vec4(0.0, 1.0 / 256.0, 1.0 / 256.0, 1.0 / 256.0);
    vec4 res = fract(depth * bitShift);
    res -= res.xxyz * bitMask;
    return res;
}

void main(void)
{
    fragColor = packDepth(gl_FragCoord.z);
}