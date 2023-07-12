#include <stdio.h>

#define MICROCOMPUTE_IMPLEMENTATION
#include "microcompute.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

struct Settings {
    mc_vec2_t center;
    mc_float_t zoom;
    mc_int_t maxIter;
};

static void debug_cb(mc_DebugLevel_t level, char* src, char* msg, void* arg) {
    (void)!arg;
    printf("[%s from %s]: %s\n", mc_debug_level_to_str(level), src, msg);
}

int main(void) {
    mc_uvec3_t size = (mc_uvec3_t){1000, 1000, 1};
    struct Settings settings = {
        .center = (mc_vec2_t){-0.7615, -0.08459},
        .zoom = 1000,
        .maxIter = 500,
    };

    mc_Instance_t* instance = mc_instance_create((mc_debug_cb*)debug_cb, NULL);
    mc_Device_t* device = mc_instance_get_devices(instance)[0];

    mc_Program_t* program
        = mc_program_create(device, "mandelbrot.spv", "main", 2);

    mc_Buffer_t* dataBuff = mc_buffer_from(device, sizeof settings, &settings);
    mc_Buffer_t* imgBuff
        = mc_buffer_create(device, sizeof(mc_int_t) * size.x * size.y);

    double time = mc_program_run(program, size, dataBuff, imgBuff);
    printf("compute time: %f[state]\n", time);

    char img[size.x * size.y * 4];
    mc_buffer_read(imgBuff, 0, sizeof(mc_int_t) * size.x * size.y, img);

    stbi_write_bmp("mandelbrot.bmp", size.x, size.y, 4, img);

    mc_buffer_destroy(dataBuff);
    mc_buffer_destroy(imgBuff);
    mc_program_destroy(program);
    mc_instance_destroy(instance);
}