#version 460
layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 0) buffer buff1 {
    vec4 floatData[];
};

layout(std430, binding = 1) buffer buff2 {
    int byteData[];
};

ivec2 get_pos() {
    return ivec2(gl_GlobalInvocationID);
}

ivec2 get_size() {
    return ivec2((gl_NumWorkGroups.xy) * gl_WorkGroupSize.xy);
}

void main(void) {
    ivec2 pos = get_pos();
    ivec2 size = get_size();

    ivec4 color = clamp(
        ivec4(floatData[(pos.y * size.x + pos.x)] * 255.0),
        0, 255
    );

    int bytes = color.r << 0 | color.g << 8 | color.b << 16 | color.a << 24;
    byteData[(pos.y * size.x + pos.x)] = bytes;
}