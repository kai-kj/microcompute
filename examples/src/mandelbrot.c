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
    "    vec4 floatData[];\n"
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
    "    float color = float(i) / float(maxIter);\n"
    "    floatData[(pos.y * size.x + pos.x)]\n"
    "        = vec4(color, 0.5 * color, 0.5 * color, 1.0);\n"
    "}\n";

static char* convSrc = //
    "#version 430\n"
    "layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;\n"
    "layout(std430, binding = 0) buffer buff0 {\n"
    "    vec4 floatData[];\n"
    "};\n"
    "layout(std430, binding = 1) buffer buff1 {\n"
    "    int byteData[];\n"
    "};\n"
    "void main(void) {\n"
    "    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);\n"
    "    ivec2 size = ivec2(gl_NumWorkGroups.xy);\n"
    "    ivec4 col = clamp(\n"
    "        ivec4(floatData[(pos.y * size.x + pos.x)] * 255.0),\n"
    "        0, 255\n"
    "    );\n"
    "    int bytes = col.r << 0 | col.g << 8 | col.b << 16 | col.a << 24;\n"
    "    byteData[(pos.y * size.x + pos.x)] = bytes;\n"
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
    char* error = NULL;
    int width = 1000;
    int height = 1000;
    int pixels = width * height;

    if ((error = mc_start(debug_cb, NULL)) != NULL) {
        printf("error at mc_start: %s\n", error);
        return -1;
    }

    mc_Program* renderProg = mc_program_from_string(renSrc);
    if ((error = mc_program_check(renderProg)) != NULL) {
        printf("error at mc_program_from_string: %s\n", error);
        return -1;
    }

    mc_Program* convertProg = mc_program_from_string(convSrc);
    if ((error = mc_program_check(convertProg)) != NULL) {
        printf("error at mc_program_from_string: %s\n", error);
        return -1;
    }

    mc_Buffer* shaderData
        = mc_buffer_create(MC_BUFFER_UNIFORM, sizeof(ShaderData));
    mc_Buffer* floatImage
        = mc_buffer_create(MC_BUFFER_STORAGE, sizeof(mc_vec4) * pixels);
    mc_Buffer* byteImage
        = mc_buffer_create(MC_BUFFER_STORAGE, sizeof(mc_int) * pixels);

    mc_buffer_write(
        shaderData,
        0,
        sizeof(ShaderData),
        &(ShaderData){(mc_vec2){-0.7615, -0.08459}, (mc_vec2){1000, 1000}, 1000}
    );

    double time1 = mc_program_run_timed(
        renderProg,
        (mc_uvec3){width, height, 1},
        (mc_Buffer*[]){shaderData, floatImage, NULL}
    );

    double time2 = mc_program_run_timed(
        convertProg,
        (mc_uvec3){width, height, 1},
        (mc_Buffer*[]){floatImage, byteImage, NULL}
    );

    printf("%f, %f\n", time1, time2);

    char* data = malloc(pixels * 4);
    mc_buffer_read(byteImage, 0, sizeof(mc_int) * pixels, data);
    stbi_write_bmp("mandelbrot.bmp", width, height, 4, data);
    free(data);

    mc_buffer_destroy(shaderData);
    mc_buffer_destroy(byteImage);
    mc_program_destroy(renderProg);
    mc_program_destroy(convertProg);
    mc_stop();
}