#include <microcompute.h>
#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

static char* renSrc = //
    "#version 430\n"
    "layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;\n"
    "layout(std430, binding = 0) buffer buff1 {\n"
    "    vec4 floatData[];\n"
    "};\n"
    "uniform float maxIter;\n"
    "uniform vec2 center;\n"
    "uniform vec2 zoom;\n"
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
    "    floatData[(pos.y * size.x + pos.x)] = vec4(color, \n"
    "0.5 * color, 0.5 * color, 1.0);\n"
    "}\n";

static char* convSrc = //
    "#version 430\n"
    "layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;\n"
    "layout(std430, binding = 0) buffer buff1 {\n"
    "    vec4 floatData[];\n"
    "};\n"
    "layout(std430, binding = 1) buffer buff2 {\n"
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

int main(void) {
    mc_Result res;

    int maxErrLen = 2048;
    char error[maxErrLen];
    int width = 800;
    int height = 600;
    int pixels = width * height;

    res = mc_start("/dev/dri/renderD129");
    if (!res.ok) {
        mc_result_pretty_print(res);
        return -1;
    }

    mc_Program* renderProg = mc_program_from_string(renSrc, maxErrLen, error);
    if (renderProg == NULL) {
        printf("error: %s\n", error);
        return -1;
    }

    mc_Program* convertProg = mc_program_from_string(convSrc, maxErrLen, error);
    if (convertProg == NULL) {
        printf("error: %s\n", error);
        return -1;
    }

    mc_Buffer* floatImage = mc_buffer_create(sizeof(float) * pixels * 4);
    mc_buffer_write(floatImage, 0, sizeof(float) * pixels * 4, NULL);

    mc_Buffer* byteImage = mc_buffer_create(sizeof(int) * pixels);
    mc_buffer_write(byteImage, 0, sizeof(int) * pixels, NULL);

    mc_program_set_float(renderProg, "maxIter", 500);
    mc_program_set_vec2(renderProg, "center", (mc_vec2){-0.7615, -0.08459});
    mc_program_set_vec2(renderProg, "zoom", (mc_vec2){1000, 1000});

    mc_program_dispatch(
        renderProg,
        (mc_ivec3){width, height, 1},
        1,
        (mc_Buffer*[]){floatImage}
    );

    mc_program_dispatch(
        convertProg,
        (mc_ivec3){width, height, 1},
        2,
        (mc_Buffer*[]){floatImage, byteImage}
    );

    char* data = malloc(pixels * 4);
    mc_buffer_read(byteImage, 0, pixels * 4, data);
    stbi_write_bmp("tmp/mandelbrot.bmp", width, height, 4, data);
    free(data);

    mc_buffer_destroy(floatImage);
    mc_buffer_destroy(byteImage);
    mc_program_destroy(renderProg);
    mc_program_destroy(convertProg);
    mc_stop();
}