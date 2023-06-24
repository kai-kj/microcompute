#include <stdio.h>

#define MICROCOMPUTE_IMPLEMENTATION
#include "microcompute.h"

static char* renSrc = //
    "#version 430\n"
    "layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;\n"
    "layout(std430, binding = 0) buffer buff1 {\n"
    "    float data1[5];\n"
    "};\n"
    "layout(std430, binding = 1) buffer buff2 {\n"
    "    float data2[5];\n"
    "};\n"
    "void main(void) {\n"
    "    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);\n"
    "    ivec2 size = ivec2(gl_NumWorkGroups.xy);\n"
    "    data2[pos.x] = data1[pos.x] + data2[pos.x];\n"
    "}\n";

typedef struct ShaderData {
    mc_int maxIter;
    mc_vec2 center;
    mc_float zoom;
} ShaderData;

void debug_cb(mc_DebugLevel level, const char* msg, void* arg) {
    printf("%d: %s\n", level, msg);
}

int main(void) {
    mc_uvec3 size = (mc_uvec3){10, 1, 1};
    mc_float arr1Data[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    mc_float arr2Data[] = {6.0, 7.0, 8.0, 9.0, 10.0};

    char* error = NULL;

    if ((error = mc_initialize(debug_cb, NULL)) != NULL) {
        printf("error at mc_initialize: %s\n", error);
        return -1;
    }

    mc_Program* program = mc_program_create(renSrc);
    if ((error = mc_program_check(program)) != NULL) {
        printf("error at mc_program_create:\n%s\n", error);
        return -1;
    }

    mc_Buffer* buff1 = mc_buffer_create(sizeof arr1Data);
    mc_Buffer* buff2 = mc_buffer_create(sizeof arr2Data);

    mc_buffer_pack(buff1, MC_FLOAT | MC_ARRAY(5), arr1Data);
    mc_buffer_pack(buff2, MC_FLOAT | MC_ARRAY(5), arr2Data);

    double time = mc_program_run_blocking(program, size, buff1, buff2);

    printf("compute time: %f[s]\n", time);

    mc_float arrOutData[5];
    mc_buffer_unpack(buff2, MC_FLOAT | MC_ARRAY(5), arrOutData);

    for (int i = 0; i < 5; i++) printf("%f\n", arrOutData[i]);

    mc_buffer_destroy(buff1);
    mc_buffer_destroy(buff2);
    mc_program_destroy(program);
    mc_terminate();
}