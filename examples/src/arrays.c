#include <stdio.h>
#include <stdlib.h>

#include "microcompute.h"

static char* src = //
    "#version 430\n"
    "layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;\n"
    "layout(std430, binding = 0) buffer buff1 {\n"
    "    float data[];\n"
    "};\n"
    "uniform float v;\n"
    "void main(void) {\n"
    "    data[gl_GlobalInvocationID.x] = data[gl_GlobalInvocationID.x] * v;\n"
    "}\n";

int main(void) {
    k_Result res;

    res = mc_start();
    if (!res.ok) {
        K_RESULT_PRINT(res);
        return -1;
    }

    int maxErrLen = 2048;
    char error[maxErrLen];

    mc_Program* program = mc_program_from_string(src, maxErrLen, error);
    if (!res.ok) {
        K_RESULT_PRINT(res);
        printf("error: %s\n", error);
        return -1;
    }

    float data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    for (int i = 0; i < 10; i++) printf("%f, ", data[i]);
    printf("\n");

    mc_Buffer* buff = mc_buffer_create(sizeof(float) * 10);

    mc_buffer_write(buff, 0, sizeof(float) * 10, data);
    mc_program_set_float(program, "v", 9);

    mc_program_dispatch(program, (mc_ivec3){10, 1, 1}, 1, (mc_Buffer*[]){buff});

    mc_buffer_read(buff, 0, sizeof(float) * 10, data);
    for (int i = 0; i < 10; i++) printf("%f, ", data[i]);
    printf("\n");

    mc_buffer_destroy(buff);
    mc_program_destroy(program);
    mc_stop();

    return 0;
}