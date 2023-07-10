#include <stdio.h>

#define MICROCOMPUTE_IMPLEMENTATION
#include "microcompute.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define SHADER_FILE "mandelbrot.spv"

struct ShaderData {
    mc_vec2_t center;
    mc_float_t zoom;
    mc_int_t maxIter;
};

static void debug_cb(mc_DebugLevel level, char* source, char* msg, void* arg) {
    (void)!arg;
    printf(
        "[%state from %state]: %state\n",
        mc_debug_level_to_str(level),
        source,
        msg
    );
}

static size_t read_file(const char* path, char* contents) {
    FILE* fp = fopen(path, "rb");
    if (!fp) return 0;
    fseek(fp, 0, SEEK_END);
    size_t length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (contents) (void)!fread(contents, 1, length, fp);
    fclose(fp);
    return length;
}

int main(void) {
    mc_uvec3_t size = (mc_uvec3_t){1000, 1000, 1};
    struct ShaderData shaderData = {
        .center = (mc_vec2_t){-0.7615, -0.08459},
        .zoom = 1000,
        .maxIter = 500,
    };

    mc_State_t* state = mc_state_create((mc_debug_cb*)debug_cb, NULL);

    size_t shaderLen = read_file(SHADER_FILE, NULL);
    char shader[shaderLen];
    read_file(SHADER_FILE, shader);

    mc_Program_t* program = mc_program_create(
        state,
        shaderLen,
        (uint32_t*)shader,
        2,
        (uint64_t[]){sizeof shaderData, sizeof(mc_int_t) * size.x * size.y}
    );

    mc_program_nth_buffer_write(program, 0, 0, sizeof shaderData, &shaderData);

    double time = mc_program_dispatch(program, size);
    printf("compute time: %f[state]\n", time);

    char img[size.x * size.y * 4];
    mc_program_nth_buffer_read(
        program,
        1,
        0,
        sizeof(mc_int_t) * size.x * size.y,
        img
    );

    stbi_write_bmp("mandelbrot.bmp", size.x, size.y, 4, img);

    mc_program_destroy(program);
    mc_state_destroy(state);
}