#include <stdio.h>
#include <stdlib.h>

#include "../out/microcompute.h"

int main(void) {
	mc_set_debug_callback(mc_default_debug_callback);
	if (!mc_start("/dev/dri/renderD129")) return -1;

	mc_Program *program = mc_program_from_file("example/arrays.glsl");
	if (program == NULL) return -1;

	float data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	for (int i = 0; i < 10; i++) printf("%f, ", data[i]);
	printf("\n");

	mc_Buffer *buff = mc_buffer_create(1, sizeof(float) * 10);
	mc_buffer_write(buff, 0, sizeof(float) * 10, data);
	mc_program_set_float(program, "test", 9);

	mc_program_dispatch(program, (mc_ivec3){10, 1, 1});

	mc_buffer_read(buff, 0, sizeof(float) * 10, data);
	for (int i = 0; i < 10; i++) printf("%f, ", data[i]);
	printf("\n");

	mc_program_destroy(program);
	mc_stop();

	return 0;
}