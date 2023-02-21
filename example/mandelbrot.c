#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "../out/microcompute.h"

int main(void) {
	int width = 10000;
	int height = 10000;
	int pixels = width * height;

	mc_set_debug_callback(mc_default_debug_callback);
	if (!mc_start("/dev/dri/renderD129")) return -1;

	mc_Program *renderProgram = mc_program_from_file("example/mandelbrot.glsl");
	if (renderProgram == NULL) return -1;

	mc_Program *convertProgram = mc_program_from_file("example/convert.glsl");
	if (convertProgram == NULL) return -1;

	mc_Buffer *floatImage = mc_buffer_create(1, sizeof(float) * pixels * 4);
	mc_buffer_write(floatImage, 0, sizeof(float) * pixels * 4, NULL);

	mc_Buffer *byteImage = mc_buffer_create(2, sizeof(int) * pixels);
	mc_buffer_write(byteImage, 0, sizeof(int) * pixels, NULL);

	mc_program_set_float(renderProgram, "maxIter", 1000);
	mc_program_set_vec2(renderProgram, "center", (mc_vec2){-0.7615, -0.08459});
	mc_program_set_vec2(renderProgram, "zoom", (mc_vec2){1000, 1000});

	mc_program_dispatch(renderProgram, (mc_ivec3){width, height, 1});
	mc_program_dispatch(convertProgram, (mc_ivec3){width, height, 1});

	char *data = malloc(pixels * 4);
	mc_buffer_read(byteImage, 0, pixels * 4, data);
	stbi_write_bmp("out/mandelbrot.bmp", width, height, 4, data);
	free(data);

	mc_program_destroy(renderProgram);
	mc_stop();
}