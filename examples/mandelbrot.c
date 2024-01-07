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

int main(void) {
    int width = 3840, height = 2160;
    size_t imgSize = sizeof(int) * width * height;

    struct Settings settings = {
        .center = {-0.7615, -0.08459},
        .zoom = 1000,
        .maxIter = 500,
    };

    mc_Instance_t* instance = mc_instance_create(mc_default_debug_cb, NULL);
    mc_Device_t* dev = mc_instance_get_devices(instance)[0];

    mc_Buffer_t* imgBuff = mc_buffer_create(dev, imgSize);

    mc_Program_t* prog = mc_program_create(dev, "mandelbrot.spv");
    mc_program_setup(prog, "main", sizeof settings, imgBuff);
    double time = mc_program_run(prog, width, height, 1, &settings);
    printf("compute time: %f[s]\n", time);

    void* img = malloc(imgSize);
    mc_buffer_read(imgBuff, 0, imgSize, img);
    stbi_write_bmp("mandelbrot.bmp", width, height, 4, img);

    free(img);
    mc_buffer_destroy(imgBuff);
    mc_program_destroy(prog);
    mc_instance_destroy(instance);
}