#version 460
layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 1) buffer buff1 {
    vec4 floatData[];
};

uniform float maxIter;
uniform vec2 center;
uniform vec2 zoom;

ivec2 get_pos() {
    return ivec2(gl_GlobalInvocationID);
}

ivec2 get_size() {
    return ivec2((gl_NumWorkGroups.xy) * gl_WorkGroupSize.xy);
}

void main(void) {
    vec2 screenPos = vec2(get_pos()) / vec2(get_size()) - 0.5;

    vec2 z0 = center + screenPos / zoom;
    vec2 z = vec2(0.0, 0.0);
    vec2 z2 = vec2(0.0, 0.0);

    int i = 0;
    for(; i < maxIter && z2.x + z2.y <= 4; i++) {
        z = vec2(z2.x - z2.y + z0.x, 2 * z.x * z.y + z0.y);
        z2 = vec2(z.x * z.x, z.y * z.y);
    }

    float color = float(i) / float(maxIter);
    floatData[(get_pos().y * get_size().x + get_pos().x)] = vec4(color, 0.5 * color, 0.5 * color, 1.0);
}