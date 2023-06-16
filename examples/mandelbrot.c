#include <stdio.h>

#define MICROCOMPUTE_IMPLEMENTATION
#include "microcompute.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

static char* renSrc = //
    "#version 430\n"
    "layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;\n"
    "layout(std140, binding = 0) uniform dBuff {\n"
    "    int maxIter;\n"
    "    vec2 center;\n"
    "    float zoom;\n"
    "};\n"
    "layout(std430, binding = 1) buffer iBuff {\n"
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

typedef struct {
    mc_int maxIter;
    mc_vec2 center;
    mc_float zoom;
} ShaderData;

void debug_cb(mc_DebugLevel level, const char* msg, void* arg) {
    printf("%d: %s\n", level, msg);
}

int main(void) {
    mc_uvec3 size = (mc_uvec3){1000, 1000, 1};
    mc_vec2 center = (mc_vec2){-0.7615, -0.08459};
    mc_float zoom = 1000;
    mc_int maxIter = 500;

    char* error = NULL;
    int totalSize = size.x * size.y;

    if ((error = mc_initialize(debug_cb, NULL)) != NULL) {
        printf("error at mc_initialize: %s\n", error);
        return -1;
    }

    mc_Program* program = mc_program_create(renSrc);
    if ((error = mc_program_check(program)) != NULL) {
        printf("error at mc_program_create:\n%s\n", error);
        return -1;
    }

    mc_Buffer* dBuff = mc_buffer_create_uniform(sizeof(ShaderData));
    mc_Buffer* iBuff = mc_buffer_create_storage(sizeof(mc_int) * totalSize);

    mc_buffer_pack(dBuff, MC_INT, &maxIter, MC_VEC2, &center, MC_FLOAT, &zoom);

    double time = mc_program_run_blocking(program, size, dBuff, iBuff);

    printf("compute time: %f[s]\n", time);

    char img_bytes[totalSize * 4];
    mc_buffer_read(iBuff, 0, sizeof(mc_int) * totalSize, img_bytes);
    stbi_write_bmp("mandelbrot.bmp", size.x, size.y, 4, img_bytes);

    mc_buffer_destroy(dBuff);
    mc_buffer_destroy(iBuff);
    mc_program_destroy(program);
    mc_terminate();
}
