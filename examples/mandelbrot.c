#include <stdio.h>

#define MICROCOMPUTE_IMPLEMENTATION
#include "microcompute.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

struct Settings {
    float center[2];
    float zoom;
    int maxIter;
};

static void debug_cb(mc_DebugLevel_t level, char* src, char* msg, void* arg) {
    (void)!arg;
    printf("[%s from %s]: %s\n", mc_debug_level_to_str(level), src, msg);
}

int main(void) {
    int width = 3840, height = 2160;
    size_t imgSize = sizeof(int) * width * height;

    struct Settings settings = {
        .center = {-0.7615, -0.08459},
        .zoom = 1000,
        .maxIter = 500,
    };

    mc_Instance_t* instance = mc_instance_create((mc_debug_cb*)debug_cb, NULL);
    mc_Device_t* dev = mc_instance_get_devices(instance)[0];

    mc_Program_t* prog = mc_program_create(dev, "mandelbrot.spv", "main", 2);
    mc_Buffer_t* dataBuff = mc_buffer_from(dev, sizeof settings, &settings);
    mc_Buffer_t* imgBuff = mc_buffer_create(dev, imgSize);

    double time = mc_program_run(prog, width, height, 1, dataBuff, imgBuff);
    printf("compute time: %f[state]\n", time);

    void* img = malloc(imgSize);
    mc_buffer_read(imgBuff, 0, imgSize, img);

    stbi_write_bmp("mandelbrot.bmp", width, height, 4, img);

    free(img);
    mc_buffer_destroy(dataBuff);
    mc_buffer_destroy(imgBuff);
    mc_program_destroy(prog);
    mc_instance_destroy(instance);
}