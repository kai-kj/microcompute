#include <stdio.h>
#include <stdlib.h>

#include "../out/microcompute.h"

int main(void) {
    mc_Result res;

    res = mc_start("/dev/dri/renderD129");
    if (!res.ok) {
        mc_result_pretty_print(res);
        return -1;
    }

    mc_Program program;
    char error[1024];

    res = mc_program_from_file(&program, "example/arrays.glsl", 1024, error);
    if (!res.ok) {
        mc_result_pretty_print(res);
        printf("error: %s\n", error);
        return -1;
    }

    float data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    for (int i = 0; i < 10; i++) printf("%f, ", data[i]);
    printf("\n");

    mc_Buffer buff;
    mc_buffer_create(&buff, 1, sizeof(float) * 10);

    mc_buffer_write(&buff, 0, sizeof(float) * 10, data);
    mc_program_set_float(&program, "test", 9);

    mc_program_dispatch(&program, (mc_ivec3){10, 1, 1});

    mc_buffer_read(&buff, 0, sizeof(float) * 10, data);
    for (int i = 0; i < 10; i++) printf("%f, ", data[i]);
    printf("\n");

    mc_buffer_destroy(&buff);
    mc_program_destroy(&program);
    mc_stop();

    return 0;
}