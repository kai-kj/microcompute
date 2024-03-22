#include <stdio.h>

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
        .center = {.x = -0.7615f, .y = -0.08459f},
        .zoom = 1000,
        .maxIter = 500,
    };

    mc_Instance* instance = mc_instance_create(mc_log_cb_simple, NULL);
    mc_Device* dev = mc_instance_get_devices(instance)[0];

    mc_Buffer* optBuff = mc_buffer_create(dev, MC_BUFFER_TYPE_CPU, sizeof opt);
    mc_buffer_write(optBuff, 0, sizeof opt, &opt);
    mc_Buffer* imgBuff = mc_buffer_create(dev, MC_BUFFER_TYPE_CPU, imgSize);

    FILE* fp = fopen("mandelbrot.spv", "rb");
    fseek(fp, 0, SEEK_END);
    size_t codeLen = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char* code = malloc(codeLen);
    fread(code, 1, codeLen, fp);
    fclose(fp);

    mc_Program* prog = mc_program_create(dev, codeLen, code, "main");

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