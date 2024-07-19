#ifndef PROGRAM_CODE_H
#define PROGRAM_CODE_H

#include "microcompute.h"

typedef struct mc_ProgramCode {
    mc_Instance* _instance;
    char* entry;
    size_t size;
    char* code;
} mc_ProgramCode;

#endif // PROGRAM_CODE_H
