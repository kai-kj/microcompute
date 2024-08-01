#include <stdio.h>
#include <stdlib.h>

#include "microcompute.h"
// #include "microcompute_extra.h"

#define SHADER_PATH "../examples/check_devs.glsl"

int main(void) {
    mc_Instance* instance = mc_instance_create(mc_log_cb_simple, NULL);
    mc_Device** devs = mc_instance_get_devices(instance);

    // char* programSource = read_file(SHADER_PATH, NULL);
    // mc_ProgramCode* programCode
    //     = mc_program_code_create_from_glsl(instance, SHADER_PATH, programSource, "main");
    //
    // printf("\nprogram source code:\n\n```\n%s\n```\n", programSource);

    printf("\n%d supported device(s) found\n\n", mc_instance_get_device_count(instance));

    // for (uint32_t i = 0; i < mc_instance_get_device_count(instance); i++) {
    //     mc_Device* dev = devs[i];
    //     mc_DeviceProperties* devProps = mc_device_get_properties(dev);
    //
    //     printf("=== %s ===\n", devProps->name);
    //     printf("- type: %s\n", mc_device_type_to_str(devProps->type));
    //     printf("- testing (values should be doubled every iteration):\n");

    // float arr[] = {0.0f, 1.0f, 2.0f, 3.0f, 4.0f};
    // mc_Buffer* buff = mc_buffer_create(dev, MC_BUFFER_TYPE_CPU, sizeof arr);
    // //
    // // mc_Program* program = mc_program_create(dev, programCode);
    // //
    // // for (uint32_t j = 0; j < 3; j++) {
    // //     mc_program_run(program, 5, 1, 1, buff);
    // //     mc_hybrid_buffer_read(buff, 0, sizeof arr, arr);
    // //
    // //     printf("  - iteration %d: {", j + 1);
    // //     for (uint32_t k = 0; k < 5; k++) printf("%f%s", arr[k], k != 4 ? ", " : "");
    // //     printf("}\n");
    // // }
    // //
    // // printf("\n");
    // //
    // mc_buffer_destroy(buff);
    // mc_program_destroy(program);
    // }

    // mc_program_code_destroy(programCode);
    // free(programSource);
    mc_instance_destroy(instance);
}
