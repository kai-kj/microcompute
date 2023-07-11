#include <stdio.h>
#include <stdlib.h>

#define MICROCOMPUTE_IMPLEMENTATION
#include "microcompute.h"

static void debug_cb(
    mc_DebugLevel level,
    const char* source,
    const char* msg,
    void* arg
) {
    (void)!arg;
    printf("[%s from %s]: %s\n", mc_debug_level_to_str(level), source, msg);
}

int main(void) {
    mc_Instance_t* instance = mc_instance_create(debug_cb, NULL);
    mc_Device_t* device = mc_instance_get_devices(instance)[0];

    float arr1[] = {1.0, 2.0, 3.0, 4.0, 5.0};
    mc_Buffer_t* buff1 = mc_buffer_from(device, sizeof arr1, arr1);

    float arr2[] = {10.0, 100.0, 1000.0, 10000.0, 100000.0};
    mc_Buffer_t* buff2 = mc_buffer_from(device, sizeof arr2, arr2);

    mc_Program_t* program = mc_program_create(device, "array.spv", "main", 2);

    mc_program_run(program, (mc_uvec3_t){5, 1, 1}, buff1, buff2);

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
