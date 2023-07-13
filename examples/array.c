#include <stdio.h>
#include <stdlib.h>

#define MICROCOMPUTE_IMPLEMENTATION
#include "microcompute.h"

static void debug_cb(mc_DebugLevel_t level, char* src, char* msg, void* arg) {
    (void)!arg;
    printf("[%s from %s]: %s\n", mc_debug_level_to_str(level), src, msg);
}

int main(void) {
    mc_Instance_t* instance = mc_instance_create((mc_debug_cb*)debug_cb, NULL);
    mc_Device_t* device = mc_instance_get_devices(instance)[0];

    float arr1[] = {0.0, 1.0, 2.0, 3.0, 4.0};
    float arr2[] = {5.0, 6.0, 7.0, 8.0, 9.0};

    mc_Buffer_t* buff1 = mc_buffer_create_from(device, sizeof arr1, arr1);
    mc_Buffer_t* buff2 = mc_buffer_create_from(device, sizeof arr2, arr2);

    mc_Program_t* program = mc_program_create(device, "array.spv");
    mc_program_setup(program, "main", 5, 1, 1, buff1, buff2);
    mc_program_run(program);

    mc_buffer_read(buff1, 0, sizeof arr1, arr1);
    mc_buffer_read(buff2, 0, sizeof arr2, arr2);

    printf("arr1: ");
    for (uint32_t i = 0; i < 5; i++) printf("%f, ", arr1[i]);
    printf("\n");

    printf("arr2: ");
    for (uint32_t i = 0; i < 5; i++) printf("%f, ", arr2[i]);
    printf("\n");

    mc_buffer_destroy(buff1);
    mc_buffer_destroy(buff2);
    mc_program_destroy(program);
    mc_instance_destroy(instance);
}
