#include <stdio.h>

#include "microcompute.h"
#include "microcompute_extra.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define SPV_PATH "mandelbrot.spv"

struct Opt {
    float center[2];
    float zoom;
    int maxIter;
};

int main(void) {
    int width = 3840, height = 2160;
    size_t imgSize = sizeof(int) * width * height;

    struct Opt opt = {
        .center = {-0.7615f, -0.08459f},
        .zoom = 1000,
        .maxIter = 500,
    };

    mc_Instance* instance = mc_instance_create(mc_log_cb_simple, NULL);
    mc_Device* dev = mc_instance_get_devices(instance)[0];

    mc_HBuffer* optBuff = mc_hybrid_buffer_create_from(dev, sizeof opt, &opt);
    mc_HBuffer* imgBuff = mc_hybrid_buffer_create(dev, imgSize);
    mc_Program* prog = mc_program_create_from_file(dev, SPV_PATH, "main");

    double time = mc_program_run(prog, width, height, 1, optBuff, imgBuff);
    printf("compute time: %f[s]\n", time);

    void* img = malloc(imgSize);
    mc_hybrid_buffer_read(imgBuff, 0, imgSize, img);
    stbi_write_png("mandelbrot.png", width, height, 4, img, width * 4);

    free(img);
    mc_hybrid_buffer_destroy(optBuff);
    mc_hybrid_buffer_destroy(imgBuff);
    mc_program_destroy(prog);
    mc_instance_destroy(instance);
}