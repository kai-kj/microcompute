#pragma once

#include <stdbool.h>
#include <stdlib.h>

typedef struct mc_vec2 {
	float x, y;
} mc_vec2;

typedef struct mc_vec3 {
	float x, y, z;
} mc_vec3;

typedef struct mc_vec4 {
	float x, y, z, w;
} mc_vec4;

typedef struct mc_ivec2 {
	int x, y;
} mc_ivec2;

typedef struct mc_ivec3 {
	int x, y, z;
} mc_ivec3;

typedef struct mc_ivec4 {
	int x, y, z, w;
} mc_ivec4;

typedef struct mc_uvec2 {
	unsigned int x, y;
} mc_uvec2;

typedef struct mc_uvec3 {
	unsigned int x, y, z;
} mc_uvec3;

typedef struct mc_uvec4 {
	unsigned int x, y, z, w;
} mc_uvec4;

typedef struct mc_mat22 {
	float values[4];
	bool transpose;
} mc_mat22;

typedef struct mc_mat33 {
	float values[9];
	bool transpose;
} mc_mat33;

typedef struct mc_mat44 {
	float values[16];
	bool transpose;
} mc_mat44;

typedef struct mc_mat23 {
	float values[6];
	bool transpose;
} mc_mat23;

typedef struct mc_mat32 {
	float values[6];
	bool transpose;
} mc_mat32;

typedef struct mc_mat24 {
	float values[8];
	bool transpose;
} mc_mat24;

typedef struct mc_mat42 {
	float values[8];
	bool transpose;
} mc_mat42;

typedef struct mc_mat34 {
	float values[12];
	bool transpose;
} mc_mat34;

typedef struct mc_mat43 {
	float values[12];
	bool transpose;
} mc_mat43;

typedef enum mc_DebugLevel {
	mc_DebugLevel_INFO,
	mc_DebugLevel_LOW,
	mc_DebugLevel_MEDIUM,
	mc_DebugLevel_HIGH,
} mc_DebugLevel;

typedef struct mc_Program mc_Program;
typedef struct mc_Buffer mc_Buffer;

//============================================================================//
// core
//============================================================================//

int mc_start(char *renderDevice);
void mc_stop();

void mc_set_debug_callback(void (*callback)(mc_DebugLevel, char *));
void mc_default_debug_callback(mc_DebugLevel level, char *message);

//============================================================================//
// program management
//============================================================================//

mc_Program *mc_program_from_str(const char *programCode);
mc_Program *mc_program_from_file(const char *filePath);
void mc_program_destroy(mc_Program *program);

void mc_program_dispatch(mc_Program *program, mc_ivec3 size);

//============================================================================//
// uniform value management
//============================================================================//

void mc_program_set_float(mc_Program *program, char *name, float value);
void mc_program_set_vec2(mc_Program *program, char *name, mc_vec2 value);
void mc_program_set_vec3(mc_Program *program, char *name, mc_vec3 value);
void mc_program_set_vec4(mc_Program *program, char *name, mc_vec4 value);

void mc_program_set_int(mc_Program *program, char *name, int value);
void mc_program_set_ivec2(mc_Program *program, char *name, mc_ivec2 value);
void mc_program_set_ivec3(mc_Program *program, char *name, mc_ivec3 value);
void mc_program_set_ivec4(mc_Program *program, char *name, mc_ivec4 value);

void mc_program_set_uint(mc_Program *program, char *name, unsigned int value);
void mc_program_set_uvec2(mc_Program *program, char *name, mc_uvec2 value);
void mc_program_set_uvec3(mc_Program *program, char *name, mc_uvec3 value);
void mc_program_set_uvec4(mc_Program *program, char *name, mc_uvec4 value);

void mc_program_set_mat22(mc_Program *program, char *name, mc_mat22 value);
void mc_program_set_mat33(mc_Program *program, char *name, mc_mat33 value);
void mc_program_set_mat44(mc_Program *program, char *name, mc_mat44 value);
void mc_program_set_mat23(mc_Program *program, char *name, mc_mat23 value);
void mc_program_set_mat32(mc_Program *program, char *name, mc_mat32 value);
void mc_program_set_mat24(mc_Program *program, char *name, mc_mat24 value);
void mc_program_set_mat42(mc_Program *program, char *name, mc_mat42 value);
void mc_program_set_mat34(mc_Program *program, char *name, mc_mat34 value);
void mc_program_set_mat43(mc_Program *program, char *name, mc_mat43 value);

//============================================================================//
// buffer management
//============================================================================//

mc_Buffer *mc_buffer_create(int binding, size_t size);
void mc_buffer_destroy(mc_Buffer *buffer);

void mc_buffer_rebind(mc_Buffer *buffer, int binding);
void mc_buffer_resize(mc_Buffer *buffer, size_t size);

size_t mc_buffer_get_size(mc_Buffer *buffer);

size_t mc_buffer_write(mc_Buffer *buffer, size_t off, size_t size, void *data);
size_t mc_buffer_read(mc_Buffer *buffer, size_t off, size_t size, void *data);