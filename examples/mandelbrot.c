#include <stdio.h>

#define MC_IMPLEMENTATION
#include "microcompute.h"
#include "microcompute_vec.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

struct Opt {
    mc_vec2 center;
    float zoom;
    int maxIter;
};

int main(void) {
    int width = 3840, height = 2160;
    size_t imgSize = sizeof(int) * width * height;

    struct Opt opt = {
        .center = {.x = -0.7615, .y = -0.08459},
        .zoom = 1000,
        .maxIter = 500,
    };

    mc_Instance_t* instance = mc_instance_create(mc_default_log_cb, NULL);
    mc_Device_t* dev = mc_instance_get_devices(instance)[0];

    mc_Buffer_t* optBuff = mc_buffer_create_from(dev, sizeof opt, &opt);
    mc_Buffer_t* imgBuff = mc_buffer_create(dev, imgSize);

    mc_Program_t* prog
        = mc_program_create_from_file(dev, "mandelbrot.spv", "main");
    double time = mc_program_run(prog, width, height, 1, optBuff, imgBuff);
    printf("compute time: %f[s]\n", time);

    void* img = malloc(imgSize);
    mc_buffer_read(imgBuff, 0, imgSize, img);
    stbi_write_png("mandelbrot.png", width, height, 4, img, width * 4);

    free(img);
    mc_buffer_destroy(optBuff);
    mc_buffer_destroy(imgBuff);
    mc_program_destroy(prog);
    mc_instance_destroy(instance);
}