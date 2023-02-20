#version 450
layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 1) buffer buff1 {
    float data[];
};

uniform float test;

ivec2 get_pos() {
	return ivec2(gl_GlobalInvocationID);
}

ivec2 get_size() {
	return ivec2((gl_NumWorkGroups.xy) * gl_WorkGroupSize.xy);
}

void main(void) {
	data[get_pos().x] = data[get_pos().x] * test;
}