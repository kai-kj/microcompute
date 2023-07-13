#version 430

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 0) buffer buff1 {
    float data1[];
};

layout(std430, binding = 1) buffer buff2 {
    float data2[];
};

void main(void) {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = ivec2(gl_NumWorkGroups.xy);

    data1[pos.x] = data1[pos.x] + 2;
    data2[pos.x] = data2[pos.x] * 2;
}