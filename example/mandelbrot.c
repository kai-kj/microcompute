#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "../out/microcompute.h"

void debug_cb(mc_DebugLevel level, char *msg) {
	if (level < mc_DebugLevel_MEDIUM) return;
	printf("DEBUG (level %d): %s\n", level, msg);
}

int main(void) {
	int width = 10000;
	int height = 10000;

	mc_set_debug_callback(debug_cb);
	if (!mc_init("/dev/dri/renderD129")) return -1;

	mc_Program *renderProgram = mc_program_from_file("example/mandelbrot.glsl");
	if (renderProgram == NULL) return -1;

	mc_Program *convertProgram = mc_program_from_file("example/convert.glsl");
	if (convertProgram == NULL) return -1;

	mc_Buffer *floatImage = mc_buffer_create(1);
	mc_buffer_write(floatImage, sizeof(float) * width * height * 4, NULL);

	mc_Buffer *byteImage = mc_buffer_create(2);
	mc_buffer_write(byteImage, sizeof(int) * width * height, NULL);

	mc_program_set_float(renderProgram, "maxIter", 1000);
	mc_program_set_vec2(renderProgram, "center", (mc_vec2){-0.7615, -0.08459});
	mc_program_set_vec2(renderProgram, "zoom", (mc_vec2){1000, 1000});

	mc_program_dispatch(renderProgram, (mc_ivec3){width, height, 1});
	mc_program_dispatch(convertProgram, (mc_ivec3){width, height, 1});

	char *data = malloc(width * height * 4);
	mc_buffer_read(byteImage, 0, width * height * 4, data);
	stbi_write_bmp("out/mandelbrot.bmp", width, height, 4, data);
	free(data);

	mc_program_destroy(renderProgram);
	mc_terminate();
}