#pragma once

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

int mc_init(char *renderDevice);
void mc_terminate();

void mc_set_debug_callback(void (*callback)(mc_DebugLevel, char *));

//============================================================================//
// program management
//============================================================================//

mc_Program *mc_program_create_from_str(const char *programCode);
mc_Program *mc_program_create_from_file(const char *programPath);
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

//============================================================================//
// buffer management
//============================================================================//

mc_Buffer *mc_buffer_create(int index);
void mc_buffer_destroy(mc_Buffer *buffer);

void mc_buffer_write(mc_Buffer *buffer, size_t size, void *data);
void mc_buffer_modify(mc_Buffer *buffer, size_t off, size_t size, void *data);

void mc_buffer_read(mc_Buffer *buffer, size_t off, size_t size, void *data);