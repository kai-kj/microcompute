#version 430

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 0) buffer buff {
    float data[];
};

void main(void) {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = ivec2(gl_NumWorkGroups.xy);
    data[pos.x] = data[pos.x] * 2;
}