#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "../out/microcompute.h"

int main(void) {
    mc_Result res;

    int maxErrorLength = 2048;
    char error[maxErrorLength];
    int width = 1000;
    int height = 1000;
    int pixels = width * height;

    res = mc_start("/dev/dri/renderD129");
    if (!res.ok) {
        mc_result_pretty_print(res);
        return -1;
    }

    mc_Program renderProgram;
    res = mc_program_from_file(
        &renderProgram,
        "example/mandelbrot.glsl",
        maxErrorLength,
        error
    );
    if (!res.ok) {
        mc_result_pretty_print(res);
        printf("error: %s\n", error);
        return -1;
    }

    mc_Program convertProgram;
    res = mc_program_from_file(
        &convertProgram,
        "example/convert.glsl",
        maxErrorLength,
        error
    );
    if (!res.ok) {
        mc_result_pretty_print(res);
        printf("error: %s\n", error);
        return -1;
    }

    mc_Buffer floatImage;
    mc_buffer_create(&floatImage, 1, sizeof(float) * pixels * 4);

    mc_buffer_write(&floatImage, 0, sizeof(float) * pixels * 4, NULL);

    mc_Buffer byteImage;
    mc_buffer_create(&byteImage, 2, sizeof(int) * pixels);

    mc_buffer_write(&byteImage, 0, sizeof(int) * pixels, NULL);

    mc_program_set_float(&renderProgram, "maxIter", 1000);
    mc_program_set_vec2(&renderProgram, "center", (mc_vec2){-0.7615, -0.08459});
    mc_program_set_vec2(&renderProgram, "zoom", (mc_vec2){1000, 1000});

    mc_program_dispatch(&renderProgram, (mc_ivec3){width, height, 1});
    mc_program_dispatch(&convertProgram, (mc_ivec3){width, height, 1});

    char* data = malloc(pixels * 4);
    mc_buffer_read(&byteImage, 0, pixels * 4, data);
    stbi_write_bmp("out/mandelbrot.bmp", width, height, 4, data);
    free(data);

    mc_buffer_destroy(&floatImage);
    mc_buffer_destroy(&byteImage);
    mc_program_destroy(&renderProgram);
    mc_program_destroy(&convertProgram);
    mc_stop();
}