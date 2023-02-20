#include <stdio.h>
#include <stdlib.h>

#include "../out/microcompute.h"

void debug_cb(mc_DebugLevel level, char *msg) {
	if (level < mc_DebugLevel_MEDIUM) return;
	printf("DEBUG (level %d): %s\n", level, msg);
}

int main(void) {
	mc_set_debug_callback(debug_cb);
	if (!mc_init("/dev/dri/renderD129")) return -1;

	mc_Program *program = mc_program_create_from_file("example/test1.glsl");
	if (program == NULL) return -1;

	float data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	for (int i = 0; i < 10; i++) printf("%f, ", data[i]);
	printf("\n");

	mc_Buffer *buff = mc_buffer_create(1);
	mc_buffer_write(buff, sizeof(float) * 10, data);
	mc_program_set_float(program, "test", 9);

	mc_program_dispatch(program, (mc_ivec3){10, 1, 1});

	mc_buffer_read(buff, 0, sizeof(float) * 10, data);
	for (int i = 0; i < 10; i++) printf("%f, ", data[i]);
	printf("\n");

	mc_program_destroy(program);
	mc_terminate();

	return 0;
}