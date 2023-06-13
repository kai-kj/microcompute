#include <microcompute.h>
#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

static char* renSrc = //
    "#version 430\n"
    "layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;\n"
    "layout(std140, binding = 0) uniform buff0 {\n"
    "    vec2 center;\n"
    "    vec2 zoom;\n"
    "    int maxIter;\n"
    "};\n"
    "layout(std430, binding = 1) buffer buff1 {\n"
    "    int img[];\n"
    "};\n"
    "void main(void) {\n"
    "    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);\n"
    "    ivec2 size = ivec2(gl_NumWorkGroups.xy);\n"
    "    vec2 screenPos = vec2(pos) / vec2(size) - 0.5;\n"
    "    vec2 z0 = center + screenPos / zoom;\n"
    "    vec2 z = vec2(0.0, 0.0);\n"
    "    vec2 z2 = vec2(0.0, 0.0);\n"
    "    int i = 0;\n"
    "    for(; i < maxIter && z2.x + z2.y <= 4; i++) {\n"
    "        z = vec2(z2.x - z2.y + z0.x, 2 * z.x * z.y + z0.y);\n"
    "        z2 = vec2(z.x * z.x, z.y * z.y);\n"
    "    }\n"
    "    float shade = float(i) / float(maxIter) * 255;\n"
    "    ivec3 c = clamp(ivec3(vec3(shade.xxx)), 0, 255);\n"
    "    img[pos.y * size.x + pos.x]\n"
    "        = c.r << 0 | c.g << 8 | c.b << 16 | 255 << 24;\n"
    "}\n";

typedef struct ShaderData {
    mc_vec2 center;
    mc_vec2 zoom;
    mc_int maxIter;
} ShaderData;

void debug_cb(mc_DebugLevel level, const char* msg, void* arg) {
    printf("%d: %s\n", level, msg);
}

int main(void) {
    int width = 1000, height = 1000;
    mc_vec2 center = (mc_vec2){-0.7615, -0.08459};
    mc_float zoom = 1000;
    mc_int iterations = 500;

    char* error = NULL;
    int size = width * height;

    if ((error = mc_initialize(debug_cb, NULL)) != NULL) {
        printf("error at mc_initialize: %s\n", error);
        return -1;
    }

    mc_Program* program = mc_program_create(renSrc);
    if ((error = mc_program_check(program)) != NULL) {
        printf("error at mc_program_create:\n%s\n", error);
        return -1;
    }

    mc_Buffer* data = mc_buffer_create(MC_BUFFER_UNIFORM, sizeof(ShaderData));
    mc_Buffer* img = mc_buffer_create(MC_BUFFER_STORAGE, sizeof(mc_int) * size);

    mc_buffer_write(
        data,
        0,
        sizeof(ShaderData),
        &(ShaderData){center, (mc_vec2){zoom, zoom}, iterations}
    );

    double time = mc_program_run_blocking(
        program,
        (mc_uvec3){width, height, 1},
        (mc_Buffer*[]){data, img, NULL}
    );

    printf("compute time: %f[s]\n", time);

    char* img_bytes = malloc(size * 4);
    mc_buffer_read(img, 0, sizeof(mc_int) * size, img_bytes);
    stbi_write_bmp("mandelbrot.bmp", width, height, 4, img_bytes);
    free(img_bytes);

    mc_buffer_destroy(data);
    mc_buffer_destroy(img);
    mc_program_destroy(program);
    mc_terminate();
}