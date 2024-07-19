#include <shaderc/shaderc.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "program_code.h"

mc_ProgramCode* mc_program_code_create_from_spirv(
    mc_Instance* instance,
    size_t size,
    const char* code
) {
    if (!instance) return NULL;

    mc_ProgramCode* programCode = malloc(sizeof *programCode);
    *programCode = (mc_ProgramCode){
        ._instance = instance,
        .size = size,
        .code = NULL,
    };

    DEBUG(
        programCode,
        "creating program from SPIR-V code, size: %d",
        programCode->size
    );

    memcpy(programCode->code, code, programCode->size);

    return programCode;
}

mc_ProgramCode* mc_program_code_create_from_glsl__(
    mc_Instance* instance,
    const char* name,
    const char* code,
    const char* entry,
    ...
) {
    if (!instance) return NULL;

    mc_ProgramCode* programCode = malloc(sizeof *programCode);
    *programCode = (mc_ProgramCode){
        ._instance = instance,
        .entry = NULL,
        .size = 0,
        .code = NULL,
    };

    DEBUG(
        programCode,
        "creating program from GLSL code, name: %s, entry point: %s",
        name,
        entry
    );

    if (!code) {
        ERROR(programCode, "code is NULL");
        mc_program_code_destroy(programCode);
        return NULL;
    }

    if (!entry) {
        ERROR(programCode, "entry is NULL");
        mc_program_code_destroy(programCode);
        return NULL;
    }

    shaderc_compile_options_t options = shaderc_compile_options_initialize();
    if (!options) {
        ERROR(programCode, "failed to initialize shader compiler options");
        mc_program_code_destroy(programCode);
        return NULL;
    }

    va_list args;
    va_start(args, entry);
    while (true) {
        mc_CompileDefinition option = va_arg(args, mc_CompileDefinition);
        if (!option.key || !option.value) break;

        DEBUG(
            programCode,
            "- defining \"%s\": \"%s\"",
            option.key,
            option.value
        );

        shaderc_compile_options_add_macro_definition(
            options,
            option.key,
            strlen(option.key),
            option.value,
            strlen(option.value)
        );
    }
    va_end(args);

    shaderc_compile_options_set_optimization_level(
        options,
        shaderc_optimization_level_performance
    );

    shaderc_compiler_t compiler = shaderc_compiler_initialize();
    if (!compiler) {
        ERROR(programCode, "failed to initialize shader compiler");
        mc_program_code_destroy(programCode);
        return NULL;
    }

    shaderc_compilation_result_t result = shaderc_compile_into_spv(
        compiler,
        code,
        strlen(code),
        shaderc_glsl_compute_shader,
        name,
        entry,
        options
    );

    shaderc_compile_options_release(options);

    if (shaderc_result_get_num_errors(result)
        || shaderc_result_get_num_warnings(result)) {
        ERROR(programCode, "failed to compile shader code:");
        ERROR(
            programCode,
            "errors:\n%s",
            shaderc_result_get_error_message(result)
        );
        ERROR(programCode, "code:\n```\n%s\n```", code);
        shaderc_result_release(result);
        shaderc_compiler_release(compiler);
        mc_program_code_destroy(programCode);
        return NULL;
    }

    programCode->size = shaderc_result_get_length(result);
    programCode->code = malloc(programCode->size);
    memcpy(
        programCode->code,
        shaderc_result_get_bytes(result),
        programCode->size
    );

    shaderc_result_release(result);
    shaderc_compiler_release(compiler);

    programCode->entry = malloc(strlen(entry) + 1);
    memcpy(programCode->entry, entry, strlen(entry) + 1);

    return programCode;
}

void mc_program_code_destroy(mc_ProgramCode* programCode) {
    if (!programCode) return;
    DEBUG(programCode, "destroying program code");
    if (programCode->code) free(programCode->code);
    if (programCode->entry) free(programCode->entry);
    free(programCode);
}