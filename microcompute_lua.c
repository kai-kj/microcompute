#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define MICROCOMPUTE_IMPLEMENTATION
#include "microcompute.h"

typedef struct Databuff {
    int size;
    int pos;
    float* data;
} Databuff;

static void debug_cb(
    mc_DebugLevel level,
    const char* msg,
    void(cb)(int level, const char* msg)
) {
    if (cb) cb(level, msg);
}

static int calc_offset(int pos, int align) {
    return (align - pos % align) % align;
}

int is_null(void* ptr) {
    return ptr == NULL;
}

void* make_null() {
    return NULL;
}

Databuff* databuff_create(int size) {
    Databuff* databuff = malloc(sizeof *databuff);
    databuff->size = size;
    databuff->pos = 0;
    databuff->data = malloc(sizeof(float) * size);
    for (int i = 0; i < size; i++) databuff->data[i] = 0;
    return databuff;
}

float databuff_read(Databuff* databuff, int align) {
    databuff->pos += calc_offset(databuff->pos, align) + 1;
    return databuff->data[databuff->pos - 1];
}

void databuff_write(Databuff* databuff, int align, float value) {
    databuff->pos += calc_offset(databuff->pos, align) + 1;
    databuff->data[databuff->pos - 1] = value;
}

void databuff_destroy(Databuff* databuff) {
    free(databuff->data);
    free(databuff);
}

void lmc_buffer_write(mc_Buffer* buffer, Databuff* databuff) {
    mc_buffer_set_size(buffer, databuff->size * 4);
    mc_buffer_write(buffer, 0, databuff->size * 4, databuff->data);
}

void lmc_buffer_read(mc_Buffer* buffer, Databuff* databuff) {
    mc_buffer_read(buffer, 0, databuff->size * 4, databuff->data);
}

char* lmc_initialize(void(cb)(int level, const char* msg)) {
    char* ret = mc_initialize((mc_debug_cb*)debug_cb, (void*)cb);
    return ret ? ret : "success";
}

void lmc_program_run(mc_Program* program, int x, int y, int z, ...) {
    va_list args;
    va_start(args, z);
    mc_program_run__(program, (mc_uvec3){x, y, z}, args);
    va_end(args);
}