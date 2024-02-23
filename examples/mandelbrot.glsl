#version 430

layout(std430, binding = 0) buffer optBuff {
    vec2 center;
    float zoom;
    int maxIter;
};

layout(std430, binding = 1) buffer imgBuff {
    int img[];
};

void main(void) {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = ivec2(gl_NumWorkGroups.xy);

    vec2 screenPos = vec2(pos) / vec2(size) - 0.5;

    vec2 z0 = center + screenPos / zoom;
    vec2 z = vec2(0.0, 0.0);
    vec2 z2 = vec2(0.0, 0.0);

    int i = 0;
    for (; i < maxIter && z2.x + z2.y <= 4; i++) {
        z = vec2(z2.x - z2.y + z0.x, 2 * z.x * z.y + z0.y);
        z2 = vec2(z.x * z.x, z.y * z.y);
    }

    float shade = float(i) / float(maxIter) * 255;
    ivec3 c = clamp(ivec3(shade.xxx), 0, 255);

    img[pos.y * size.x + pos.x] = c.r << 0 | c.g << 8 | c.b << 16 | 255 << 24;
}